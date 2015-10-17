#include <pebble.h>
#include "Dhalsim.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateDhalsimAnimations()
{
 	if(CharacterAnims[C_Dhalsim] == NULL)
	{
		CharacterAnims[C_Dhalsim] = malloc(sizeof(struct CharacterAnimationSet));

		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->WinPose), RESOURCE_ID_DHALSIMDATAWIN, RESOURCE_ID_IMAGE_DHALSIMWIN);
		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->ReadyPose), RESOURCE_ID_DHALSIMDATAIDLE, RESOURCE_ID_IMAGE_DHALSIMIDLE);

		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->Moves[0]), RESOURCE_ID_DHALSIMDATAFLAME, RESOURCE_ID_IMAGE_DHALSIMFLAME);
		CharacterAnims[C_Dhalsim]->MoveHitFrame[0] = 11;
		CharacterAnims[C_Dhalsim]->MoveSpawnProjectile[0] = false;
		CharacterAnims[C_Dhalsim]->HitBurstSpawnOffsetX[0] = 63;
		CharacterAnims[C_Dhalsim]->HitBurstSpawnOffsetY[0] = -40;
		
		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->Moves[1]), RESOURCE_ID_DHALSIMDATAFIREMOTION, RESOURCE_ID_IMAGE_DHALSIMFIREMOTION);
		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->ProjectileIdle), RESOURCE_ID_DHALSIMDATAFIREIDLE, RESOURCE_ID_IMAGE_DHALSIMFIREIDLE);
		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->ProjectileHit), RESOURCE_ID_DHALSIMDATAFIREHIT, RESOURCE_ID_IMAGE_DHALSIMFIREHIT);
		CharacterAnims[C_Dhalsim]->MoveHitFrame[1] = 10;
		CharacterAnims[C_Dhalsim]->MoveSpawnProjectile[1] = true;
	  CharacterAnims[C_Dhalsim]->ProjectileHitFrame = 15;
		CharacterAnims[C_Dhalsim]->ProjectileSpawnOffsetX = 46;
 	  CharacterAnims[C_Dhalsim]->ProjectileSpawnOffsetY = -33;		
		
		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->HitAnim), RESOURCE_ID_DHALSIMDATAHIT, RESOURCE_ID_IMAGE_DHALSIMHIT);

		CreateCharacterAnim(&(CharacterAnims[C_Dhalsim]->KOPose), RESOURCE_ID_DHALSIMDATAKO, RESOURCE_ID_IMAGE_DHALSIMKO);
	}
}
