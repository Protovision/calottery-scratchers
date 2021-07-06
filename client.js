document.addEventListener("DOMContentLoaded", (event) => {
		const api = document.querySelector("#api");
		const apiStatus = document.querySelector("#status");
		const apiType = document.querySelector("#type");
		const apiPrice = document.querySelector("#price");
		const ldsDualRing = document.querySelector(".lds-dual-ring");
		const table = document.querySelector("table");
		const results = document.querySelector("#results");
		api.addEventListener("submit", async (event) => {
				event.preventDefault();
				event.stopPropagation();
				table.classList.add("hidden");
				ldsDualRing.classList.remove("hidden");
				while (results.firstChild) { results.removeChild(results.firstChild) }
				let resource = "/api?";
				resource += "status=" + encodeURIComponent(apiStatus.value) + "&";
				resource += "type=" + encodeURIComponent(apiType.value) + "&";
				resource += "price=" + encodeURIComponent(apiPrice.value);
				try {(
						await (
							(await fetch(resource)).json()))
					.forEach((scratcher) => {
							const row = document.createElement("tr");
							const nameCell = document.createElement("td");
							nameCell.textContent = scratcher.name;
							const numberCell = document.createElement("td");
							numberCell.textContent = scratcher.number;
							const statusCell = document.createElement("td");
							statusCell.textContent = scratcher.status;
							const typeCell = document.createElement("td");
							typeCell.textContent = scratcher.type;
							const priceCell = document.createElement("td");
							priceCell.textContent = scratcher.price;
							const payoutCell = document.createElement("td");
							payoutCell.textContent = scratcher.payout;
							row.appendChild(nameCell);
							row.appendChild(numberCell);
							row.appendChild(statusCell);
							row.appendChild(typeCell);
							row.appendChild(priceCell);
							row.appendChild(payoutCell);
							results.appendChild(row);});
					ldsDualRing.classList.add("hidden");
					table.classList.remove("hidden")}
				catch (exception) {
					console.error(exception)}})})
					

