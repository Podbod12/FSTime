#include <pebble.h>
#include "Plane.h"
#include "Settings.h"
#include "AutoAnimationEngine.h"

#include "main.h"

int FlagResourceIDs[8] = {
                      RESOURCE_ID_IMAGE_FLAGJAPAN,  
                      RESOURCE_ID_IMAGE_FLAGUSA,
                      RESOURCE_ID_IMAGE_FLAGJAPAN,
                      RESOURCE_ID_IMAGE_FLAGUSA,
                      RESOURCE_ID_IMAGE_FLAGBRAZIL,
                      RESOURCE_ID_IMAGE_FLAGRUSSIA,
                      RESOURCE_ID_IMAGE_FLAGINDIA,
                      RESOURCE_ID_IMAGE_FLAGCHINA,
                      }; //These should match the character array in character.h

static BitmapLayer *PlaneBackLayer;
static GBitmap *PlaneBackBitmap;

static Layer *PlaneLayer;
static GBitmap *PlaneBitmap;

static BitmapLayer *LeftFlagLayer;
static GBitmap *LeftFlagBitmap;
static BitmapLayer *RightFlagLayer;
static GBitmap *RightFlagBitmap;

static uint8_t CurrentFlag = 0;
static uint8_t NextFlag = 0;

int CurrentPlanePerc;
static uint8_t PlaneFlightWidth = 122;
static uint8_t PlaneFlightStart = 2;

bool bInitialisedPlane;

void UpdatePlane(struct tm *tick_time)
{
	if(bInitialisedPlane == false)
		return;
	
	CurrentPlanePerc = (int)(100.0 - GetTimeTillFightAsPerc(tick_time));
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Plane update=%d", (int)(CurrentPlanePerc));
	
  layer_mark_dirty(PlaneLayer);
}

void DrawPlane(Layer *layer, GContext *ctx)
{
	if(bInitialisedPlane == false)
		return;

	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, PlaneBitmap, GRect(PlaneFlightStart + ((PlaneFlightWidth * CurrentPlanePerc) / 100), 0, 18, 18));
}

inline int GetNextFlag()
{
	return FlagResourceIDs[GetOpponentCharacter()];	
}

inline int GetCurrentFlag()
{
	return FlagResourceIDs[GetCharacter()];	
}

void UpdateFlags()
{
 	if(bInitialisedPlane == false)
		return;

	gbitmap_destroy(LeftFlagBitmap);
  gbitmap_destroy(RightFlagBitmap);
	
	CurrentFlag = GetCurrentFlag();
	NextFlag = GetNextFlag();
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Flag update=%d,%d", CurrentFlag, NextFlag);
	
  LeftFlagBitmap = gbitmap_create_with_resource(CurrentFlag);
  bitmap_layer_set_bitmap(LeftFlagLayer, LeftFlagBitmap);
	layer_mark_dirty((Layer *) LeftFlagLayer);
	
  RightFlagBitmap = gbitmap_create_with_resource(NextFlag);
  bitmap_layer_set_bitmap(RightFlagLayer, RightFlagBitmap);
	layer_mark_dirty((Layer *) RightFlagLayer);
}

void RestartPlane()
{
	if(bInitialisedPlane == false)
		return;

	UpdateFlags();
  //CurrentPlanePerc = 0;
	layer_mark_dirty(PlaneLayer);
}

void LoadPlane(Window *ParentWindow, bool bFirstInit)
{
	if(bFirstInit == false && bInitialisedPlane)
		return;
	
	bInitialisedPlane = true;
	
	CurrentFlag = GetCurrentFlag();
	NextFlag = GetNextFlag();

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Flag load=%d,%d - %d %d - %d %d - %d %d", CurrentFlag, NextFlag, GetCharacter(), GetOpponentCharacter(), FlagResourceIDs[1], FlagResourceIDs[4], RESOURCE_ID_IMAGE_FLAGUSA, RESOURCE_ID_IMAGE_FLAGBRAZIL);

	Layer *AnimLayer = GetAnimLayer();
	
  // Create GBitmap, then set to created BitmapLayer
  PlaneBackBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLANEBACK);
  PlaneBackLayer = bitmap_layer_create(GRect(0, 150, 144, 18));
  bitmap_layer_set_bitmap(PlaneBackLayer, PlaneBackBitmap);
  layer_add_child((AnimLayer), bitmap_layer_get_layer(PlaneBackLayer));  

  LeftFlagBitmap = gbitmap_create_with_resource(CurrentFlag);
  LeftFlagLayer = bitmap_layer_create(GRect(4, 154, 15, 11));
  bitmap_layer_set_bitmap(LeftFlagLayer, LeftFlagBitmap);
  layer_add_child((AnimLayer), bitmap_layer_get_layer(LeftFlagLayer));  

  RightFlagBitmap = gbitmap_create_with_resource(NextFlag);
  RightFlagLayer = bitmap_layer_create(GRect(124, 154, 15, 11));
  bitmap_layer_set_bitmap(RightFlagLayer, RightFlagBitmap);
  layer_add_child((AnimLayer), bitmap_layer_get_layer(RightFlagLayer));  
	
	PlaneBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLANE);
  PlaneLayer = layer_create(GRect(0, 152, 144, 18));
  layer_add_child((AnimLayer), PlaneLayer);  
  layer_set_update_proc(PlaneLayer, DrawPlane);
}

void UnloadPlane()
{
	if(bInitialisedPlane == false)
		return;
	
	bInitialisedPlane = false;
	
	 // Destroy GBitmaps
  gbitmap_destroy(PlaneBackBitmap);
  gbitmap_destroy(PlaneBitmap);
  gbitmap_destroy(LeftFlagBitmap);
  gbitmap_destroy(RightFlagBitmap);

  // Destroy BitmapLayers
  bitmap_layer_destroy(PlaneBackLayer);
  layer_destroy(PlaneLayer);
  bitmap_layer_destroy(LeftFlagLayer);
  bitmap_layer_destroy(RightFlagLayer);
}