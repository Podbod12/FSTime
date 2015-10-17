#include <pebble.h>
#include "Battery.h"
#include "main.h"
#include "BitmapFlip.h"
	
static Layer *BatteryLayer;
static GBitmap *BatteryBitmap;

uint8_t BatteryBlocks = 0;

void DrawBattery(Layer *layer, GContext *ctx)
{
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	
	graphics_draw_bitmap_in_rect(ctx, BatteryBitmap, GRect(0, 0, 10, 10));
	
	graphics_context_set_fill_color(ctx, GColorFromRGB(255,255,255));
	for(uint8_t index = 0; index < BatteryBlocks && index < 4; ++index)
	{
		graphics_fill_rect(ctx, GRect(12 + (index * 6), 2, 4, 6), 0, GCornerNone);
	}
}

void UpdateBatteryBlocks()
{
	BatteryChargeState charge_state = battery_state_service_peek();
	BatteryBlocks = (charge_state.charge_percent / 20);
}

void UpdateBattery()
{
	UpdateBatteryBlocks();
	layer_mark_dirty((Layer *) BatteryLayer);
}

void AllocateBattery()
{
	GBitmap *TestB;
	BatteryBitmap = gbitmap_create_blank(GSize(10, 10), GBitmapFormat1BitPalette);
	TestB = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
	CopyBitmapData(TestB, BatteryBitmap, 0);
	gbitmap_destroy(TestB);
	//BatteryBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY);
	
  BatteryLayer = layer_create(GRect(108, 36, 38, 10));
	
	UpdateBattery();
}

void LoadBattery(Window *ParentWindow)
{
  layer_add_child(window_get_root_layer(ParentWindow), BatteryLayer);  
	  
	layer_set_update_proc(BatteryLayer, DrawBattery);
}

void UnloadBattery()
{
	 // Destroy GBitmaps
  gbitmap_destroy(BatteryBitmap);

  // Destroy BitmapLayers
  layer_destroy(BatteryLayer);
}

