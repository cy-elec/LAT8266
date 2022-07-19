function init() {
	//set images
	var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
			document.getElementById("favicon").href = xmlHttp.responseText;
			document.getElementById("logoimg").src = document.getElementById("favicon").href;
			console.log("Initialized...")
    }
    xmlHttp.open("GET", "image", true); // true for asynchronous 
    xmlHttp.send(null);
}