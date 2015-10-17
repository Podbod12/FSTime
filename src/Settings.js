Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    //console.log("Configuration window returned: " + JSON.stringify(configuration));
 
		localStorage.setItem('MK_SETTINGS_CHAR', configuration.character);
		localStorage.setItem('MK_SETTINGS_TIMETYPE', configuration.fighttime);
		localStorage.setItem('MK_SETTINGS_TIMEEXACT', configuration.fightCustom);
		localStorage.setItem('MK_SETTINGS_FIGHTRUMBLEOFF', configuration.matchrumble);
		localStorage.setItem('MK_SETTINGS_USDATEFORMAT', configuration.dateformat);
		localStorage.setItem('MK_SETTINGS_SLEEPSTART', configuration.sleepstart);
		localStorage.setItem('MK_SETTINGS_SLEEPEND', configuration.sleepend);

    //Send to Pebble, persist there
    Pebble.sendAppMessage(
      {"MK_SETTINGS_CHAR": configuration.character, "MK_SETTINGS_TIMETYPE": configuration.fighttime, "MK_SETTINGS_TIMEEXACT": configuration.fightCustom, "MK_SETTINGS_RESETSETTINGS": configuration.resetstats, "MK_SETTINGS_FIGHTRUMBLEOFF": configuration.matchrumble, "MK_SETTINGS_USDATEFORMAT": configuration.dateformat, "MK_SETTINGS_SLEEPSTART": configuration.sleepstart, "MK_SETTINGS_SLEEPEND": configuration.sleepend},
      function(e) {
        //console.log("Sending settings data...");
      },
      function(e) {
        //console.log("Settings feedback failed!");
      }
    );
  }
);

//Listen for when settings is loaded
Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    //console.log("Trying to load config page");
		var character = localStorage.getItem('MK_SETTINGS_CHAR');
		var FightTime = localStorage.getItem('MK_SETTINGS_TIMETYPE');
		var TimeExact = localStorage.getItem('MK_SETTINGS_TIMEEXACT');
		var Rumble = localStorage.getItem('MK_SETTINGS_FIGHTRUMBLEOFF');
		var UsDate = localStorage.getItem('MK_SETTINGS_USDATEFORMAT');
		var SleepStart = localStorage.getItem('MK_SETTINGS_SLEEPSTART');
		var SleepEnd = localStorage.getItem('MK_SETTINGS_SLEEPEND');
		//v1
    //Pebble.openURL("https://www.googledrive.com/host/0B8my7qRrMtjpNWdLZTBfREd4SmM/?Ch="+character+"fi"+FightTime+"ti"+TimeExact+"ru"+Rumble+"us"+UsDate+"ss"+SleepStart+"se"+SleepEnd);
		//v2
    Pebble.openURL("https://www.googledrive.com/host/0B8my7qRrMtjpOEIyenhXVHFaYUU/?Ch="+character+"&fi="+FightTime+"&ti="+TimeExact+"&ru="+Rumble+"&us="+UsDate+"&ss="+SleepStart+"&se="+SleepEnd);
	}
);