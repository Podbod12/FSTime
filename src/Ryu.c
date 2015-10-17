#include <pebble.h>
#include "Ryu.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateRyuAnimations()
{
 	if(CharacterAnims[C_Ryu] == NULL)
	{
		CharacterAnims[C_Ryu] = malloc(sizeof(struct CharacterAnimationSet));

		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->WinPose), RESOURCE_ID_RYUDATAWIN, RESOURCE_ID_IMAGE_RYUWIN);
		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->ReadyPose), RESOURCE_ID_RYUDATAIDLE, RESOURCE_ID_IMAGE_RYUIDLE);

		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->Moves[0]), RESOURCE_ID_RYUDATADP, RESOURCE_ID_IMAGE_RYUDP);
		CharacterAnims[C_Ryu]->MoveHitFrame[0] = 11;
		CharacterAnims[C_Ryu]->MoveSpawnProjectile[0] = false;
		CharacterAnims[C_Ryu]->HitBurstSpawnOffsetX[0] = 25;
		CharacterAnims[C_Ryu]->HitBurstSpawnOffsetY[0] = -40;
				
		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->Moves[1]), RESOURCE_ID_RYUDATAFBMOTION, RESOURCE_ID_IMAGE_RYUFBMOTION);
		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->ProjectileIdle), RESOURCE_ID_RYUDATAFBIDLE, RESOURCE_ID_IMAGE_KENFIREBALLIDLE);
		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->ProjectileHit), RESOURCE_ID_RYUDATAFBHIT, RESOURCE_ID_IMAGE_KENFIREBALLHIT);
		CharacterAnims[C_Ryu]->MoveHitFrame[1] = 10;
		CharacterAnims[C_Ryu]->MoveSpawnProjectile[1] = true;
	  CharacterAnims[C_Ryu]->ProjectileHitFrame = 15;
	  CharacterAnims[C_Ryu]->ProjectileSpawnOffsetX = 46;
 	  CharacterAnims[C_Ryu]->ProjectileSpawnOffsetY = -33;	
		CharacterAnims[C_Ryu]->HitBurstSpawnOffsetX[1] = 24;
		CharacterAnims[C_Ryu]->HitBurstSpawnOffsetY[1] = 0;

		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->HitAnim), RESOURCE_ID_RYUDATAHIT, RESOURCE_ID_IMAGE_RYUHIT);

		CreateCharacterAnim(&(CharacterAnims[C_Ryu]->KOPose), RESOURCE_ID_RYUDATAKO, RESOURCE_ID_IMAGE_RYUKO);
	}
}