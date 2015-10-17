#include <pebble.h>
#include "Message.h"
#include "Weather.h"
#include "Settings.h"
#include "main.h"
#include "Plane.h"
#include "Characters.h"

static char CharNames[C_MAX][8] = {
  "Ryu",
  "Ken",
  "Honda",
  "Guile",
  "Blanka",
  "Zangief",
  "Dhalsim",
  "ChunLi",
};

static char TimeNames[FT_MAX][7] = {
  "Hourly",
//  "Eight",
  "Custom",
};

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
	bool bSettigsChanged = false;
	
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case MK_WEATHER:
		{
			char conditions_buffer[32];
			snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      //APP_LOG(APP_LOG_LEVEL_ERROR, "Weather key recieved - %s", conditions_buffer);
			SetWeather(conditions_buffer);
		}  break;
			
		case MK_SETTINGS_RESETSETTINGS:
		{
			if(strcmp(t->value->cstring, "true") == 0)
      {
				persist_write_bool(MK_SETTINGS_RESETSETTINGS, true);
				bSettigsChanged = true;
			}
		}  break;
	
		case MK_SETTINGS_CHAR:
		{
			int CurrentChar = persist_read_int(MK_SETTINGS_CHAR);
			for(int index = 0; index < C_MAX; ++index)
			{
				if(strcmp(t->value->cstring, CharNames[index]) == 0 && CurrentChar != index)
				{
					bSettigsChanged = true;
					SetCharacter(index);
					persist_write_int(MK_SETTINGS_CHAR, index);
				}
			}
		}  break;
			
		case MK_SETTINGS_TIMETYPE:
		{
			int CurrentTime = persist_read_int(MK_SETTINGS_TIMETYPE);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "TimeSettings=%s, %d, %d, %s", t->value->cstring, CurrentTime, strcmp(t->value->cstring, TimeNames[1]), TimeNames[1]);
			for(int index = 0; index < FT_MAX; ++index)
			{
				if(strcmp(t->value->cstring, TimeNames[index]) == 0 && CurrentTime != index)
				{
					bSettigsChanged = true;
       		SetFightTime(index);
        	persist_write_int(MK_SETTINGS_TIMETYPE, index);
					persist_write_int(MK_SETTINGS_ROUNDWINS, 0);
					persist_write_int(MK_SETTINGS_ROUNDLOSES, 0);
				}
			}
		}  break;
			
		case MK_SETTINGS_TIMEEXACT:
		{
      persist_write_int(MK_SETTINGS_TIMEEXACT, atoi(t->value->cstring));
		}  break;
		
		case MK_SETTINGS_USDATEFORMAT:
		{
			bool bNewVal = false;
 			if(strcmp(t->value->cstring, "true") == 0)
				bNewVal = true;
			SetUSDateFormat(bNewVal);
		}  break;
			
		case MK_SETTINGS_FIGHTRUMBLEOFF:
		{
			bool bNewVal = false;
 			if(strcmp(t->value->cstring, "true") == 0)
				bNewVal = true;
			SetAlertFight(!bNewVal);
		}  break;
	
		case MK_SETTINGS_SLEEPSTART:
		{
			SetSleepStart(atoi(t->value->cstring));
		}  break;
			
		case MK_SETTINGS_SLEEPEND:
		{
			SetSleepEnd(atoi(t->value->cstring));
		}  break;
			
		default:
      //APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
	
	//Reload
	if(bSettigsChanged)
	{
		RestartAfterSettingsChange();
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! %d", reason);
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) 
{
  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void RegisterMessageCallbacks()
{
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}