function init() {
	//set images
	var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
			document.getElementById("favicon").href = xmlHttp.responseText;
			let elements = [].slice.call(document.getElementsByClassName("logoimg"));
			let hr = document.getElementById("favicon").href;
			elements.forEach(element => {
				element.src = hr;
			});
			console.log("Initialized...")
    }
    xmlHttp.open("GET", "image", true); // true for asynchronous 
    xmlHttp.send(null);

	setContentOptions();
	updateDisplay();
}

function updateDisplay() {
	document.getElementById("selected_content").style.display="";
	let elements = [].slice.call(document.getElementsByClassName("content"));
	elements.forEach(element => {
		if(element.id!="selected_content")
			element.style.display="none";
	});
}

function setContentOptions() {
	let index_div = document.getElementById("index_div");
	let elements = [].slice.call(document.getElementsByClassName("content"));
	elements.forEach(element => {
		let button = document.createElement("button");
		button.innerHTML = element.getAttribute("name");
		button.className = "content_button";
		button.disabled = element.getAttribute("id")==="selected_content" ? true:false;
		button.onclick = function() {
			let buttons = [].slice.call(document.getElementsByClassName("content_button"));
			buttons.forEach(btn => {
				if(btn.innerHTML!= element.getAttribute("name"))
					btn.disabled = false;
				else
					btn.disabled = true;
			})
			document.getElementById("selected_content").id="";
			document.getElementsByName(element.getAttribute("name"))[0].id="selected_content";
			updateDisplay();
		};
		index_div.appendChild(button);
	});
}