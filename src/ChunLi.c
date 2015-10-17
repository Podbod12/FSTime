#include <pebble.h>
#include "ChunLi.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateChunLiAnimations()
{
 	if(CharacterAnims[C_ChunLi] == NULL)
	{
		CharacterAnims[C_ChunLi] = malloc(sizeof(struct CharacterAnimationSet));

		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->WinPose), RESOURCE_ID_CHUNLIDATAWIN, RESOURCE_ID_IMAGE_CHUNLIWIN);
		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->ReadyPose), RESOURCE_ID_CHUNLIDATAIDLE, RESOURCE_ID_IMAGE_CHUNLIIDLE);

		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->Moves[0]), RESOURCE_ID_CHUNLIDATAKICKS, RESOURCE_ID_IMAGE_CHUNLIKICKS);
		CharacterAnims[C_ChunLi]->MoveHitFrame[0] = 17;
		CharacterAnims[C_ChunLi]->MoveSpawnProjectile[0] = false;
		CharacterAnims[C_ChunLi]->HitBurstSpawnOffsetX[0] = 49;
		CharacterAnims[C_ChunLi]->HitBurstSpawnOffsetY[0] = -55;

		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->Moves[1]), RESOURCE_ID_CHUNLIDATAFBMOTION, RESOURCE_ID_IMAGE_CHUNLIFBMOTION);
		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->ProjectileIdle), RESOURCE_ID_CHUNLIDATAFBIDLE, RESOURCE_ID_IMAGE_CHUNLIFBIDLE);
		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->ProjectileHit), RESOURCE_ID_CHUNLIDATAFBHIT, RESOURCE_ID_IMAGE_CHUNLIFBHIT);
		CharacterAnims[C_ChunLi]->MoveHitFrame[1] = 10;
		CharacterAnims[C_ChunLi]->MoveSpawnProjectile[1] = true;
	  CharacterAnims[C_ChunLi]->ProjectileHitFrame = 15;
	  CharacterAnims[C_ChunLi]->ProjectileSpawnOffsetX = 46;
 	  CharacterAnims[C_ChunLi]->ProjectileSpawnOffsetY = -33;		
		
		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->HitAnim), RESOURCE_ID_CHUNLIDATAHIT, RESOURCE_ID_IMAGE_CHUNLIHIT);

		CreateCharacterAnim(&(CharacterAnims[C_ChunLi]->KOPose), RESOURCE_ID_CHUNLIDATAKO, RESOURCE_ID_IMAGE_CHUNLIKO);
	}
}