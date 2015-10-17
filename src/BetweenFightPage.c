#include <pebble.h>
#include "BetweenFightPage.h"
#include "AutoAnimationEngine.h"
#include "Characters.h"
#include "Settings.h"
#include "Time.h"
#include "main.h"

static BitmapLayer *BackgroundLayer;
//static Layer *BetweenFightLayer;
static GBitmap *BackgroundBitmap;

static GFont MainFont12;

bool bInitialisedBetweenFight;

bool IsInStatsPage()
{
	return bInitialisedBetweenFight;
}

uint8_t Faces[C_MAX] = 
{
	RESOURCE_ID_IMAGE_RYUFACE,
	RESOURCE_ID_IMAGE_KENFACE,
	RESOURCE_ID_IMAGE_HONDAFACE,
	RESOURCE_ID_IMAGE_GUILEFACE,
	RESOURCE_ID_IMAGE_BLANKAFACE,
	RESOURCE_ID_IMAGE_ZANGIEFFACE,
	RESOURCE_ID_IMAGE_DHALSIMFACE,
	RESOURCE_ID_IMAGE_CHUNFACE
};

uint8_t FacesGrey[C_MAX] = 
{
	RESOURCE_ID_IMAGE_RYUGREYFACE,
	RESOURCE_ID_IMAGE_KENGREYFACE,
	RESOURCE_ID_IMAGE_HONDAGREYFACE,
	RESOURCE_ID_IMAGE_GUILEGREYFACE,
	RESOURCE_ID_IMAGE_BLANKAGREYFACE,
	RESOURCE_ID_IMAGE_ZANGIEFGREYFACE,
	RESOURCE_ID_IMAGE_DHALSIMGREYFACE,
	RESOURCE_ID_IMAGE_CHUNGREYFACE
};

#define QUESTIONFACE RESOURCE_ID_IMAGE_QUESTIONFACE
#define FACESTARTY 3
#define FACESTARTX 4
#define FACEGAP 2
	
#define FACESIZEY 22
#define FACESIZEX 19

#define TEXTSTARTX 52

static Layer *FacesLayer;
static GBitmap *FacesBitmaps[C_MAX];
static GBitmap *FacesGreyBitmaps[C_MAX];
static GBitmap *QuestionFaceBitmap;

static TextLayer *TourneysWonLayer;
static TextLayer *TourneysWonDropLayer;
static TextLayer *CurrentTourneyLayer;
static TextLayer *CurrentTourneyDropLayer;
static TextLayer *WinLossLayer;
static TextLayer *WinLossDropLayer;
static TextLayer *StepsLayer;
static TextLayer *StepsDropLayer;

static Layer *StepBarsLayer;

void UpdateStats()
{
	CommitStepHistory();	
	
	if(bInitialisedBetweenFight == false)
		return;
}

static void DrawStepsBars(Layer *layer, GContext *ctx) 
{	
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
	
	int MaxStepHis = GetMaxStepHistory();
	float BestSteps = 1;
	for(int index = MaxStepHis-1; index >= 0; --index)
	{
		int thisHis = GetStepHistory(index);
		
		if(thisHis > BestSteps)
			BestSteps = thisHis;
	}
	
	graphics_context_set_fill_color(ctx, GColorFromRGB(255,255,0));
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "StepUpdate3");
	for(int index = 0; index < MaxStepHis; ++index)
	{
		float thisHis = (float)GetStepHistory(index);
		int zSize = (thisHis / BestSteps) * 18;
		if(zSize <= 0)
			zSize = 1;
		graphics_fill_rect(ctx, GRect(index * 3, 18 - zSize, 2, zSize), 0, GCornerNone);		
	}
}

GBitmap *GetNextFaceBitmap(int idx)
{
	int opp, nextOpp;

	opp = GetFightHistory(idx);
	nextOpp = GetFightHistory(idx + 1);

	if(opp != C_MAX)
	{
		if(nextOpp != C_MAX)
			return FacesGreyBitmaps[opp];
		else
			return FacesBitmaps[opp];
	}
	
	return QuestionFaceBitmap;
}

