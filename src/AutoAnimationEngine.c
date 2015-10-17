#include <pebble.h>
#include "AutoAnimationEngine.h"
#include "BitmapFlip.h"

//////////////////////////////////////
//Constants
//////////////////////////////////////

#define REBUILDSEQEACHFRAME 1

//////////////////////////////////////
//Vars
//////////////////////////////////////
  
struct AnimationSequence RunningAnimations[MaxAnimations]; 

static GBitmapSequence *AnimationBitmapSequences[MaxAnimations];
static GBitmap *AnimationBitmaps[MaxAnimations];

//void *SavedSpace[MaxAnimations];

bool bAnimationsInUse[MaxAnimations];
int NumRunningAnimations;

Layer *Main_Layer;

//////////////////////////////////////
//Funcs
//////////////////////////////////////

Layer *GetAnimLayer()
{
	return Main_Layer;
}

//////////////////////////////////////
//Draw stuff
static void DrawAnimations(Layer *layer, GContext *ctx) 
{
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing anims %d", NumRunningAnimations);

  if(NumRunningAnimations > 0)
  {
    int index;
    
    graphics_context_set_compositing_mode(ctx, GCompOpSet);

    for(index = 0; index < MaxAnimations; ++index)
    {
      if(bAnimationsInUse[index] == true)
      {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing anim %d", index);
				struct AnimationFrameData *thisFrameData = &(RunningAnimations[index].FrameData[RunningAnimations[index].CurrentFrame]);
        
				//Test new mem
				if(REBUILDSEQEACHFRAME)
				{
					AnimationBitmapSequences[index] = gbitmap_sequence_create_with_resource(RunningAnimations[index].ResourceID);
					/*if(AnimationBitmapSequences[index] )
						APP_LOG(APP_LOG_LEVEL_DEBUG, "AnimLoaded");
					else
						APP_LOG(APP_LOG_LEVEL_DEBUG, "AnimFAIL to load");*/
					RunningAnimations[index].LastAnimationFrame = 0;
				}
				
        //Advance bitmap to correct frame
				if(RunningAnimations[index].LastAnimationFrame > thisFrameData->AnimationFrame)
	      {
					RunningAnimations[index].LastAnimationFrame = 0;
					gbitmap_sequence_restart(AnimationBitmapSequences[index]);				
				}
					
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Frame wanted %d, NumFrames %d)", thisFrameData->AnimationFrame, (int)gbitmap_sequence_get_total_num_frames(AnimationBitmapSequences[index]));
        while(RunningAnimations[index].LastAnimationFrame < thisFrameData->AnimationFrame && 
             thisFrameData->AnimationFrame <= (int)gbitmap_sequence_get_total_num_frames(AnimationBitmapSequences[index]))
        {
          //APP_LOG(APP_LOG_LEVEL_DEBUG, "Advancing apng from %d by 1 towards %d (TotalFrames %d)", RunningAnimations[index].LastAnimationFrame, RunningAnimations[index].FrameData[RunningAnimations[index].CurrentFrame].AnimationFrame, (int)gbitmap_sequence_get_total_num_frames(AnimationBitmapSequences[index]));

          uint32_t next_delay;
          //flip back first to update before flipping back to mirror
          if(RunningAnimations[index].bMirror)
          {            
            FlipBitmap(AnimationBitmaps[index]);
          }
					
					bool bFailed = false;
          if(gbitmap_sequence_update_bitmap_next_frame(AnimationBitmapSequences[index], AnimationBitmaps[index], &next_delay))
          {
            RunningAnimations[index].LastAnimationFrame++;            
          }
          else
          {
            //Error
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "Failed to advance animation!");
            //break;
						bFailed = true;
          }
					
					//Flip even if it failed to advance of it'll cause the image to flip out
					if(RunningAnimations[index].bMirror)
          {            
            FlipBitmap(AnimationBitmaps[index]);
          }
					
					if(bFailed)
						break;
        }
                  
				if(REBUILDSEQEACHFRAME)
       		gbitmap_sequence_destroy(AnimationBitmapSequences[index]);
				
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing anim %d at frame %d", index, RunningAnimations[index].CurrentFrame);
        //Draw into correct place
				GRect BitSize = gbitmap_get_bounds(AnimationBitmaps[index]);
				int XPos;
        if(RunningAnimations[index].bMirror)
					XPos = RunningAnimations[index].StartXOffset - (RunningAnimations[index].StartXPos + thisFrameData->OffsetXPos + RunningAnimations[index].SizeX);
				else
					XPos = RunningAnimations[index].StartXPos + RunningAnimations[index].StartXOffset + thisFrameData->OffsetXPos;
				int YPos = RunningAnimations[index].StartYPos + RunningAnimations[index].StartYOffset - (thisFrameData->OffsetYPos + RunningAnimations[index].SizeY);
				
        graphics_draw_bitmap_in_rect(ctx, 
                                     AnimationBitmaps[index], 
                                     GRect(XPos, 
                                           YPos, 
                                           BitSize.size.w, 
                                           BitSize.size.h));
        
        //Update animation to next frame and see if over/loop
#ifdef PHONEBUILD
				RunningAnimations[index].CurrentFrame+=2;
#else
				RunningAnimations[index].CurrentFrame++;
#endif
       if(RunningAnimations[index].CurrentFrame >= RunningAnimations[index].NumFrames)
        {
          if(RunningAnimations[index].bLoop)
          {
					  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Loop of Animation %d, %d, %d, %d", index, RunningAnimations[index].StartYOffset, RunningAnimations[index].StartYOffset + RunningAnimations[index].FrameData[RunningAnimations[index].CurrentFrame - 1].OffsetYPos, RunningAnimations[index].CurrentFrame - 1);
						
						if(RunningAnimations[index].bMirror)
							RunningAnimations[index].StartXOffset = RunningAnimations[index].StartXOffset - (thisFrameData->OffsetXPos);
						else
							RunningAnimations[index].StartXOffset = RunningAnimations[index].StartXOffset + thisFrameData->OffsetXPos;
						RunningAnimations[index].StartYOffset = RunningAnimations[index].StartYOffset + thisFrameData->OffsetYPos;
							
						if(REBUILDSEQEACHFRAME == false)
						{
							gbitmap_sequence_restart(AnimationBitmapSequences[index]);
            	RunningAnimations[index].LastAnimationFrame = 0;
						}
						
            RunningAnimations[index].CurrentFrame = RunningAnimations[index].LoopFrame;
          }
          else
          {
						int XPos;
						
					  //APP_LOG(APP_LOG_LEVEL_DEBUG, "End of Animation %d", index);

						if(RunningAnimations[index].bMirror)
							XPos = RunningAnimations[index].StartXOffset - (thisFrameData->OffsetXPos);
						else
							XPos = RunningAnimations[index].StartXOffset + thisFrameData->OffsetXPos;
				
	          //Stop the animation
						int YPos = RunningAnimations[index].StartYOffset - thisFrameData->OffsetYPos;
            StopAnimation((AnimationHandle)index);

						//Call callback to notify caller its over
						if(RunningAnimations[index].EndCallback != NULL)
						{
							RunningAnimations[index].EndCallback(XPos, YPos);
						}  
          }
        }
      }
    }
  }
}

