#include <pebble.h>
#include "Blanka.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateBlankaAnimations()
{
 	if(CharacterAnims[C_Blanka] == NULL)
	{
		CharacterAnims[C_Blanka] = malloc(sizeof(struct CharacterAnimationSet));

		CreateCharacterAnim(&(CharacterAnims[C_Blanka]->WinPose), RESOURCE_ID_BLANKADATAWIN, RESOURCE_ID_IMAGE_BLANKAWIN);
		CreateCharacterAnim(&(CharacterAnims[C_Blanka]->ReadyPose), RESOURCE_ID_BLANKADATAIDLE, RESOURCE_ID_IMAGE_BLANKAIDLE);

		CreateCharacterAnim(&(CharacterAnims[C_Blanka]->Moves[0]), RESOURCE_ID_BLANKADATAHOPELECTRIC, RESOURCE_ID_IMAGE_BLANKAHOPELECTRIC);
		CharacterAnims[C_Blanka]->MoveHitFrame[0] = 17;
		CharacterAnims[C_Blanka]->MoveSpawnProjectile[0] = false;
		CharacterAnims[C_Blanka]->HitBurstSpawnOffsetX[0] = 35;
		CharacterAnims[C_Blanka]->HitBurstSpawnOffsetY[0] = -32;
		
		//No second move
		CharacterAnims[C_Blanka]->MoveHitFrame[1] = 0;
		CharacterAnims[C_Blanka]->Moves[1].FrameData = NULL;
	
		//No Projectile
		CharacterAnims[C_Blanka]->ProjectileHit.FrameData = NULL;
		CharacterAnims[C_Blanka]->ProjectileIdle.FrameData = NULL;
			
		CreateCharacterAnim(&(CharacterAnims[C_Blanka]->HitAnim), RESOURCE_ID_BLANKADATAHIT, RESOURCE_ID_IMAGE_BLANKAHIT);

		CreateCharacterAnim(&(CharacterAnims[C_Blanka]->KOPose), RESOURCE_ID_BLANKADATAKO, RESOURCE_ID_IMAGE_BLANKAKO);
	}
}