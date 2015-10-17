#include <pebble.h>
#include "Characters.h"
#include "AutoAnimationEngine.h"

uint16_t ChrIdx = 0;
char *TextBuffer;

int GetNextDataInt()
{
	int TempIdx = 0;
	char TempBuffer[4];
		
	for(; TextBuffer[ChrIdx] != ','; ++(ChrIdx), ++TempIdx)
	{
		TempBuffer[TempIdx] = TextBuffer[ChrIdx];
	}
	
	TempBuffer[TempIdx] = '\0';
	(ChrIdx)++;
	
	return atoi(TempBuffer);
}
	
void CreateCharacterAnim(struct AnimationSequence *AnimSeq, int DataResourceId, int AnimationResourceId)
{
	AnimSeq->ResourceID = AnimationResourceId;
	
	// Get resource and size
  ResHandle handle = resource_get_handle(DataResourceId);
  size_t res_size = resource_size(handle);

  // Copy to buffer
  TextBuffer = (char*)malloc(res_size);
  resource_load(handle, (uint8_t*)TextBuffer, res_size);
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuWInData %s", TextBuffer);

	ChrIdx = 0;
	
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuData NumFrames %s %d", TempBuffer, atoi(TempBuffer));
	AnimSeq->NumFrames = GetNextDataInt();
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuData bLoop %c", TextBuffer[ChrIdx]);
  AnimSeq->bLoop = TextBuffer[ChrIdx] != '0';
	ChrIdx+=2; //skip ,
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuData LoopFrame %s %d", TempBuffer, atoi(TempBuffer));
  AnimSeq->LoopFrame = GetNextDataInt();
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuData StartXPos %s %d", TempBuffer, atoi(TempBuffer));
  AnimSeq->StartXPos = GetNextDataInt();
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuData StartYPos %s %d", TempBuffer, atoi(TempBuffer));
  AnimSeq->StartYPos = GetNextDataInt();
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuData SizeX %s %d", TempBuffer, atoi(TempBuffer));
  AnimSeq->SizeX = GetNextDataInt();
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "RyuData SizeY %s %d", TempBuffer, atoi(TempBuffer));
  AnimSeq->SizeY = GetNextDataInt();
	AnimSeq->FrameData = (struct AnimationFrameData *)malloc(sizeof(struct AnimationFrameData) * AnimSeq->NumFrames);

	for(int index = 0; index < AnimSeq->NumFrames; index++)
	{
  	AnimSeq->FrameData[index].OffsetXPos = GetNextDataInt();
  	//APP_LOG(APP_LOG_LEVEL_DEBUG, "FrameData AnimSeq->FrameData[index].OffsetXPos=%d", AnimSeq->FrameData[index].OffsetXPos);
  	AnimSeq->FrameData[index].OffsetYPos = GetNextDataInt();
  	//APP_LOG(APP_LOG_LEVEL_DEBUG, "FrameData AnimSeq->FrameData[index].OffsetYPos=%d", AnimSeq->FrameData[index].OffsetYPos);
	  AnimSeq->FrameData[index].AnimationFrame = GetNextDataInt();
  	//APP_LOG(APP_LOG_LEVEL_DEBUG, "FrameData AnimSeq->FrameData[index].AnimationFrame=%d", AnimSeq->FrameData[index].AnimationFrame);
	}
	
	free(TextBuffer);
}

void FreeCharacter(enum Characters CharToFree)
{
	if(CharacterAnims[CharToFree] != NULL)
	{
		free(CharacterAnims[CharToFree]->WinPose.FrameData);
		free(CharacterAnims[CharToFree]->ReadyPose.FrameData);
		free(CharacterAnims[CharToFree]->HitAnim.FrameData);
		free(CharacterAnims[CharToFree]->KOPose.FrameData);
		free(CharacterAnims[CharToFree]->Moves[0].FrameData);
		if(CharacterAnims[CharToFree]->Moves[1].FrameData)
			free(CharacterAnims[CharToFree]->Moves[1].FrameData);
		if(CharacterAnims[CharToFree]->ProjectileIdle.FrameData)
		{
			free(CharacterAnims[CharToFree]->ProjectileIdle.FrameData);
			free(CharacterAnims[CharToFree]->ProjectileHit.FrameData);
		}
		free(CharacterAnims[CharToFree]);
		CharacterAnims[CharToFree] = NULL;
  }
}