static void DrawFaces(Layer *layer, GContext *ctx) 
{
	GRect BitSize = gbitmap_get_bounds(QuestionFaceBitmap);
	
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
	
	for(int FaceIdx = 0; FaceIdx < (C_MAX / 2); ++FaceIdx)
	{
		GBitmap *faceToUse;
		
		//left
		faceToUse = GetNextFaceBitmap(FaceIdx * 2);
		graphics_draw_bitmap_in_rect(ctx, faceToUse, GRect(0, (BitSize.size.h + FACEGAP) * FaceIdx, BitSize.size.w, BitSize.size.h));			

		//right
		faceToUse = GetNextFaceBitmap((FaceIdx * 2) + 1);
		graphics_draw_bitmap_in_rect(ctx, faceToUse, GRect(BitSize.size.w + FACEGAP, (BitSize.size.h + FACEGAP) * FaceIdx, BitSize.size.w, BitSize.size.h));			
	}
}

void LoadBackground(Window *ParentWindow)
{
	Layer *AnimLayer = GetAnimLayer();

	// Create GBitmap, then set to created BitmapLayer
  BackgroundBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STATSBACKGROUND);
  BackgroundLayer = bitmap_layer_create(GRect(0, 50, 144, 100));
  bitmap_layer_set_bitmap(BackgroundLayer, BackgroundBitmap);
  layer_add_child(/*window_get_root_layer(ParentWindow)*/AnimLayer, bitmap_layer_get_layer(BackgroundLayer));  
}

void LoadFaces(Window *ParentWindow)
{
	QuestionFaceBitmap = gbitmap_create_with_resource(QUESTIONFACE);
	
	for(int FaceIdx = 0; FaceIdx < C_MAX; ++FaceIdx)
	{
  	FacesBitmaps[FaceIdx] = gbitmap_create_with_resource(Faces[FaceIdx]);
  	FacesGreyBitmaps[FaceIdx] = gbitmap_create_with_resource(FacesGrey[FaceIdx]);
	}
	
	// Create GBitmap, then set to created BitmapLayer
  FacesLayer = layer_create(GRect(FACESTARTX, FACESTARTY, (FACESIZEX * 2) + FACEGAP, (FACESIZEY * 4) + (FACEGAP * 3)));

	//Parent to background
  bitmap_layer_set_bitmap(BackgroundLayer, BackgroundBitmap);
  layer_add_child(bitmap_layer_get_layer(BackgroundLayer), FacesLayer);  
	
	//Set update func
	layer_set_update_proc(FacesLayer, DrawFaces);
}

void InitTextStyle(TextLayer *TextL, GColor8 col)
{
  text_layer_set_background_color(TextL, GColorClear);
  text_layer_set_text_color(TextL, col);
  text_layer_set_font(TextL, MainFont12);
  text_layer_set_text_alignment(TextL, GTextAlignmentLeft);
  layer_add_child(bitmap_layer_get_layer(BackgroundLayer), text_layer_get_layer(TextL));
}

void DoTextBack(TextLayer **Lay, int x, int y, int w, int h, GColor8 col)
{
	*Lay = text_layer_create(GRect(x, y, w, h));
	InitTextStyle(*Lay, col);
	//text_layer_set_text(TourneysWonDropLayer, Buffer);
}

