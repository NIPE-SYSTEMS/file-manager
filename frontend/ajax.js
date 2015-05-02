function requestPOST(url, data, callback)
{
	var request = new XMLHttpRequest();
	request.onreadystatechange = function()
	{
		if(request.readyState == 4 && request.status == 200)
		{
			callback(request.responseText);
		}
	};
	request.open("POST", url, true);
	request.send(data);
}