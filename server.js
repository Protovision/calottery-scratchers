const scratchers = require("./scratchers.js");
const cors = require("cors");
const express = require("express");
const server = express();
[	"/", 
	"/index.html", 
	"/client.js", 
	"/client.css", 
	"/reset.css",
	"/lds-dual-ring.css"]
.forEach((path) =>
	server.get(path, cors(), (request, response) => {
		if (request.path == "/") { request.path = "/index.html" };
		console.log(request.ip, request.path, request.query);
		response.sendFile(__dirname + request.path)}));
server.get("/api", cors(), async (request, response) => {
	console.log(request.ip, request.path, request.query);
	response.json(
		await (
			scratchers.fetchListWithPayouts(
				request.query)))});
server.listen(3000)
		
	
