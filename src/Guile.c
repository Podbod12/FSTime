#include <pebble.h>
#include "Guile.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateGuileAnimations()
{
 	if(CharacterAnims[C_Guile] == NULL)
	{
		CharacterAnims[C_Guile] = malloc(sizeof(struct CharacterAnimationSet));

		CreateCharacterAnim(&(CharacterAnims[C_Guile]->WinPose), RESOURCE_ID_GUILEDATAWIN, RESOURCE_ID_IMAGE_GUILEWIN);
		CreateCharacterAnim(&(CharacterAnims[C_Guile]->ReadyPose), RESOURCE_ID_GUILEDATAIDLE, RESOURCE_ID_IMAGE_GUILEIDLE);

		CreateCharacterAnim(&(CharacterAnims[C_Guile]->Moves[0]), RESOURCE_ID_GUILEDATASBMOTION, RESOURCE_ID_IMAGE_GUILESBMOTION);
		CreateCharacterAnim(&(CharacterAnims[C_Guile]->ProjectileIdle), RESOURCE_ID_GUILEDATASBIDLE, RESOURCE_ID_IMAGE_GUILESBIDLE);
		CreateCharacterAnim(&(CharacterAnims[C_Guile]->ProjectileHit), RESOURCE_ID_GUILEDATASBHIT, RESOURCE_ID_IMAGE_GUILESBHIT);
		CharacterAnims[C_Guile]->MoveHitFrame[0] = 10;
		CharacterAnims[C_Guile]->MoveSpawnProjectile[0] = true;
	  CharacterAnims[C_Guile]->ProjectileHitFrame = 15;
	  CharacterAnims[C_Guile]->ProjectileSpawnOffsetX = 46;
 	  CharacterAnims[C_Guile]->ProjectileSpawnOffsetY = -33;		
		
		//No second move
 		//APP_LOG(APP_LOG_LEVEL_DEBUG, "GUILE FLASH"); 
		CreateCharacterAnim(&(CharacterAnims[C_Guile]->Moves[1]), RESOURCE_ID_GUILEDATAFLASHKICK, RESOURCE_ID_IMAGE_GUILEFLASHKICK);
		CharacterAnims[C_Guile]->MoveHitFrame[1] = 11;
		CharacterAnims[C_Guile]->MoveSpawnProjectile[1] = false;
		CharacterAnims[C_Guile]->HitBurstSpawnOffsetX[1] = 50;
		CharacterAnims[C_Guile]->HitBurstSpawnOffsetY[1] = -32;
		//CharacterAnims[C_Guile]->MoveHitFrame[1] = 0;
		//CharacterAnims[C_Guile]->Moves[1].FrameData = NULL;

		
		CreateCharacterAnim(&(CharacterAnims[C_Guile]->HitAnim), RESOURCE_ID_GUILEDATAHIT, RESOURCE_ID_IMAGE_GUILEHIT);

		CreateCharacterAnim(&(CharacterAnims[C_Guile]->KOPose), RESOURCE_ID_GUILEDATAKO, RESOURCE_ID_IMAGE_GUILEKO);
	}
}
