var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // We will request the weather here
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
    pos.coords.latitude + '&lon=' + pos.coords.longitude;

  // Send request to OpenWeatherMap
	xhrRequest(url, 'GET', 
		function(responseText) {
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			// Conditions
			var conditions = json.weather[0].icon;      
			//console.log('Conditions are ' + conditions);

			// Assemble dictionary using our keys
			var dictionary = {
				'MK_WEATHER': conditions,
			};

			// Send to Pebble
			Pebble.sendAppMessage(dictionary,
				function(e) {
					//console.log('Weather info sent to Pebble successfully!');
				},
				function(e) {
					//console.log('Error sending weather info to Pebble!');
				}
			);
    }      
  );
}

function locationError(err) {
  //console.log('Error requesting location!');
}

function getWeather() {
  //console.log('Getting Weather!');
	navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}