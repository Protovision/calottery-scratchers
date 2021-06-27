(async () => {
	try {
		const fetch = require("node-fetch");
		const jsdom = require("jsdom");
		const { JSDOM } = jsdom;
		const fetchPayoutRatio = async (price, scratcherProductPage) =>
			Array.prototype.slice.call(
				JSDOM.fragment(
					await (
						(await fetch("https://www.calottery.com" + scratcherProductPage))
						.text()))
				.querySelectorAll(".odds-available-prizes__table__body"))
			.map(
				(currentElement) => ({
					prize: Number(
						currentElement
						.children[0]
						.textContent
						.replace(/\$|,/g, "")),
					odds: Number(
						currentElement	
						.children[1]
						.textContent
						.replace(/\$|,/g, ""))}))
			.filter(
				(currentElement) => 
					!(Number.isNaN(currentElement.prize)
						|| Number.isNaN(currentElement.odds)))
			.reduce(
				(payout, currentElement) =>
					payout + currentElement.prize / currentElement.odds
				, 0.0)
			/ price
			* 100.0;
		const main = async () => {
			let processedScratchers = 0;
			console.log("Downloading scratcher data...");
			const scratcherData = await (
				(await fetch(
					"https://www.calottery.com/" +
					"api/Sitecore/ScratchersFilteredList/GetScratchers?size=999"))
				.json());
			const totalScratchers = scratcherData.TotalScratcherCards;
			(await (
				Promise.all(
					scratcherData
					.SerializedScratcherCardList
					.map((scratcher) => ({
						name: scratcher.GameName,
						number: scratcher.GameNumber,
						page: scratcher.GameProductPage,
						price: Number(scratcher.GamePrice.replace(/\$/g, ""))}))
					.map(async (scratcher) => ({
						name: scratcher.name,
						number: scratcher.number,
						price: scratcher.price,
						payoutRatio: await (
							fetchPayoutRatio(scratcher.price, scratcher.page)
							.then(
								(payoutRatio) => {
									console.log(
										++processedScratchers
										,"of"
										,totalScratchers 
										,":"
										,scratcher.page);
									return payoutRatio}))})))))
			.sort((leftScratcher, rightScratcher) =>
				leftScratcher.payoutRatio - rightScratcher.payoutRatio)
			.forEach((scratcher) =>
				console.log(
					scratcher.name.padEnd(32)
					,String(scratcher.number).padEnd(4)
					,("$" + scratcher.price).padEnd(4)
					,scratcher.payoutRatio.toPrecision(4) + "%"));};
		await main()
	} catch (error) {
		console.error(error);
		process.exit(1)}
})();
			
