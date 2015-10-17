#include <pebble.h>
#include "Honda.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateHondaAnimations()
{
 	if(CharacterAnims[C_Honda] == NULL)
	{
		CharacterAnims[C_Honda] = malloc(sizeof(struct CharacterAnimationSet));

		CreateCharacterAnim(&(CharacterAnims[C_Honda]->WinPose), RESOURCE_ID_HONDADATAWIN, RESOURCE_ID_IMAGE_HONDAWIN);
		CreateCharacterAnim(&(CharacterAnims[C_Honda]->ReadyPose), RESOURCE_ID_HONDADATAIDLE, RESOURCE_ID_IMAGE_HONDAIDLE);

		CreateCharacterAnim(&(CharacterAnims[C_Honda]->Moves[0]), RESOURCE_ID_HONDADATAHANDSLAPSHORT, RESOURCE_ID_IMAGE_HONDAHANDSLAP);
		CharacterAnims[C_Honda]->MoveHitFrame[0] = 12;
		CharacterAnims[C_Honda]->MoveSpawnProjectile[0] = false;
		CharacterAnims[C_Honda]->HitBurstSpawnOffsetX[0] = 75;
		CharacterAnims[C_Honda]->HitBurstSpawnOffsetY[0] = -55;
		
		CreateCharacterAnim(&(CharacterAnims[C_Honda]->Moves[1]), RESOURCE_ID_HONDADATAHEADBUTT, RESOURCE_ID_IMAGE_HONDAHEADBUTT);
		CharacterAnims[C_Honda]->MoveHitFrame[1] = 11;
		CharacterAnims[C_Honda]->MoveSpawnProjectile[1] = false;
		CharacterAnims[C_Honda]->HitBurstSpawnOffsetX[1] = 75;
		CharacterAnims[C_Honda]->HitBurstSpawnOffsetY[1] = -30;
		
		//No Projectile
		CharacterAnims[C_Honda]->ProjectileHit.FrameData = NULL;
		CharacterAnims[C_Honda]->ProjectileIdle.FrameData = NULL;

		CreateCharacterAnim(&(CharacterAnims[C_Honda]->HitAnim), RESOURCE_ID_HONDADATAHIT, RESOURCE_ID_IMAGE_HONDAHIT);

		CreateCharacterAnim(&(CharacterAnims[C_Honda]->KOPose), RESOURCE_ID_HONDADATAKO, RESOURCE_ID_IMAGE_HONDAKO);
	}
}
