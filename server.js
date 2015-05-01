var http = require("http");
var url = require("url");

http.createServer(function (request, response)
{
	var postData = "";
	
	console.log(" -> " + request.connection.remoteAddress + " \x1b[1m" + request.method + "\x1b[0m " + url.parse(request.url).pathname);
	
	if(request.method == "GET")
	{
		if(url.parse(request.url).pathname == "/")
		{
			response.writeHead(200, { "Content-Type": "text/plain" });
			response.end("TODO: Insert regular index file.");
		}
		else
		{
			response.writeHead(404,
				{
					"Content-Type": "text/plain",
					"Content-Length": "404 Not Found".length
				});
			response.end("404 Not Found");
		}
		
		console.log("<-  \x1b[1m" + response.statusCode + "\x1b[0m " + url.parse(request.url).pathname);
	}
	else if(request.method == "POST")
	{
		request.on("data", function(chunk)
		{
			postData += chunk;
		});
		
		request.on("end", function()
		{
			if(postData.length > 0)
			{
				console.log("    sent data: " + postData);
				
				try
				{
					var requestData = JSON.parse(postData);
					
					
				}
				catch(e)
				{
					console.log("Cannot parse JSON");
				}
				
				response.writeHead(200, { "Content-Type": "text/plain" });
				response.write("TODO: Insert regular index file.");
				response.end();
			}
			else
			{
				response.writeHead(400,
					{
						"Content-Type": "text/plain",
						"Content-Length": "400 Bad Request".length
					});
				response.end("400 Bad Request");
			}
		});
	}
}).listen(8000, "127.0.0.1");