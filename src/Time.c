#include <pebble.h>
#include "Time.h"
#include "main.h"
#include "Settings.h"

static TextLayer *TimeLayer;
static TextLayer *DayLayer;
static TextLayer *DateLayer;

//Backgrounds
static Layer *BackgroundLayer;
//static GBitmap *BackgroundBitmap;

static GFont MainFont28;
static GFont MainFont14;

void UpdateTime() {
  // Get a tm structure
  time_t Temp = time(NULL); 
  struct tm *TickTime = localtime(&Temp);

  // Create a long-lived buffer
  static char Buffer[] = "00:00";
  //static char BufferSeconds[] = "00";
  static char BufferDay[] = "Mon";
  static char BufferDate[] = "21/Aug";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(Buffer, sizeof("00:00"), "%H:%M", TickTime);
  } else {
    // Use 12 hour format
    strftime(Buffer, sizeof("00:00"), "%I:%M", TickTime);
  }
	
	//day and month
	if(UseAmericanDateFormat())
  	strftime(BufferDate, sizeof("21 Aug"), "%b %e", TickTime);
	else
  	strftime(BufferDate, sizeof("21 Aug"), "%e %b", TickTime);
	//Day of the week
  strftime(BufferDay, sizeof("Mon"), "%a", TickTime);

  // Display this time on the TextLayer
  text_layer_set_text(TimeLayer, Buffer);

  //Now get the day
  text_layer_set_text(DayLayer, BufferDay);

  //Now get the date
  text_layer_set_text(DateLayer, BufferDate);
}

static void DrawTimeBack(Layer *layer, GContext *ctx) 
{
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_context_set_fill_color(ctx, GColorFromRGB(0,0,0));
	graphics_fill_rect(ctx, GRect(0,0, 144, 50), 0, GCornerNone);
}

void SetTimeLayerSettings(Window *ParentWindow, TextLayer *lay, GFont *ft)
{
	text_layer_set_background_color(lay, GColorClear);
  text_layer_set_text_color(lay, GColorWhite);
  // Improve the layout to be more like a watchface
  text_layer_set_font(lay, ft);
  text_layer_set_text_alignment(lay, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(ParentWindow), text_layer_get_layer(lay));
}

void LoadTextLayer(Window *ParentWindow)
{
	// Create time TextLayer
	SetTimeLayerSettings(ParentWindow, TimeLayer, MainFont28);
  
  // Create day TextLayer
	SetTimeLayerSettings(ParentWindow, DayLayer, MainFont14);

  // Create date TextLayer
	SetTimeLayerSettings(ParentWindow, DateLayer, MainFont14);
}

void LoadBackgroundLayer(Window *ParentWindow)
{
  // Create GBitmap, then set to created BitmapLayer
 // bitmap_layer_set_bitmap(BackgroundLayer, BackgroundBitmap);
  layer_add_child(window_get_root_layer(ParentWindow), (BackgroundLayer)); 
}

inline void AllocateTime()
{
	MainFont28 = fonts_load_custom_font(resource_get_handle(/*RESOURCE_ID_FONT_TIME_28*/RESOURCE_ID_FONT_TIME_34));
	MainFont14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_14));

	TimeLayer = text_layer_create(GRect(32, 4, 80, 34));
  DayLayer = text_layer_create(GRect(0, 8, 38, 16));
  DateLayer = text_layer_create(GRect(0, 26, 38, 16));

 // BackgroundBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIMEBACKGROUND);
 // BackgroundLayer = bitmap_layer_create(GRect(0, 0, 144, 50));
	BackgroundLayer = layer_create(GRect(0, 0, 144, 50));
	layer_set_update_proc(BackgroundLayer, DrawTimeBack);
}

inline void LoadTime(Window *ParentWindow)
{
  LoadBackgroundLayer(ParentWindow);
  LoadTextLayer(ParentWindow);	
}

inline void UnloadTime()
{
	// Destroy TextLayer
  text_layer_destroy(TimeLayer);
  text_layer_destroy(DayLayer);
  text_layer_destroy(DateLayer);
  
	//Destroy fonts
	fonts_unload_custom_font(MainFont28);
	fonts_unload_custom_font(MainFont14);

	// Destroy GBitmaps
  // gbitmap_destroy(BackgroundBitmap);

  // Destroy BitmapLayers
  //bitmap_layer_destroy(BackgroundLayer);
	layer_destroy(BackgroundLayer);
}
