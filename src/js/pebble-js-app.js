var initialized = false;



// app message sent successfully
//
function appMessageAck(e) {
	console.log("App message sent successfully");
}


// app message failed to send
//
function appMessageNack(e) {
	console.log("App message failed to send");
}


// app ready event
//
Pebble.addEventListener("ready",
	function(e) {
		console.log("connect! [" + e.ready + "] (" + e.type + ")");
	}
);


Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
	 var configUrl = "https://f8171218526fd80c808fab6703bb496af0b21e1a.googledrive.com/host/0B06_GUk833sLLXZHVldVWWFmeU0/configuration.html";


		//Get the settings store on the phone
		var settings = localStorage.getItem("settings");
		var numberOfKeys = 3;

		var firstIndex = 0;
		var secondIndex = 0;
		
		var keyList = ["?showdate=", "&dateformat=", "&invert="]; 

		//Loop through settings output and parse the key values
		for (i = 0; i < numberOfKeys; i++) {
    			firstIndex = settings.indexOf(":", secondIndex);
			
			//The last key is followed by '}' instead of ','
			if(i == numberOfKeys-1){
				secondIndex = settings.indexOf("}", firstIndex);
			} else {
				secondIndex = settings.indexOf(",", firstIndex);
			}
			
			//Create the URL
			configUrl = configUrl + keyList[i] + settings.substring(firstIndex+2, secondIndex-1); 

		}	

		console.log("Pre Settings: " + localStorage.getItem("settings"));
		console.log("Opening Config: " + configUrl);
		Pebble.openURL(configUrl);
	}
);
// close configuration screen
//
Pebble.addEventListener("webviewclosed",
	function(e) {
		var settings;
		try {
			console.log("Local Settings Before Clear: " + window.localStorage.getItem("settings"));
			settings = JSON.parse(decodeURIComponent(e.response));
			window.localStorage.clear();
			window.localStorage.setItem("settings", JSON.stringify(settings));
			console.log("Local Settings: " + window.localStorage.getItem("settings"));
			console.log("Stringify Settings: " + JSON.stringify(settings));
			Pebble.sendAppMessage(settings, appMessageAck, appMessageNack);
		} catch(err) {
			settings = false;
			console.log("No JSON response or received Cancel event");
		}
	}
);