//////////////////////////////////////
//Animation stuff
void UpdateAnimations()
{
  if(NumRunningAnimations > 0)
  {
    //Queue next update
    layer_mark_dirty(Main_Layer);
    app_timer_register(67, UpdateAnimations, NULL); //15fps
  }
}

bool GetCurrentAnimationPos(AnimationHandle AnimHandle, int *ProjX, int *ProjY)
{
  if(bAnimationsInUse[AnimHandle] == false)
		return false;

	struct AnimationFrameData *thisFrameData = &(RunningAnimations[AnimHandle].FrameData[RunningAnimations[AnimHandle].CurrentFrame]);
	if(RunningAnimations[AnimHandle].bMirror)
		*ProjX = RunningAnimations[AnimHandle].StartXOffset - thisFrameData->OffsetXPos;
	else
		*ProjX = RunningAnimations[AnimHandle].StartXOffset + thisFrameData->OffsetXPos;

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "GetAnimY %d %d %d", RunningAnimations[AnimHandle].CurrentFrame, RunningAnimations[AnimHandle].StartYOffset, RunningAnimations[AnimHandle].FrameData[RunningAnimations[AnimHandle].CurrentFrame].OffsetYPos);
	*ProjY = RunningAnimations[AnimHandle].StartYOffset - thisFrameData->OffsetYPos;

	return true;
}

