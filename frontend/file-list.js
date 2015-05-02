var fileListUrl = "http://127.0.0.1:8000";

function fileListRetrieveList(path, parentElement)
{
	fileListLoadingAnimation(parentElement);
	fileListRetrieve(path, parentElement, function(response)
	{
		fileListClear(parentElement);
		
		var table = document.createElement("table");
		parentElement.appendChild(table);
		var trTh = document.createElement("tr");
		table.appendChild(trTh);
		var thName = document.createElement("th");
		thName.innerHTML = "Name";
		trTh.appendChild(thName);
		var thSize = document.createElement("th");
		thSize.innerHTML = "Size";
		trTh.appendChild(thSize);
		var thMime = document.createElement("th");
		thMime.innerHTML = "MIME";
		trTh.appendChild(thMime);
		var thLastModified = document.createElement("th");
		thLastModified.innerHTML = "Last Modified";
		trTh.appendChild(thLastModified);
		
		response.files.forEach(function(value, key, array)
		{
			var tr = document.createElement("tr");
			table.appendChild(tr);
			var tdName = document.createElement("td");
			tdName.innerHTML = value.name;
			tr.appendChild(tdName);
			var tdSize = document.createElement("td");
			tdSize.innerHTML = value.size;
			tr.appendChild(tdSize);
			var tdMime = document.createElement("td");
			tdMime.innerHTML = value.mime;
			tr.appendChild(tdMime);
			var tdLastModified = document.createElement("td");
			tdLastModified.innerHTML = value.name;
			tr.appendChild(tdLastModified);
		});
	});
}

function fileListRetrieve(path, parentElement, callback)
{
	
	requestPOST(fileListUrl, JSON.stringify({ path: path }), function(responseJSON)
	{
		var response = JSON.parse(responseJSON);
		
		if(response.status == false)
		{
			fileListClear(parentElement);
			parentElement.innerHTML = response.reason;
		}
		else
		{
			callback(response);
			
			// var ul = document.createElement("ul");
			// parentElement.appendChild(ul);
			// response.files.forEach(function(value, key, array)
			// {
			// 	console.log(value.name);
			// 	var li = document.createElement("li");
			// 	li.innerHTML = value.name;
			// 	ul.appendChild(li);
			// });
		}
	});
}

function fileListLoadingAnimation(parentElement)
{
	parentElement.innerHTML = "Loading...";
}

function fileListClear(parentElement)
{
	while(parentElement.firstChild)
	{
		parentElement.removeChild(parentElement.firstChild);
	}
}