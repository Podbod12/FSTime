#include <pebble.h>
#include "main.h"
#include "WipeEffect.h"
#include "BitmapFlip.h"

//////////////////////////////////////
//Backgrounds
//////////////////////////////////////

static Layer *WipeSquareLayer;
static GBitmap *WipeSquareBitmap;

//////////////////////////////////////
//Constants
//////////////////////////////////////

#define NumFramesForFullMove 15
#define NumFramesBetweenSquares 5

#define SquareImageSize 61
#define SquareSize 50
#define SquareOffset (SquareImageSize / 2)

#define TravelDistance 100
  
#define WipeWidth 144
#define WipeHeight 152
  
#define WipeWidthStart 0
#define WipeHeightStart 26
	
#ifdef PHONEBUILD
	#define HoldTime 15
	#define SquareMovePerFrame 32
#else
	#define HoldTime 30
	#define SquareMovePerFrame 16
#endif	
	
#define NumYSquares /*((WipeHeight / SquareSize) + 1)*/ 4
#define NumXSquares /*((WipeWidth / (SquareSize / 2)) + 2)*/ 6 //Tiled means twice as much
#define NumSquaresNeeded /*(NumYSquares * NumXSquares)*/ 31

//////////////////////////////////////
//Vars
//////////////////////////////////////

enum WipeState
{
  WS_NotRunning,
  WS_WaitingToStart,
  WS_In,
  WS_Hold,
  WS_Out,
};

enum WipeState CurrentWipeState;
int StateTimer;

void (*MiddleCallback)();
void (*EndCallback)();

//////////////////////////////////////
//Funcs
//////////////////////////////////////

void StartWipe()
{
  int index;
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "start wipe");
 
  //Set start positions
  for(index = 0; index < NumSquares; ++index)
  {
    int XOffset, YOffset;
    
    //Determine row and coloum
    XOffset = (int)index / NumYSquares;
    YOffset = index % NumYSquares;
    
    //Set end positions
    Squares[index].EndXPos = 0 + (XOffset * (SquareSize / 2));
    Squares[index].EndYPos = 0 + (YOffset * SquareSize);
    if(XOffset % 2 == 1)
    {
      Squares[index].EndYPos += (SquareSize / 2);
    }      
    
    //Set Start Positons
    int CenterOffsetIndex = abs(72 - Squares[index].EndXPos) / (SquareSize / 2);
		int FinalOffset = (100 + (80 * CenterOffsetIndex) + (rand() % 70));
    if(Squares[index].EndXPos <= (WipeWidth/2))
      Squares[index].StartXPos = Squares[index].EndXPos - FinalOffset; 
    else
      Squares[index].StartXPos = Squares[index].EndXPos + FinalOffset; 
    Squares[index].CurrentXPos = Squares[index].StartXPos;
    Squares[index].CurrentYPos = Squares[index].EndYPos;
                                                         
    //Set if used in wipe
		Squares[index].bBeingUsed = index < NumSquaresNeeded;
    
    //Reset other vars
  }
                                                         
  CurrentWipeState = WS_In;
  StateTimer = 0;
}
                                                         
void WipeIn()
{
  int index;
  bool bFinished = true;
  
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Wipe In");

	for(index = 0; index < NumSquares; ++index)
  {
    if(Squares[index].bBeingUsed)
    {
      if(Squares[index].CurrentXPos != Squares[index].EndXPos)
      {
        if(Squares[index].EndXPos > Squares[index].CurrentXPos)
        {
          Squares[index].CurrentXPos += SquareMovePerFrame;
          if(Squares[index].EndXPos < Squares[index].CurrentXPos)
            Squares[index].CurrentXPos = Squares[index].EndXPos;
        }
        else
        {
          Squares[index].CurrentXPos -= SquareMovePerFrame;
          if(Squares[index].EndXPos > Squares[index].CurrentXPos)
            Squares[index].CurrentXPos = Squares[index].EndXPos;
        }
        
        bFinished = false;
      }
    }
  } 
  
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "WipeIn %d, %d", Squares[0].CurrentXPos, Squares[0].StartXPos);

	if(bFinished)
  {
    StateTimer = 0;
    CurrentWipeState = WS_Hold;
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "WipeIn Middle");
		MiddleCallback();
  }
}                                                         
                                                         
void WipeHold()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "WipeHold");

	StateTimer++;
  
  if(StateTimer > HoldTime)
  {
    StateTimer = 0;
    CurrentWipeState = WS_Out;
  }
}
                                                         