AnimationHandle AddAnimation(struct AnimationSequence *NewAnimation, int StartXOffset, int StartYOffset, bool bMirror)
{
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Adding Animation");

  //find a free sequence
  for(int index = 0; index < MaxAnimations; ++index)
  {
    if(bAnimationsInUse[index] == false)
    {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Into Slot %d", index);
      bAnimationsInUse[index] = true;
      
      //Create a new entry
      RunningAnimations[index].CurrentFrame = 0;
      RunningAnimations[index].LastAnimationFrame = 0;
      RunningAnimations[index].bLoop = NewAnimation->bLoop;
      RunningAnimations[index].LoopFrame = NewAnimation->LoopFrame;
      RunningAnimations[index].NumFrames = NewAnimation->NumFrames;
      RunningAnimations[index].StartXPos = NewAnimation->StartXPos;
			RunningAnimations[index].StartXOffset = StartXOffset;
      RunningAnimations[index].StartYPos = NewAnimation->StartYPos;
			RunningAnimations[index].StartYOffset = StartYOffset;
      RunningAnimations[index].SizeX = NewAnimation->SizeX;
      RunningAnimations[index].SizeY = NewAnimation->SizeY;
      RunningAnimations[index].ResourceID = NewAnimation->ResourceID;
      
      RunningAnimations[index].bMirror = bMirror;
      
      RunningAnimations[index].EndCallback = NewAnimation->EndCallback;

			RunningAnimations[index].FrameData = (struct AnimationFrameData *)malloc(sizeof(struct AnimationFrameData) * NewAnimation->NumFrames);
      for(int FrameIndex = 0; FrameIndex < NewAnimation->NumFrames; ++FrameIndex)
      {
        RunningAnimations[index].FrameData[FrameIndex].OffsetXPos = NewAnimation->FrameData[FrameIndex].OffsetXPos;
        RunningAnimations[index].FrameData[FrameIndex].OffsetYPos = NewAnimation->FrameData[FrameIndex].OffsetYPos;
        RunningAnimations[index].FrameData[FrameIndex].AnimationFrame = NewAnimation->FrameData[FrameIndex].AnimationFrame;
      }
      
      //Initialise the bitmap,bitmapsequence
      // Create sequence
  		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Loadanim");
      AnimationBitmapSequences[index] = gbitmap_sequence_create_with_resource(RunningAnimations[index].ResourceID);
  		/*if(AnimationBitmapSequences[index] )
				APP_LOG(APP_LOG_LEVEL_DEBUG, "AnimLoaded");
			else
				APP_LOG(APP_LOG_LEVEL_DEBUG, "AnimFAIL to load");*/

      // Create GBitmap
			GSize BS = gbitmap_sequence_get_bitmap_size(AnimationBitmapSequences[index]);
      if(REBUILDSEQEACHFRAME)
				gbitmap_sequence_destroy(AnimationBitmapSequences[index]);
      AnimationBitmaps[index] = gbitmap_create_blank(BS, GBitmapFormat8Bit);
  		/*if(AnimationBitmaps[index])
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Anim BMP alloc");
			else
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Anim BMP FAIL to alloc");*/
			//SavedSpace[index] = malloc(sizeof(uint8_t) * (4310 - (BS.w * BS.h)));
			//APP_LOG(APP_LOG_LEVEL_INFO, "Saving animation space %d (%d x %d) at %d", (4310 - (BS.w * BS.h)), BS.w, BS.h, (int)(0x20030000 - (long int)SavedSpace[index]));
  		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Anim Size %d %d", gbitmap_sequence_get_bitmap_size(AnimationBitmapSequences[index]).w, gbitmap_sequence_get_bitmap_size(AnimationBitmapSequences[index]).h);
		
			//APP_LOG(APP_LOG_LEVEL_INFO, "Anim allocated to, %d - %d", /*(int)(0x20030000 - (long int)AnimationBitmapSequences[index]),*/ (int)(0x20030000 - (long int)AnimationBitmaps[index]), (int)(0x20030000 - (long int)gbitmap_get_data(AnimationBitmaps[index])));
    
      if(NumRunningAnimations == 0)
      {
        app_timer_register(67, UpdateAnimations, NULL); //15fps
      }
      
      NumRunningAnimations++;
      
      return index;
    }
  }
  
  return -1;
}

void StopAnimation(AnimationHandle AnimToStop)
{
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopping Animation %d", AnimToStop);
  if(AnimToStop >= 0 && bAnimationsInUse[AnimToStop] == true)
  {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Removing Animation %d", AnimToStop);
    bAnimationsInUse[AnimToStop] = false;
		free(RunningAnimations[AnimToStop].FrameData);
		//free(SavedSpace[AnimToStop]);
		if(REBUILDSEQEACHFRAME == false)
    	gbitmap_sequence_destroy(AnimationBitmapSequences[AnimToStop]);
    gbitmap_destroy(AnimationBitmaps[AnimToStop]);
  
    NumRunningAnimations--;
  }
}

//////////////////////////////////////
//Init stuff
static void CreateAnimationLayer(Window *ParentWindow)
{
  Main_Layer = layer_create(GRect(0, 0, 144, 168));
  
  layer_add_child(window_get_root_layer(ParentWindow), Main_Layer);  

  layer_set_update_proc(Main_Layer, DrawAnimations);
}

inline void LoadAutoAnimation(Window *ParentWindow)
{
  //Make sure all init'd
  for(int index = 0; index < MaxAnimations; ++index)
  {
    bAnimationsInUse[index] = false;
		RunningAnimations[index].FrameData = NULL;
  }
  
  CreateAnimationLayer(ParentWindow);
}

//////////////////////////////////////
//De-init stuff
inline void UnloadAutoAnimation()
{
  // Destroy remaining BMP sequences
  int index;
  for(index = 0; index < MaxAnimations; ++index)
  {
    if(bAnimationsInUse[index])
    {
      bAnimationsInUse[index] = false;
			free(RunningAnimations[index].FrameData);
			//free(SavedSpace[index]);
      if(REBUILDSEQEACHFRAME == false)
				gbitmap_sequence_destroy(AnimationBitmapSequences[index]);
      gbitmap_destroy(AnimationBitmaps[index]);
    }
  }
  
  //Remove layer
  layer_destroy(Main_Layer);
}