void LoadStatsText(Window *ParentWindow)
{
 	static char Buffer[22];
	static char Buffer2[20];
	static char Buffer3[21];

	MainFont12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_12));

	// Create Best Tourney TextLayer (dropped)
	snprintf(Buffer, sizeof(Buffer), "BEST LEVEL WON: %d", GetTourneysWon());
	DoTextBack(&TourneysWonDropLayer, TEXTSTARTX-2, FACESTARTY+2, (144 - TEXTSTARTX) - 1, 14, GColorBlack);
	text_layer_set_text(TourneysWonDropLayer, Buffer);
	// Create Best Tourney TextLayer
	DoTextBack(&TourneysWonLayer, TEXTSTARTX, FACESTARTY, (144 - TEXTSTARTX), 14, GColorWhite);
	text_layer_set_text(TourneysWonLayer, Buffer);
	
	// Create Current toruney TextLayer (dropped)
	snprintf(Buffer2, sizeof(Buffer2), "CURRENT LEVEL: %d", GetCurrentTournament() + 1);
	DoTextBack(&CurrentTourneyLayer, TEXTSTARTX-2, FACESTARTY+2 + 16, (144 - TEXTSTARTX) - 1, 14, GColorBlack);
	text_layer_set_text(CurrentTourneyLayer, Buffer2);
	// Create Current toruney TextLayer
	DoTextBack(&CurrentTourneyDropLayer, TEXTSTARTX, FACESTARTY + 16, (144 - TEXTSTARTX), 14, GColorWhite);
	text_layer_set_text(CurrentTourneyDropLayer, Buffer2);
	
	// Create Win loss TextLayer (dropped)
	int16_t wins, loses;
	GetWinLoss(&wins, &loses);
	snprintf(Buffer3, sizeof(Buffer3), "WIN/LOSS: %d/%d", wins, loses);
	DoTextBack(&WinLossDropLayer, TEXTSTARTX-2, FACESTARTY+2 + 32, (144 - TEXTSTARTX) - 1, 14, GColorBlack);
	text_layer_set_text(WinLossDropLayer, Buffer3);
	// Create Win loss TextLayer
	DoTextBack(&WinLossLayer, TEXTSTARTX, FACESTARTY + 32, (144 - TEXTSTARTX), 14, GColorWhite);
	text_layer_set_text(WinLossLayer, Buffer3);
	
	// Create steps (dropped)
	DoTextBack(&StepsLayer, TEXTSTARTX-2, FACESTARTY+2 + 48, (144 - TEXTSTARTX) - 1, 14, GColorBlack);
	text_layer_set_text(StepsLayer, "STEPS");
	// Create steps TextLayer
	DoTextBack(&StepsDropLayer, TEXTSTARTX, FACESTARTY + 48, (144 - TEXTSTARTX), 14, GColorWhite);
	text_layer_set_text(StepsDropLayer, "STEPS");
}

void LoadBetweenFight(Window *ParentWindow, bool bFirstInit)
{
	if(bFirstInit == false && bInitialisedBetweenFight)
		return;
	
	bInitialisedBetweenFight = true;

	LoadBackground(ParentWindow);
	LoadFaces(ParentWindow);
	LoadStatsText(ParentWindow);
	
	StepBarsLayer = layer_create(GRect(TEXTSTARTX, 128, 91, 24));
	Layer *AnimLayer = GetAnimLayer();
 	layer_add_child(AnimLayer/*window_get_root_layer(ParentWindow)*/, StepBarsLayer);  		
	layer_set_update_proc(StepBarsLayer, DrawStepsBars);
	
	//testing
	app_timer_register(1000, UpdateStats, NULL); //1fps
}

inline void UnloadBetweenFight()
{
 	if(bInitialisedBetweenFight == false)
		return;
	
	bInitialisedBetweenFight = false;
	
	// Destroy GBitmaps
  gbitmap_destroy(BackgroundBitmap);

  // Destroy Layers
  bitmap_layer_destroy(BackgroundLayer);
  layer_destroy(StepBarsLayer);
	
	//Destroy Text
	text_layer_destroy(TourneysWonLayer);
	text_layer_destroy(TourneysWonDropLayer);
	text_layer_destroy(CurrentTourneyLayer);
	text_layer_destroy(CurrentTourneyDropLayer);
	text_layer_destroy(WinLossLayer);
	text_layer_destroy(WinLossDropLayer);
	text_layer_destroy(StepsLayer);
	text_layer_destroy(StepsDropLayer);

	//Destroy Faces
	for(int FaceIdx = 0; FaceIdx < C_MAX; ++FaceIdx)
	{
  	gbitmap_destroy(FacesBitmaps[FaceIdx]);
  	gbitmap_destroy(FacesGreyBitmaps[FaceIdx]);
	}
  gbitmap_destroy(QuestionFaceBitmap);
  layer_destroy(FacesLayer);
	
	//Destroy fonts
	fonts_unload_custom_font(MainFont12);
}