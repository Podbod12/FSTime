#pragma once
  
typedef int AnimationHandle;
#define ANIM_NOSPACE -1

struct AnimationFrameData
{
  int8_t OffsetXPos;
  int8_t OffsetYPos;
  
  uint8_t AnimationFrame;
};

struct AnimationSequence
{
  struct AnimationFrameData *FrameData;

	void (*EndCallback)(int,int);

  int ResourceID;

	int8_t StartXPos;
  int8_t StartYPos;
  
  int16_t StartXOffset;
  int16_t StartYOffset;
	
	uint8_t CurrentFrame;
  uint8_t NumFrames;
  
  uint8_t LastAnimationFrame;
  
  uint8_t LoopFrame;
  
	uint8_t SizeX;
  uint8_t SizeY;
	    
  bool bLoop;

	bool bMirror;
 };

#define MaxAnimations 3
  
Layer *GetAnimLayer();
	
void LoadAutoAnimation(Window *ParentWindow);
void UnloadAutoAnimation();

bool GetCurrentAnimationPos(AnimationHandle AnimHandle, int *ProjX, int *ProjY);

AnimationHandle AddAnimation(struct AnimationSequence *NewAnimation, int StartXOffset, int StartYOffset, bool bMirror);
void StopAnimation(AnimationHandle AnimToStop);