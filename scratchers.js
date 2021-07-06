"use strict";
const fetch = require("node-fetch");
const jsdom  = require("jsdom");
const deepFreeze = require("deep-freeze-strict");
const scratcherStatusStrings = deepFreeze([
		"Newest Games",
		"Hot Sellers",
		"Games Ending Soon"]);
const scratcherTypeStrings = deepFreeze([
		"Crossword",
		"Poker",
		"Bingo",
		"Tic-Tac-Toe",
		"Match 3",
		"Key Number Match",
		"Key Number Match and Match 3",
		"Find Legend",
		"Match Up"]);
exports.fetchList = deepFreeze(
	async (filter = null) => (
		await ((
				await fetch(
					"https://www.calottery.com/" +
					"api/Sitecore/ScratchersFilteredList/GetScratchers?size=999"))
			.json()))
	.SerializedScratcherCardList
	.map((scratcher) => ({
				name: scratcher.GameName,
				number: scratcher.GameNumber,
				price: Number(scratcher.GamePrice.replace(/\$/g, "")),
				imageLink: scratcher.ScratchersImage,
				productLink: "https://www.calottery.com" + scratcher.GameProductPage,
				type: scratcher.GameType,
				status: scratcher.GameShows}))
	.filter((scratcher) =>
		!filter 
		|| ((
				!(filter.status)
				|| filter.status == "Any"
				|| (
					Array.isArray(filter.status)
					&& (
						!(filter.status.length)
						|| scratcher.status.some(
							(s) => filter.status.includes(s))))
				|| scratcher.status.includes(filter.status))
			&& (
				!(filter.type)
				|| filter.type == "Any"
				|| (
					Array.isArray(filter.type)
					&& (
						!(filter.type.length)
						|| filter.type.includes(scratcher.type)))
				|| scratcher.type == filter.type)
			&& (
				!(filter.price)
				|| filter.price == "Any"
				|| (
					Array.isArray(filter.price)
					&& (
						!(filter.price.length)
						|| filter.price.includes(scratcher.price)))
				|| scratcher.price == filter.price))));
exports.fetchPayout = deepFreeze(
	async (price, productLink) => (
		Array.prototype.slice.call(
			jsdom.JSDOM.fragment(await ((await fetch(productLink)).text()))
			.querySelectorAll(".odds-available-prizes__table__body"))
		.map((currentElement) => ({
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
		.filter((currentElement) => !(
				Number.isNaN(currentElement.prize)
				|| Number.isNaN(currentElement.odds)))
		.reduce(
			((payout, currentElement) => (
					payout + currentElement.prize / currentElement.odds))
			, 0.0)
		/ price * 100.0));
exports.fetchListWithPayouts = deepFreeze(
	async (filter = null) => (
		await (Promise.all((
					await exports.fetchList(filter))
				.map(async (scratcher) => ({
							name: scratcher.name,
							number: scratcher.number,
							price: scratcher.price,
							imageLink: scratcher.imageLink,
							productLink: scratcher.productLink,
							type: scratcher.type,
							status: scratcher.status,
							payout: await (
								exports.fetchPayout(
									scratcher.price, scratcher.productLink))})))))
	.sort((leftScratcher, rightScratcher) =>
		rightScratcher.payout - leftScratcher.payout));
deepFreeze(module.exports)