void WipeOut()                                                         
{
  int index;
  bool bFinished = true;
  
	for(index = 0; index < NumSquares; ++index)
	{
    if(Squares[index].bBeingUsed)
    {
      if(Squares[index].CurrentXPos > (0 - (SquareSize/2.1)) && Squares[index].CurrentXPos < (144 + (SquareSize/2.1)))
      {
        if(Squares[index].StartXPos > Squares[index].CurrentXPos)
        {
          Squares[index].CurrentXPos += (SquareMovePerFrame/2);
          if(Squares[index].StartXPos < Squares[index].CurrentXPos)
            Squares[index].CurrentXPos = Squares[index].StartXPos;
        }
        else
        {
          Squares[index].CurrentXPos -= (SquareMovePerFrame/2);
          if(Squares[index].StartXPos > Squares[index].CurrentXPos)
            Squares[index].CurrentXPos = Squares[index].StartXPos;
        }
        
        bFinished = false;
      }
    }
  } 
  
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "WipeOut %d, %d", Squares[0].CurrentXPos, Squares[0].StartXPos);

  if(bFinished)
  {
    StateTimer = 0;
    CurrentWipeState = WS_NotRunning;
		EndCallback();
  } 
}
                                                         
void UpdateWipe()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "update wipe");
  switch(CurrentWipeState)
  {
    case WS_WaitingToStart:
    {
      StartWipe();
      break;
    }
    
    case WS_In:
    {
      WipeIn();
      break;
    }
    
    case WS_Hold:
    {
      WipeHold();
      break;
    }

    case WS_Out:
    {
      WipeOut();
      break;
    }
  
    case WS_NotRunning:
    default:
    {
      //Testing - repeat loop
      //CurrentWipeState = WS_WaitingToStart;
      //app_timer_register(80, UpdateWipe, NULL);
      //layer_mark_dirty(WipeSquareLayer);
      
      //Stop running
      return;
    }
  }
  
  //Keep running
  app_timer_register(80, UpdateWipe, NULL);
  layer_mark_dirty(WipeSquareLayer);
}

static void DrawSquareWipe(Layer *layer, GContext *ctx) 
{
  int index;
  
  if(CurrentWipeState == WS_NotRunning)
    return;
	    
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  
  for(index = 0; index < NumSquares; ++index)
  {
    if(Squares[index].bBeingUsed)
    {
      graphics_draw_bitmap_in_rect(ctx, WipeSquareBitmap, GRect(Squares[index].CurrentXPos - SquareOffset, Squares[index].CurrentYPos - SquareOffset, SquareImageSize, SquareImageSize));
      
      //Test End pos
      //graphics_draw_bitmap_in_rect(ctx, WipeSquareBitmap, GRect(Squares[index].EndXPos - SquareOffset, Squares[index].EndYPos - SquareOffset, SquareImageSize, SquareImageSize));

      //Test start pos
      //graphics_draw_bitmap_in_rect(ctx, WipeSquareBitmap, GRect(Squares[index].StartXPos - SquareOffset, Squares[index].StartYPos - SquareOffset, SquareImageSize, SquareImageSize));
    }
  }
}

static void CreateWipeSquareLayer(Window *ParentWindow)
{
  // Create GBitmap, then set to created BitmapLayer
  //WipeSquareBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WIPESQUAREBIG);
  //WipeSquareLayer = layer_create(GRect(WipeWidthStart, WipeHeightStart, WipeWidth, WipeHeight));
  
  layer_add_child(window_get_root_layer(ParentWindow), WipeSquareLayer);  
  
  layer_set_update_proc(WipeSquareLayer, DrawSquareWipe);
}

void AllocateWipe()
{
	WipeSquareBitmap = gbitmap_create_blank(GSize(61, 61), GBitmapFormat2BitPalette);
	PackBitmap(RESOURCE_ID_IMAGE_WIPESQUAREBIG2, WipeSquareBitmap);
  //WipeSquareBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WIPESQUAREBIG);
  WipeSquareLayer = layer_create(GRect(WipeWidthStart, WipeHeightStart, WipeWidth, WipeHeight));
}

void LoadScreenWipe(Window *ParentWindow)
{
  CreateWipeSquareLayer(ParentWindow);
  
  CurrentWipeState = WS_NotRunning;
  layer_set_hidden(WipeSquareLayer, true);
}

void ClearWipe()
{
	CurrentWipeState = WS_NotRunning;
}

void PerformWipe(void (*WipeMiddleCallback), void (*WipeOverCallback))
{
	layer_set_hidden(WipeSquareLayer, false);
	CurrentWipeState = WS_WaitingToStart;
  UpdateWipe();
	MiddleCallback = WipeMiddleCallback;
	EndCallback = WipeOverCallback;
}

void UnloadScreenWipe()
{
  // Destroy GBitmap
  gbitmap_destroy(WipeSquareBitmap);

  // Destroy BitmapLayer
  layer_destroy(WipeSquareLayer);
}