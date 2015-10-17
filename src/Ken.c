#include <pebble.h>
#include "Ken.h"
#include "AutoAnimationEngine.h"
#include "main.h"
#include "Characters.h"

inline void CreateKenAnimations()
{
 	if(CharacterAnims[C_Ken] == NULL)
	{
		CharacterAnims[C_Ken] = malloc(sizeof(struct CharacterAnimationSet));

		struct CharacterAnimationSet *Ken = (CharacterAnims[C_Ken]);
	
		CreateCharacterAnim(&(Ken->WinPose), RESOURCE_ID_KENDATAWIN, RESOURCE_ID_IMAGE_KENWIN);
		CreateCharacterAnim(&(Ken->ReadyPose), RESOURCE_ID_KENDATAIDLE, RESOURCE_ID_IMAGE_KENIDLE);

		CreateCharacterAnim(&(Ken->Moves[0]), RESOURCE_ID_KENDATAFIREDP, RESOURCE_ID_IMAGE_KENFIREDP);
		Ken->MoveHitFrame[0] = 11;
		Ken->MoveSpawnProjectile[0] = false;
		CharacterAnims[C_Ken]->HitBurstSpawnOffsetX[0] = 25;
		CharacterAnims[C_Ken]->HitBurstSpawnOffsetY[0] = -40;
		
		CreateCharacterAnim(&(Ken->Moves[1]), RESOURCE_ID_KENDATAFBMOTION, RESOURCE_ID_IMAGE_KENFBMOTION);
		CreateCharacterAnim(&(Ken->ProjectileIdle), RESOURCE_ID_KENDATAFBIDLE, RESOURCE_ID_IMAGE_KENFIREBALLIDLE);
		CreateCharacterAnim(&(Ken->ProjectileHit), RESOURCE_ID_KENDATAFBHIT, RESOURCE_ID_IMAGE_KENFIREBALLHIT);
		Ken->MoveHitFrame[1] = 10;
		Ken->MoveSpawnProjectile[1] = true;
	  Ken->ProjectileHitFrame = 15;
	  Ken->ProjectileSpawnOffsetX = 46;
 	  Ken->ProjectileSpawnOffsetY = -33;
				
		CreateCharacterAnim(&(Ken->HitAnim), RESOURCE_ID_KENDATAHIT, RESOURCE_ID_IMAGE_KENHIT);

		CreateCharacterAnim(&(Ken->KOPose), RESOURCE_ID_KENDATAKO, RESOURCE_ID_IMAGE_KENKO);
	}
}