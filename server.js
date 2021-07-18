const scratchers=require("./scratchers.js");
const cors=require("cors");
const express=require("express");
const server=express();
server.use(express.static("public"));
server.get("/",cors(),(request,response)=>{
	response.sendFile(__dirname + "/public/index.html")});
server.get("/api", cors(), async (request, response) => {
	console.log(request.ip, request.path, request.query);
	response.json(
		await (
			scratchers.fetchListWithPayouts(
				request.query)))});
server.listen(3000)
		
	
