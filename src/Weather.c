#include <pebble.h>
#include "Weather.h"
#include "main.h"
#include "BitmapFlip.h"
	
enum WeatherTypes
{
  WT_Sunny,
  WT_FewClouds,
  WT_Cloudy,
	WT_BrokenClouds,
	WT_LightRain,
  WT_Rain,
  WT_Thunder,
  WT_Snow,
	WT_Mist,
  WT_Night,
  WT_NightFewClouds,
  WT_NightRain,
	
  WT_Unknown
};

enum WeatherTypes CurrentWeather = WT_Unknown;

int WeatherResourceIDs[12] = {
                      RESOURCE_ID_IMAGE_WEATHER_SUNNY,
                      RESOURCE_ID_IMAGE_WEATHER_FEWCLOUDS,
                      RESOURCE_ID_IMAGE_WEATHER_CLOUDY,
                      RESOURCE_ID_IMAGE_WEATHER_BROKENCLOUDS,
                      RESOURCE_ID_IMAGE_WEATHER_SHOWERRAIN,
                      RESOURCE_ID_IMAGE_WEATHER_RAIN,
                      RESOURCE_ID_IMAGE_WEATHER_THUNDERSTORM,
                      RESOURCE_ID_IMAGE_WEATHER_SNOW,
                      RESOURCE_ID_IMAGE_WEATHER_MIST,
	                    RESOURCE_ID_IMAGE_WEATHER_NIGHT,
                    	RESOURCE_ID_IMAGE_WEATHER_NIGHTFEWCLOUDS,
                    	RESOURCE_ID_IMAGE_WEATHER_NIGHTRAIN,
                      };

static BitmapLayer *WeatherLayer;
static GBitmap *WeatherBitmap;

#define WEATHERSTARTY 6

inline int GetCurrentWeatherResourceID()
{
  return WeatherResourceIDs[CurrentWeather];
}

bool NoWeatherIconSet()
{
	return CurrentWeather == WT_Unknown;
}

void SetWeather(char *WeatherConditions)
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather Set %s %i", WeatherConditions, strcmp(WeatherConditions, "03n"));
	
	//Find most appropriate image
	if(WeatherConditions[0] == '0')
	{
		if(WeatherConditions[1] == '1')
		{
			if(WeatherConditions[2] == 'd')
			{
				CurrentWeather = WT_Sunny;
			}
			else
			{
				CurrentWeather = WT_Night;			
			}
		}
		else if(WeatherConditions[1] == '2')
		{
			if(WeatherConditions[2] == 'd')
			{
				CurrentWeather = WT_FewClouds;
			}
			else
			{
				CurrentWeather = WT_NightFewClouds;			
			}
		}
		else if(WeatherConditions[1] == '3')
		{
			CurrentWeather = WT_Cloudy;
		}
		else if(WeatherConditions[1] == '4')
		{
			CurrentWeather = WT_BrokenClouds;
		}
		else if(WeatherConditions[1] == '9')
		{
			CurrentWeather = WT_LightRain;
		}
	}
	else if(WeatherConditions[0] == '1')
	{
		if(WeatherConditions[1] == '0')
		{
			if(WeatherConditions[2] == 'd')
			{
				CurrentWeather = WT_Rain;
			}
			else
			{
				CurrentWeather = WT_NightRain;			
			}
		}
		else if(WeatherConditions[1] == '1')
		{
			CurrentWeather = WT_Thunder;
		}
		else if(WeatherConditions[1] == '3')
		{
			CurrentWeather = WT_Snow;
		}
	}
	else if(WeatherConditions[0] == '5')
	{
		CurrentWeather = WT_Mist;
	}
	/*if(strcmp(WeatherConditions, "01d") == 0)
		CurrentWeather = WT_Sunny;
	else if(strcmp(WeatherConditions, "01n") == 0)
		CurrentWeather = WT_Night;
	else if(strcmp(WeatherConditions, "02d") == 0)
		CurrentWeather = WT_FewClouds;
	else if(strcmp(WeatherConditions, "02n") == 0)
		CurrentWeather = WT_NightFewClouds;
	else if(strcmp(WeatherConditions, "03d") == 0 || strcmp(WeatherConditions, "03n") == 0)
		CurrentWeather = WT_Cloudy;
	else if(strcmp(WeatherConditions, "04d") == 0 || strcmp(WeatherConditions, "04n") == 0)
		CurrentWeather = WT_BrokenClouds;
	else if(strcmp(WeatherConditions, "09d") == 0 || strcmp(WeatherConditions, "09n") == 0)
		CurrentWeather = WT_LightRain;
	else if(strcmp(WeatherConditions, "10d") == 0)
		CurrentWeather = WT_Rain;
	else if(strcmp(WeatherConditions, "10n") == 0)
		CurrentWeather = WT_NightRain;
	else if(strcmp(WeatherConditions, "11d") == 0 || strcmp(WeatherConditions, "11n") == 0)
		CurrentWeather = WT_Thunder;
	else if(strcmp(WeatherConditions, "13d") == 0 || strcmp(WeatherConditions, "13n") == 0)
		CurrentWeather = WT_Snow;
	else if(strcmp(WeatherConditions, "50d") == 0 || strcmp(WeatherConditions, "50n") == 0)
		CurrentWeather = WT_Mist;*/
	
	//Tell main to update the gfx
	if(CurrentWeather != WT_Unknown)
		UpdateWeatherImage();
}

void UpdateWeather()
{
	// Begin dictionary
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	// Add a key-value pair
	dict_write_uint8(iter, 0, 0);

	// Send the message!
	app_message_outbox_send();
}

void UpdateWeatherImage()
{
	GBitmap *TestB;

	//Stick weather image where I want it. Not stupid SDK place
	TestB = gbitmap_create_with_resource(GetCurrentWeatherResourceID());
	CopyBitmapData(TestB, WeatherBitmap, 0);
	gbitmap_destroy(TestB);
	
	//APP_LOG(APP_LOG_LEVEL_INFO, "Memory weather 1 %d %d", heap_bytes_free(), heap_bytes_used());

	layer_mark_dirty((Layer *) WeatherLayer);
}

void AllocateWeather()
{
	WeatherBitmap = gbitmap_create_blank(GSize(25, 25), GBitmapFormat4BitPalette);
	
  //GBitmapFormat format = gbitmap_get_format(WeatherBitmap);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather image %d", format);
	
  WeatherLayer = bitmap_layer_create(GRect(116, WEATHERSTARTY, 25, 25));
	
  bitmap_layer_set_bitmap(WeatherLayer, WeatherBitmap);
}

inline void LoadWeather(Window *ParentWindow)
{
  // Create GBitmap, then set to created BitmapLayer
  //WeatherBitmap = gbitmap_create_with_resource(GetCurrentWeatherResourceID());
  //WeatherLayer = bitmap_layer_create(GRect(116, 8, 25, 25));
  bitmap_layer_set_bitmap(WeatherLayer, WeatherBitmap);
  layer_add_child(window_get_root_layer(ParentWindow), bitmap_layer_get_layer(WeatherLayer));  
}

inline void UnloadWeather()
{
	 // Destroy GBitmaps
  gbitmap_destroy(WeatherBitmap);

  // Destroy BitmapLayers
  bitmap_layer_destroy(WeatherLayer);
}

