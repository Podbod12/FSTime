#pragma once
	
#include "AutoAnimationEngine.h"

enum Characters
{
  C_Ryu,
  C_Ken,
  C_Honda,
  C_Guile,
  C_Blanka,
  C_Zangief,
  C_Dhalsim,
  C_ChunLi,
	
	C_MAX,
};

struct CharacterAnimationSet
{
  struct AnimationSequence HitAnim;
	struct AnimationSequence KOPose;
  struct AnimationSequence ProjectileIdle;
  struct AnimationSequence ProjectileHit;
  struct AnimationSequence ReadyPose;
	struct AnimationSequence WinPose;
 	struct AnimationSequence Moves[2];
	
	uint8_t MoveHitFrame[3];
	uint8_t ProjectileHitFrame;
	int8_t ProjectileSpawnOffsetX;
 	int8_t ProjectileSpawnOffsetY;
	
	int8_t HitBurstSpawnOffsetX[2];
 	int8_t HitBurstSpawnOffsetY[2];

	bool MoveSpawnProjectile[3];
};

struct CharacterAnimationSet *CharacterAnims[C_MAX];

void CreateCharacterAnim(struct AnimationSequence *AnimSeq, int DataResourceId, int AnimationResourceId);
void FreeCharacter(enum Characters CharToFree);