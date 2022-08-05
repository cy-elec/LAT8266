function init() {
	//set images
	var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
			document.getElementById("favicon").href = xmlHttp.responseText;
			let elements = [].slice.call(document.getElementsByClassName("logoimg"));
			let hr = document.getElementById("favicon").href;
			elements.forEach(element => {
				element.src = hr;
			});
			console.log("Initialized...");
		}
    }
    xmlHttp.open("GET", "image", true); // true for asynchronous 
    xmlHttp.send(null);

	setContentOptions();
	updateDisplay();

	let response = document.getElementById("reflect_result");
	response.textContent="";
	response.value="";
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

function reflects() {
	let input = document.getElementById("reflect_input");
	//form post request
	var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4) {
			let setr="";
			if(xmlHttp.status == 202) {
				receive(input, xmlHttp.response);
			}
			else if(xmlHttp.status == 200) {
				let response = document.getElementById("reflect_result");
				response.value=response.value + "<<" + xmlHttp.response;
				response.scrollTop = response.scrollHeight;
				let wait = document.getElementById("reflect_wait");
				wait.removeAttribute("required");
			}
			else {
				setr="Couldn't retrieve response";
				if(xmlHttp.response=="-1")
					setr="Server is too busy right now. Please try again later";
				if(xmlHttp.status==401)
					setr="Interface is currently disabled";
				let response = document.getElementById("reflect_result");
				response.value=response.value + "<<" + setr + "\n";
				response.scrollTop = response.scrollHeight;
				let wait = document.getElementById("reflect_wait");
				wait.removeAttribute("required");
			}
		}
    }
	let formData = new FormData();
	formData.append("reflect", input.value);
    xmlHttp.open("POST", "reflect", true); // true for asynchronous 
    xmlHttp.send(formData);
	let response = document.getElementById("reflect_result");
	response.value=response.value + ">>" + input.value +"\n";
	response.scrollTop = response.scrollHeight;
	input.value="";
	let wait = document.getElementById("reflect_wait");
	wait.setAttribute("required", "");
}

function receive(input, id) {
	//form put request
	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function() {
		if(xmlHttp.readyState==4) {
			if(xmlHttp.status==200) {
				let response = document.getElementById("reflect_result");
				response.value=response.value + "<<" + xmlHttp.response;
				response.scrollTop = response.scrollHeight;
				let wait = document.getElementById("reflect_wait");
				wait.removeAttribute("required");
			}
			else if(xmlHttp.status==408) {
				let response = document.getElementById("reflect_result");
				response.value=response.value + "<< ERROR: id invalid\n";
				response.scrollTop = response.scrollHeight;
				let wait = document.getElementById("reflect_wait");
				wait.removeAttribute("required");
			}
			else {
				setTimeout(function(){
					receive(input, id);
				}, 500);
			}
		}
	}
	let formData = new FormData();
	formData.append("id", id);
	xmlHttp.open("PUT", "reflect", true);
	xmlHttp.send(formData);
}