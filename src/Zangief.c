#include <pebble.h>
#include "Zangief.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateZangiefAnimations()
{
 	if(CharacterAnims[C_Zangief] == NULL)
	{
		CharacterAnims[C_Zangief] = malloc(sizeof(struct CharacterAnimationSet));

		CreateCharacterAnim(&(CharacterAnims[C_Zangief]->WinPose), RESOURCE_ID_ZANGIEFDATAWIN, RESOURCE_ID_IMAGE_ZANGIEFWIN);

		CreateCharacterAnim(&(CharacterAnims[C_Zangief]->ReadyPose), RESOURCE_ID_ZANGIEFDATAIDLE, RESOURCE_ID_IMAGE_ZANGIEFIDLE);
		
		CreateCharacterAnim(&(CharacterAnims[C_Zangief]->Moves[0]), RESOURCE_ID_ZANGIEFDATALARIAT, RESOURCE_ID_IMAGE_ZANGIEFLARIAT);
		CharacterAnims[C_Zangief]->MoveHitFrame[0] = 20;
		CharacterAnims[C_Zangief]->MoveSpawnProjectile[0] = false;
		CharacterAnims[C_Zangief]->HitBurstSpawnOffsetX[0] = 64;
		CharacterAnims[C_Zangief]->HitBurstSpawnOffsetY[0] = -45;
				
		//No second move
		CharacterAnims[C_Zangief]->MoveHitFrame[1] = 0;
		CharacterAnims[C_Zangief]->Moves[1].FrameData = NULL;

		//No Projectile
		CharacterAnims[C_Zangief]->ProjectileHit.FrameData = NULL;
		CharacterAnims[C_Zangief]->ProjectileIdle.FrameData = NULL;

		CreateCharacterAnim(&(CharacterAnims[C_Zangief]->HitAnim), RESOURCE_ID_ZANGIEFDATAHIT, RESOURCE_ID_IMAGE_ZANGIEFHIT);

		CreateCharacterAnim(&(CharacterAnims[C_Zangief]->KOPose), RESOURCE_ID_ZANGIEFDATAKO, RESOURCE_ID_IMAGE_ZANGIEFKO);
	}
}
