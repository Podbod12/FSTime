#include <pebble.h>
#include "MatchEngine.h"
#include "WipeEffect.h"
#include "Settings.h"
#include "main.h"
#include "Ryu.h"
#include "Ken.h"
#include "Honda.h"
#include "Zangief.h"
#include "Dhalsim.h"
#include "Guile.h"
#include "Blanka.h"
#include "ChunLi.h"
#include "BetweenFightPage.h"
#include "BitmapFlip.h"

#ifdef PHONEBUILD
	#define SUSPENSETIME 5
	#define ROUNDTIME 10
	#define FIGHTTIME 10
	#define WAITFORMOVETIME 5
	#define KOWAITTIME 15
	#define WINPOSEWAITTIME 15
	#define WINLOSSTIME 15
	#define TOURNEYWONTIME 90
	
	#define ANIMFRAMETIME 67
#else
	#define SUSPENSETIME 10
	#define ROUNDTIME 20
	#define FIGHTTIME 20
	#define WAITFORMOVETIME 10
	#define KOWAITTIME 30
	#define WINPOSEWAITTIME 30
	#define WINLOSSTIME 30
	#define TOURNEYWONTIME 60
	
	#define ANIMFRAMETIME 67
#endif
	
#define PLR1STARTX 42
#define PLR2STARTX 102
#define PLRSTARTY 162
	
#define ROUNDICONCENTEROFFSET 1
#define ROUNDICONY 64
	
#define HEALTHY 50
#define HEALTHSIZEX 63
#define HEALTHSIZEY 13
	
#define SHADOWSTARTY 152

enum FightState
{
	FS_WaitForWipe,
	FS_WaitForStart,
	FS_Round,
	FS_Fight,
	FS_WaitForMove,
	FS_Animating,
	FS_KODelay,
	FS_WinPose,
	FS_WinLoss,
	FS_WaitForWipeOutTourneyWon,
	FS_TourneyWon,
	FS_WaitForWipeOut,
	FS_WaitForWipeOutAfterTourneyWon,
	FS_End,
};

enum FightState CurrentFightState;

int StateTimer;

AnimationHandle Plr1MoveHandle;
AnimationHandle Plr2MoveHandle;
AnimationHandle ProjectileHandle;

bool bPlayerWon;

uint8_t LoserHealth;

int Player1XPos;
int Player1YPos;
int Player2XPos;
int Player2YPos;

bool bMoveOver;
bool bHitOver;

uint8_t RWins, RLosses, Rounds;

static BitmapLayer *StageLayer;
static GBitmap *StageBitmap;
static BitmapLayer *StageLayerB;
static GBitmap *StageBitmapB;

static Layer *RoundsLayer;
static GBitmap *RoundBackBitmap;
static GBitmap *RoundWinBitmap;

static GBitmap *HealthBitmap;
static GBitmap *Health2Bitmap;
static Layer *HealthLayer;

static GBitmap *SmallShadowBitmap;
static GBitmap *LargeShadowBitmap;
static Layer *ShadowLayer;

static Layer *StarburstLayer;
static GBitmap *StarburstBitmap;
int8_t StarburstTimer;
uint8_t StarX;
uint8_t StarY;

static BitmapLayer *WinLossLayer;
static GBitmap *WinLossBitmap;
static TextLayer *RoundTextLayer;
static TextLayer *RoundTextDropLayer;
static GFont MainFont28;
bool bFontLoaded;

bool bUpdateBackground;

uint8_t MoveSelected;

uint8_t Stages[C_MAX] = 
{
	RESOURCE_ID_IMAGE_RYUSTAGE16,
	RESOURCE_ID_IMAGE_KENSTAGE16,
	RESOURCE_ID_IMAGE_HONDASTAGE16,
	RESOURCE_ID_IMAGE_GUILESTAGE16,
	RESOURCE_ID_IMAGE_BLANKASTAGE16,
	RESOURCE_ID_IMAGE_ZANGIEFSTAGE16,
	RESOURCE_ID_IMAGE_DHALSIMSTAGE16,
	RESOURCE_ID_IMAGE_CHUNLISTAGE16
};
uint8_t StagesBottom[C_MAX] = 
{
	RESOURCE_ID_IMAGE_RYUSTAGE16b,
	RESOURCE_ID_IMAGE_KENSTAGE16b,
	RESOURCE_ID_IMAGE_HONDASTAGE16b,
	RESOURCE_ID_IMAGE_GUILESTAGE16b,
	RESOURCE_ID_IMAGE_BLANKASTAGE16b,
	RESOURCE_ID_IMAGE_ZANGIEFSTAGE16b,
	RESOURCE_ID_IMAGE_DHALSIMSTAGE16b,
	RESOURCE_ID_IMAGE_CHUNLISTAGE16b
};

uint8_t RoundNoIds[5] = 
{
	RESOURCE_ID_IMAGE_ROUNDONE,
	RESOURCE_ID_IMAGE_ROUNDTWO,
	RESOURCE_ID_IMAGE_ROUNDTHREE,
	RESOURCE_ID_IMAGE_ROUNDFOUR,
	RESOURCE_ID_IMAGE_ROUNDFIVE,
};

struct CharacterAnimationSet *PlrChar;
struct CharacterAnimationSet *OppChar;

static char TexBuffer[10];

void (*LoadCharacterFuncs[C_MAX])() = 
{
	CreateRyuAnimations,
	CreateKenAnimations,
	CreateHondaAnimations,
	CreateGuileAnimations,
	CreateBlankaAnimations,
	CreateZangiefAnimations,
	CreateDhalsimAnimations,
	CreateChunLiAnimations,
};

//Utilities//////////////////////////////////////////////////////////////////
void LoadNeededCharacter(enum Characters CharacterToLoad)
{
	LoadCharacterFuncs[CharacterToLoad]();
}

void UnloadNeededCharacter(enum Characters CharacterToUnload)
{
	FreeCharacter(CharacterToUnload);
}

void DoesPlayerWin()
{
	bPlayerWon = HasDoneEnoughStepsToWin();
}

int PickMove()
{
#ifdef FORCEMOVE
	return FORCEMOVE - 1;
#endif
	
	if(bPlayerWon)
	{
		if(PlrChar->MoveHitFrame[1] != 0)
			return rand() % 2;
	}
	else
	{
		if(OppChar->MoveHitFrame[1] != 0)
			return rand() % 2;
	}

	return 0;
}

//Move updates//////////////////////////////////////////////////////////////////
void PlayWinPose()
{
	int ProjX, ProjY;
		
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "WinPose");

	if(bPlayerWon)
	{
		GetCurrentAnimationPos(Plr1MoveHandle, &ProjX, &ProjY);
		StopAnimation(Plr1MoveHandle);
		
		Plr1MoveHandle = AddAnimation(&(PlrChar->WinPose), ProjX, ProjY, false);
	}	
	else
	{
		GetCurrentAnimationPos(Plr2MoveHandle, &ProjX, &ProjY);
		StopAnimation(Plr2MoveHandle);
		
		Plr2MoveHandle = AddAnimation(&(OppChar->WinPose), ProjX, ProjY, true);
	}	
}

void DoNextMoveAfterMoveOrHit(int FinalXPos, int FinalYPos, struct AnimationSequence *AnimToPlay, bool bPlayer2)
{
	if(!bPlayer2)
	{
		Player1XPos = FinalXPos;
		Player1YPos = FinalYPos;
	}	
	else
	{
		Player2XPos = FinalXPos;
		Player2YPos = FinalYPos;
	}	
	
	AnimationHandle *MoveHandle = (bPlayer2 ? &Plr2MoveHandle : &Plr1MoveHandle);
	*MoveHandle = AddAnimation(AnimToPlay, FinalXPos, FinalYPos, bPlayer2);
}

void MoveOver(int FinalXPos, int FinalYPos)
{
	bMoveOver = true;
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "MoveOver");

	if(bHitOver && bMoveOver)
		CurrentFightState = FS_KODelay;

	DoNextMoveAfterMoveOrHit(FinalXPos, FinalYPos, (bPlayerWon ? &PlrChar->ReadyPose : &OppChar->ReadyPose), !bPlayerWon);
	/*if(bPlayerWon)
	{
		Player1XPos = FinalXPos;
		Player1YPos = FinalYPos;
		Plr1MoveHandle = AddAnimation(&(PlrChar->ReadyPose), Player1XPos, Player1YPos, false);
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "MoveOver %d %d %d", FinalXPos, FinalYPos, Plr1MoveHandle);
	}	
	else
	{
		Player2XPos = FinalXPos;
		Player2YPos = FinalYPos;
		Plr2MoveHandle = AddAnimation(&(OppChar->ReadyPose), Player2XPos, Player2YPos, true);
	}*/	
}

void HitOver(int FinalXPos, int FinalYPos)
{
	bHitOver = true;
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "HitOver");

	if(bHitOver && bMoveOver)
		CurrentFightState = FS_KODelay;
	
	DoNextMoveAfterMoveOrHit(FinalXPos, FinalYPos, (!bPlayerWon ? &PlrChar->KOPose : &OppChar->KOPose), bPlayerWon);
	/*if(bPlayerWon)
	{
		Player2XPos = FinalXPos;
		Player2YPos = FinalYPos;
		Plr2MoveHandle = AddAnimation(&(OppChar->KOPose), Player2XPos, Player2YPos, true);
	}	
	else
	{
		Player1XPos = FinalXPos;
		Player1YPos = FinalYPos;
		Plr1MoveHandle = AddAnimation(&(PlrChar->KOPose), Player1XPos, Player1YPos, false);
	}*/
}

void MoveLanded()
{
	int ProjX, ProjY;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "MoveLanded");
	
	LoserHealth--; //begin drain

	if(bPlayerWon)
	{
		StopAnimation(Plr2MoveHandle);
		OppChar->HitAnim.EndCallback = HitOver;
		Plr2MoveHandle = AddAnimation(&(OppChar->HitAnim), Player2XPos, Player2YPos, true);
		
		//starburst
		if(PlrChar->MoveSpawnProjectile[MoveSelected] == false)
		{
			GetCurrentAnimationPos(Plr1MoveHandle, &ProjX, &ProjY);

			StarburstTimer = 0;
			StarY = ProjY + PlrChar->HitBurstSpawnOffsetY[MoveSelected] - 24;
			StarX = ProjX + PlrChar->HitBurstSpawnOffsetX[MoveSelected];
		}
	}	
	else
	{
		StopAnimation(Plr1MoveHandle);
		PlrChar->HitAnim.EndCallback = HitOver;
		Plr1MoveHandle = AddAnimation(&(PlrChar->HitAnim), Player1XPos, Player1YPos, false);
	
		//starburst
		if(OppChar->MoveSpawnProjectile[MoveSelected] == false)
		{
			GetCurrentAnimationPos(Plr2MoveHandle, &ProjX, &ProjY);
			
			StarburstTimer = 0;
			StarY = ProjY + OppChar->HitBurstSpawnOffsetY[MoveSelected] - 24;
			StarX = ((ProjX - OppChar->HitBurstSpawnOffsetX[MoveSelected]) - 20);
		}
	}	
}

void ProjectileHit()
{
	int ProjX, ProjY;
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Projectilehit");

	GetCurrentAnimationPos(ProjectileHandle, &ProjX, &ProjY);
	StopAnimation(ProjectileHandle);
	MoveLanded();

	struct CharacterAnimationSet *CharToAnimate = OppChar;
	if(bPlayerWon)
	{
		CharToAnimate = PlrChar;
		//PlrChar->ProjectileHit.EndCallback = NULL;
		//ProjectileHandle = AddAnimation(&(PlrChar->ProjectileHit), ProjX, ProjY, false);
	}	
	/*else
	{
		OppChar->ProjectileHit.EndCallback = NULL;
		ProjectileHandle = AddAnimation(&(OppChar->ProjectileHit), ProjX, ProjY, true);
	}	*/
	
	CharToAnimate->ProjectileHit.EndCallback = NULL;
	ProjectileHandle = AddAnimation(&(CharToAnimate->ProjectileHit), ProjX, ProjY, !bPlayerWon);
}

void SpawnProjectile()
{
	int ProjX, ProjY;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Projectile spawn");
	
	if(bPlayerWon)
	{
		GetCurrentAnimationPos(Plr1MoveHandle, &ProjX, &ProjY);
		PlrChar->ProjectileIdle.EndCallback = NULL;
		ProjectileHandle = AddAnimation(&(PlrChar->ProjectileIdle), ProjX + PlrChar->ProjectileSpawnOffsetX, ProjY + PlrChar->ProjectileSpawnOffsetY, false);
		app_timer_register(ANIMFRAMETIME * PlrChar->ProjectileHitFrame, ProjectileHit, NULL);
	}	
	else
	{
		GetCurrentAnimationPos(Plr2MoveHandle, &ProjX, &ProjY);
		OppChar->ProjectileIdle.EndCallback = NULL;
		ProjX = (ProjX - OppChar->ProjectileSpawnOffsetX)/* - CharacterAnims[GetOpponentCharacter()].Moves[MoveSelected].SizeX*/;
		ProjY = ProjY + OppChar->ProjectileSpawnOffsetY;
		ProjectileHandle = AddAnimation(&(OppChar->ProjectileIdle), ProjX, ProjY, true);
		app_timer_register(ANIMFRAMETIME * OppChar->ProjectileHitFrame, ProjectileHit, NULL);
	}	
}

void PlayFightAnimation(AnimationHandle *MoveHandle, int x, int y, struct CharacterAnimationSet *CharToAnimate)
{
	StopAnimation(*MoveHandle);
	CharToAnimate->Moves[MoveSelected].EndCallback = MoveOver;
	*MoveHandle = AddAnimation(&(CharToAnimate->Moves[MoveSelected]), x, y, (*MoveHandle) == Plr2MoveHandle);

	if(CharToAnimate->MoveSpawnProjectile[MoveSelected] == false)
		app_timer_register(ANIMFRAMETIME * CharToAnimate->MoveHitFrame[MoveSelected], MoveLanded, NULL);
	else
		app_timer_register(ANIMFRAMETIME * CharToAnimate->MoveHitFrame[MoveSelected], SpawnProjectile, NULL);
}

void StartFightAnimations()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Start fight anims");

	MoveSelected = PickMove();
	
	if(bPlayerWon)
  {
		PlayFightAnimation(&Plr1MoveHandle, Player1XPos, Player1YPos, PlrChar);
		/*StopAnimation(Plr1MoveHandle);
		PlrChar->Moves[MoveSelected].EndCallback = MoveOver;
		Plr1MoveHandle = AddAnimation(&(PlrChar->Moves[MoveSelected]), Player1XPos, Player1YPos, false);

		if(PlrChar->MoveSpawnProjectile[MoveSelected] == false)
			app_timer_register(ANIMFRAMETIME * PlrChar->MoveHitFrame[MoveSelected], MoveLanded, NULL);
		else
			app_timer_register(ANIMFRAMETIME * PlrChar->MoveHitFrame[MoveSelected], SpawnProjectile, NULL);*/
	}
	else
  {
		PlayFightAnimation(&Plr2MoveHandle, Player2XPos, Player2YPos, OppChar);
		/*StopAnimation(Plr2MoveHandle);
		OppChar->Moves[MoveSelected].EndCallback = MoveOver;
		Plr2MoveHandle = AddAnimation(&(OppChar->Moves[MoveSelected]), Player2XPos, Player2YPos, true);

		if(OppChar->MoveSpawnProjectile[MoveSelected] == false)
			app_timer_register(ANIMFRAMETIME * OppChar->MoveHitFrame[MoveSelected], MoveLanded, NULL);
		else
			app_timer_register(ANIMFRAMETIME * OppChar->MoveHitFrame[MoveSelected], SpawnProjectile, NULL);*/
	}
}

//Wipe//////////////////////////////////////////////////////////////////
void BitmapDestroy(GBitmap **Bit)
{
	if(Bit != NULL)
	{
		gbitmap_destroy(*Bit);
		*Bit = NULL;
	}
}
void LayerDestroy(Layer **Lay)
{
	if(Lay != NULL)
	{
		layer_destroy(*Lay);
		*Lay = NULL;
	}
}
void BLayerDestroy(BitmapLayer **Lay)
{
	if(Lay != NULL)
	{
		bitmap_layer_destroy(*Lay);
		*Lay = NULL;
	}
}

void ClearWinLoss()
{
	if(WinLossLayer != NULL)
	{
		gbitmap_destroy(WinLossBitmap);
		WinLossBitmap = NULL;
		bitmap_layer_destroy(WinLossLayer);
		WinLossLayer = NULL;
	}
}

void DeleteBitmaps()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "UnloadBmp");
	ClearWinLoss();
	
	//Also non persistant layers
	if(StageLayer != NULL)
	{
	 	bitmap_layer_set_bitmap(StageLayer, NULL);
	}	
	if(StageLayerB != NULL)
	{
	 	bitmap_layer_set_bitmap(StageLayerB, NULL);
	}	
	BLayerDestroy(&WinLossLayer);
	LayerDestroy(&StarburstLayer);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "UnloadBmp2");

	//text
	if(bFontLoaded)
	{
		bFontLoaded = false;
		fonts_unload_custom_font(MainFont28);
		text_layer_destroy(RoundTextLayer);
		text_layer_destroy(RoundTextDropLayer);
	}
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "UnloadBmp3");
	
	//bitmaps
	BitmapDestroy(&StageBitmap);
	BitmapDestroy(&StageBitmapB);
	BitmapDestroy(&HealthBitmap);
	BitmapDestroy(&Health2Bitmap);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "UnloadBmp4");
	BitmapDestroy(&RoundWinBitmap);
	BitmapDestroy(&RoundBackBitmap);
	BitmapDestroy(&WinLossBitmap);
	BitmapDestroy(&LargeShadowBitmap);
	BitmapDestroy(&SmallShadowBitmap);
	BitmapDestroy(&StarburstBitmap);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "UnloadBmp5");
}

void DeleteLayers()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "UnloadLayer");

	BLayerDestroy(&StageLayer);
	BLayerDestroy(&StageLayerB);
	LayerDestroy(&RoundsLayer);
	LayerDestroy(&HealthLayer);
	LayerDestroy(&ShadowLayer);
}

void SetTempLayersVisible(bool bVis)
{
	layer_set_hidden(RoundsLayer, bVis);
	layer_set_hidden(HealthLayer, bVis);
	layer_set_hidden(ShadowLayer, bVis);
}

void StopAllAnimations()
{
	StopAnimation(Plr1MoveHandle);
	Plr1MoveHandle = -1;
	StopAnimation(Plr2MoveHandle);
	Plr2MoveHandle = -1;
	StopAnimation(ProjectileHandle);
	ProjectileHandle = -1;
}

static void DrawStarburst(Layer *layer, GContext *ctx) 
{		
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	if(StarburstTimer == 2)
	{
		PackBitmap(RESOURCE_ID_IMAGE_STARBURST2, StarburstBitmap);
	}
	else if(StarburstTimer == 4)
	{
		PackBitmap(RESOURCE_ID_IMAGE_STARBURST3, StarburstBitmap);
	}
	else if(StarburstTimer > 5)
		return;
	
	graphics_draw_bitmap_in_rect(ctx, StarburstBitmap, GRect(StarX, StarY, 20, 24));	
	
	StarburstTimer++;
}

void TurnOffLightDelayed()
{
	light_enable(false);
}

void WipeMiddle()
{
	if(CurrentFightState == FS_WaitForWipe)
	{
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Load data");
		
		//unload planes
		UnloadPlaneWindow();
		
		//unload between fight page
		UnloadBetweenWindow();
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "Memory after Clear %d %d", heap_bytes_free(), heap_bytes_used());
		
		//Set start positions
		Player1XPos = PLR1STARTX;
		Player1YPos = PLRSTARTY;
		Player2XPos = PLR2STARTX;
		Player2YPos = PLRSTARTY;

		//Load stage
		StageBitmap = gbitmap_create_blank(GSize(144, 59), GBitmapFormat4BitPalette);
		StageBitmapB = gbitmap_create_blank(GSize(144, 59), GBitmapFormat4BitPalette);

		PackBitmap(Stages[GetOpponentCharacter()], StageBitmap);
	 	bitmap_layer_set_bitmap(StageLayer, StageBitmap);
		layer_set_hidden(bitmap_layer_get_layer(StageLayer), false);
		
		PackBitmap(StagesBottom[GetOpponentCharacter()], StageBitmapB);
	 	bitmap_layer_set_bitmap(StageLayerB, StageBitmapB);
		layer_set_hidden(bitmap_layer_get_layer(StageLayerB), false);
	
		//Load round icons
		RoundWinBitmap = gbitmap_create_blank(GSize(7, 7), GBitmapFormat4BitPalette);
		RoundBackBitmap = gbitmap_create_blank(GSize(7, 7), GBitmapFormat2BitPalette);
	  PackBitmap(RESOURCE_ID_IMAGE_ROUNDWIN, RoundWinBitmap);
		PackBitmap(RESOURCE_ID_IMAGE_ROUNDBACK, RoundBackBitmap);
	  //RoundWinBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ROUNDWIN);
	  //RoundBackBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ROUNDBACK);
		//layer_mark_dirty(RoundsLayer);
				
		//Load HealthBars
	 // HealthBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEALTHBAR1);
	 //Health2Bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEALTHBAR2);
		HealthBitmap = gbitmap_create_blank(GSize(63, 14), GBitmapFormat2BitPalette);
		Health2Bitmap = gbitmap_create_blank(GSize(63, 14), GBitmapFormat2BitPalette);
		PackBitmap(RESOURCE_ID_IMAGE_HEALTHBAR1, HealthBitmap);
		PackBitmap(RESOURCE_ID_IMAGE_HEALTHBAR2, Health2Bitmap);
		//layer_mark_dirty(HealthLayer);
		
		//Load shadows 
	  //LargeShadowBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SHADOWLARGE);
	  //SmallShadowBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SHADOWSMALL);
		LargeShadowBitmap = gbitmap_create_blank(GSize(56, 10), GBitmapFormat2BitPalette);
		SmallShadowBitmap = gbitmap_create_blank(GSize(30, 10), GBitmapFormat2BitPalette);
		PackBitmap(RESOURCE_ID_IMAGE_SHADOWLARGE, LargeShadowBitmap);
		PackBitmap(RESOURCE_ID_IMAGE_SHADOWSMALL, SmallShadowBitmap);
		//layer_mark_dirty(ShadowLayer);
		
		SetTempLayersVisible(false);
		
		//Load starburst
		StarburstLayer = layer_create(GRect(0, 0, 144, 168));
 		layer_add_child(GetAnimLayer(), StarburstLayer);  		
		layer_set_update_proc(StarburstLayer, DrawStarburst);
		StarburstBitmap = gbitmap_create_blank(GSize(20, 24), GBitmapFormat4BitPalette);
		PackBitmap(RESOURCE_ID_IMAGE_STARBURST1, StarburstBitmap);
	
		//Load Characters
		LoadNeededCharacter(GetCharacter());
		LoadNeededCharacter(GetOpponentCharacter());
		PlrChar = (CharacterAnims[GetCharacter()]);
		OppChar = (CharacterAnims[GetOpponentCharacter()]);
	
		//put plrs in correct start spot
		Player1XPos -= (PlrChar->ReadyPose.SizeX / 2);
		Player2XPos += (OppChar->ReadyPose.SizeX / 2);
	
		//Set Ready poses, do winning player second
		if(bPlayerWon == false)
			Plr1MoveHandle = AddAnimation(&(PlrChar->ReadyPose), Player1XPos, Player1YPos, false);
		Plr2MoveHandle = AddAnimation(&(OppChar->ReadyPose), Player2XPos, Player2YPos, true);
		if(bPlayerWon)
			Plr1MoveHandle = AddAnimation(&(PlrChar->ReadyPose), Player1XPos, Player1YPos, false);
		
		//Request layer/bitmap removal (delayed as this callback is within the gfx render)
		//bUpdateBackground=true;
		
		//APP_LOG(APP_LOG_LEVEL_INFO, "Memory Match Ready %d %d", heap_bytes_free(), heap_bytes_used());
 	}
	else if(CurrentFightState == FS_WaitForWipeOut || CurrentFightState == FS_WaitForWipeOutTourneyWon)
	{
		//Unload all characters
		StopAllAnimations();
		UnloadNeededCharacter(GetCharacter());
		UnloadNeededCharacter(GetOpponentCharacter());
				
		/*DeleteBitmaps();
		APP_LOG(APP_LOG_LEVEL_INFO, "Bitmaps removed");
		CurrentFightState = FS_End;
		return;*/

		DeleteBitmaps();

		//Update match result
		CommitResult(bPlayerWon);

		SetTempLayersVisible(true);

		if(CurrentFightState != FS_WaitForWipeOutTourneyWon)
		{
			layer_set_hidden(bitmap_layer_get_layer(StageLayer), true);
			layer_set_hidden(bitmap_layer_get_layer(StageLayerB), true);

			//APP_LOG(APP_LOG_LEVEL_INFO, "Memory after FightOver %d %d", heap_bytes_free(), heap_bytes_used());

			//load planes
			LoadPlaneWindow();

			//give control of back light back to sdk
			app_timer_register(3000, TurnOffLightDelayed, NULL);
			//light_enable(false);

			//Load between fight page
			LoadBetweenWindow();
		}
		else
		{
			//Load Image
			int ResourceId1 = RESOURCE_ID_IMAGE_TOURNEYWON2;
			int ResourceId2 = RESOURCE_ID_IMAGE_TOURNEYWON2b;			
			if((rand() % 100) > 50) //never %2 as some rands can go odd/even/odd etc
			{
				ResourceId1 = RESOURCE_ID_IMAGE_TOURNEYWON1;
				ResourceId2 = RESOURCE_ID_IMAGE_TOURNEYWON1b;
			}

			StageBitmap = gbitmap_create_with_resource(ResourceId1);	
			StageBitmapB = gbitmap_create_with_resource(ResourceId2);	

			bitmap_layer_set_bitmap(StageLayer, StageBitmap);
			layer_set_hidden(bitmap_layer_get_layer(StageLayer), false);
			bitmap_layer_set_bitmap(StageLayerB, StageBitmapB);
			layer_set_hidden(bitmap_layer_get_layer(StageLayerB), false);
		}
	}
	else if(CurrentFightState == FS_WaitForWipeOutAfterTourneyWon)
	{
		DeleteBitmaps();

		//APP_LOG(APP_LOG_LEVEL_INFO, "Memory after Tourney won %d %d", heap_bytes_free(), heap_bytes_used());

		//load planes
		LoadPlaneWindow();

		//give control of back light back to sdk
		app_timer_register(1000, TurnOffLightDelayed, NULL);
		//light_enable(false);

		//Load between fight page
		LoadBetweenWindow();
	}
}

void WipeEnd()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Wipeend %d" , CurrentFightState);
	
	if(CurrentFightState == FS_WaitForWipe)
	{
		CurrentFightState = FS_WaitForStart;
		StateTimer = 0;
	}
	else if(CurrentFightState == FS_WaitForWipeOut)
	{
		SaveSettings();
		
		CurrentFightState = FS_End;
	}
	else if(CurrentFightState == FS_WaitForWipeOutTourneyWon)
	{
		SaveSettings();
		
		CurrentFightState = FS_TourneyWon;
	}
	else if(CurrentFightState == FS_WaitForWipeOutAfterTourneyWon)
	{
		CurrentFightState = FS_End;
	}
}
//HUD//////////////////////////////////////////////////////////////////
static void DrawShadow(Layer *layer, GContext *ctx, enum Characters CharToDraw, int XPos, int XOff) 
{	
 	if(CharToDraw != C_Honda && CharToDraw != C_Zangief)
		graphics_draw_bitmap_in_rect(ctx, SmallShadowBitmap, GRect((XPos + XOff) - 15, 0, 30, 10));						
	else
		graphics_draw_bitmap_in_rect(ctx, LargeShadowBitmap, GRect((XPos + XOff) - 28, 0, 56, 10));						
}

static void DrawShadows(Layer *layer, GContext *ctx) 
{		
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
	
	int ProjX, ProjY;
	
	GetCurrentAnimationPos(Plr1MoveHandle, &ProjX, &ProjY);
	DrawShadow(layer, ctx, GetCharacter(), ProjX, PlrChar->ReadyPose.SizeX / 2.0);
	GetCurrentAnimationPos(Plr2MoveHandle, &ProjX, &ProjY);
	DrawShadow(layer, ctx, GetOpponentCharacter(), ProjX, -(OppChar->ReadyPose.SizeX / 2.0));
}

static void DrawRounds(Layer *layer, GContext *ctx) 
{
	int CurrentOffset = 0;
			
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
	
	for(int index = 0; index < Rounds; ++index)
	{
		if(RWins > index)
		{
			graphics_draw_bitmap_in_rect(ctx, RoundWinBitmap, GRect(((50 - ROUNDICONCENTEROFFSET) - 7) - CurrentOffset, 0, 7, 7));			
		}
		
		if(RLosses > index)
		{
			graphics_draw_bitmap_in_rect(ctx, RoundWinBitmap, GRect(50 + ROUNDICONCENTEROFFSET + CurrentOffset, 0, 7, 7));						
		}
		
		graphics_draw_bitmap_in_rect(ctx, RoundBackBitmap, GRect(((50 - ROUNDICONCENTEROFFSET) - 7) - CurrentOffset, 0, 7, 7));						
		graphics_draw_bitmap_in_rect(ctx, RoundBackBitmap, GRect(50 + ROUNDICONCENTEROFFSET + CurrentOffset, 0, 7, 7));					
		
		CurrentOffset += 7; //Round icon width
	}
}

static void DrawHealthBar(Layer *layer, GContext *ctx) 
{
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
	
	graphics_draw_bitmap_in_rect(ctx, HealthBitmap, GRect(((72 - ROUNDICONCENTEROFFSET) - 63), 0, 63, 13));			
	graphics_draw_bitmap_in_rect(ctx, Health2Bitmap, GRect((72 + ROUNDICONCENTEROFFSET), 0, 63, 13));			

	graphics_context_set_fill_color(ctx, GColorFromRGB(255,255,0));
	
	struct GRect WinnerHealthBarRect; 
	struct GRect LoserHealthBarRect1; 
	struct GRect LoserHealthBarRect2; 
	if(bPlayerWon)
	{
		WinnerHealthBarRect = GRect(((72 - ROUNDICONCENTEROFFSET) - HEALTHSIZEX) + 2, 6, HEALTHSIZEX - 4, 4);
		if(LoserHealth > 0)
			LoserHealthBarRect1 = GRect((72 + ROUNDICONCENTEROFFSET) + 2, 6, LoserHealth, 4);
		LoserHealthBarRect2 = GRect((72 + ROUNDICONCENTEROFFSET) + 2 + LoserHealth, 6, (HEALTHSIZEX - 4) - LoserHealth, 4);
	}	
	else
	{
		WinnerHealthBarRect = GRect((72 + ROUNDICONCENTEROFFSET) + 2, 6, HEALTHSIZEX - 4, 4);

		if(LoserHealth > 0)
			LoserHealthBarRect1 = GRect(((72 - ROUNDICONCENTEROFFSET) - (HEALTHSIZEX - 2)) + ((HEALTHSIZEX - 4) - LoserHealth), 6, LoserHealth, 4);
		LoserHealthBarRect2 = GRect((72 - ROUNDICONCENTEROFFSET) - (HEALTHSIZEX - 2), 6, (HEALTHSIZEX - 4) - LoserHealth, 4);
	}	
	
	graphics_fill_rect(ctx, WinnerHealthBarRect, 0, GCornerNone);
	if(LoserHealth > 0)
		graphics_fill_rect(ctx, LoserHealthBarRect1, 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorFromRGB(255,0,0));
	graphics_fill_rect(ctx, LoserHealthBarRect2, 0, GCornerNone);

	if(LoserHealth < HEALTHSIZEX - 4 && LoserHealth > 0)
	{
		//watch for unsigned wrap
		if(LoserHealth > 1)
			LoserHealth -= 2;
		else
			LoserHealth = 0;
	}
}

void InitRoundTextStyle(TextLayer *TextL, GColor8 col)
{
	Layer *AnimLayer = GetAnimLayer();
	
	text_layer_set_background_color(TextL, GColorClear);
  text_layer_set_text_color(TextL, col);
  text_layer_set_font(TextL, MainFont28);
  text_layer_set_text_alignment(TextL, GTextAlignmentLeft);
  layer_add_child(AnimLayer, text_layer_get_layer(TextL));
}

void DoText(TextLayer **Lay, int x, int y, int w, int h, GColor8 col)
{
	*Lay = text_layer_create(GRect(x, y, w, h));
	InitRoundTextStyle(*Lay, col);
	text_layer_set_text(*Lay, TexBuffer);
}

void DisplayRoundNo()
{
	uint8_t RWins, RLosses, Rounds;
	GetRoundData(&RWins, &RLosses, &Rounds);
	
	bFontLoaded = true;

	MainFont28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_28));

	// Create dropped TextLayer
	snprintf(TexBuffer, sizeof(TexBuffer), "ROUND %d", RWins + RLosses + 1);
	DoText(&RoundTextDropLayer, 38, 82, 144-38, 30, GColorBlack);
	// Create TextLayer
	DoText(&RoundTextLayer, 40, 80, 144-40, 30, GColorWhite);
}

void DisplayFight()
{
	text_layer_destroy(RoundTextLayer);
	text_layer_destroy(RoundTextDropLayer);

	// Create dropped TextLayer
	snprintf(TexBuffer, sizeof(TexBuffer), "FIGHT");
	DoText(&RoundTextDropLayer, 50, 82, 144-50, 30, GColorBlack);
	// Create TextLayer
 	DoText(&RoundTextLayer, 52, 80, 144-52, 30, GColorWhite);
}

void ClearFight()
{
	bFontLoaded = false;
	fonts_unload_custom_font(MainFont28);
	text_layer_destroy(RoundTextLayer);
	text_layer_destroy(RoundTextDropLayer);
}

void DisplayWinLoss()
{
	Layer *AnimLayer = GetAnimLayer();
	
	WinLossLayer = bitmap_layer_create(GRect(0, 70, 144, 88));
 	layer_add_child(AnimLayer, bitmap_layer_get_layer(WinLossLayer));  		
	bitmap_layer_set_compositing_mode(WinLossLayer, GCompOpSet);

	if(bPlayerWon)
		WinLossBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YOUWIN);
	else
		WinLossBitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_YOULOSE);
		
  bitmap_layer_set_bitmap(WinLossLayer, WinLossBitmap);
}

//Main start/loop/end//////////////////////////////////////////////////////////////////
void UpdateFight()
{
	StateTimer++;
	
	switch(CurrentFightState)
	{
		case FS_WaitForStart:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_WaitForStart %d", StateTimer);
			if(StateTimer > SUSPENSETIME) 
			{
				StateTimer = 0;
				DisplayRoundNo();
				CurrentFightState = FS_Round;
			}
		} break;
		
		case FS_Round:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_Round %d", StateTimer);
			if(StateTimer > ROUNDTIME) 
			{
				StateTimer = 0;
				CurrentFightState = FS_Fight;
				DisplayFight();
			}
			
			layer_mark_dirty(ShadowLayer);
		} break;
		
		case FS_Fight:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_Fight %d", StateTimer);
			if(StateTimer > FIGHTTIME) 
			{
				StateTimer = 0;
				CurrentFightState = FS_WaitForMove;
				ClearFight();
			}

			layer_mark_dirty(ShadowLayer);
		} break;
		
		case FS_WaitForMove:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_WaitForMove %d", StateTimer);
			if(StateTimer > WAITFORMOVETIME) 
			{
				StateTimer = 0;
				CurrentFightState = FS_Animating;
				StartFightAnimations();
			}

			layer_mark_dirty(ShadowLayer);
		} break;
		
		case FS_KODelay:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_KODelay %d", StateTimer);
			if(StateTimer > KOWAITTIME) 
			{
				StateTimer = 0;
				CurrentFightState = FS_WinPose;
				
				//Soft Update match result
				if(bPlayerWon)
					RWins++;
				else
					RLosses++;					
				
				//Update hud
				layer_mark_dirty(RoundsLayer);
				layer_mark_dirty(HealthLayer);
				layer_mark_dirty(ShadowLayer);
				
				//Play win pose
				PlayWinPose();
			}			
		} break;
		
		case FS_WinPose:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_WinPose %d", StateTimer);
			if(StateTimer > WINPOSEWAITTIME) 
			{
				StateTimer = 0;
				
				if(FightOverAfterThisRound(bPlayerWon))
				{
					CurrentFightState = FS_WinLoss;
					DisplayWinLoss();
				}
				else
				{
					CurrentFightState = FS_WaitForWipeOut;
					PerformWipe(WipeMiddle, WipeEnd);
				}					
			}			

			layer_mark_dirty(ShadowLayer);
		} break;
		
		case FS_WinLoss:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_Round %d", StateTimer);
			if(StateTimer > WINLOSSTIME) 
			{
				StateTimer = 0;
				if(bPlayerWon && FightOverAfterThisRound(bPlayerWon) && IsLastOpponent())
					CurrentFightState = FS_WaitForWipeOutTourneyWon;
				else	
					CurrentFightState = FS_WaitForWipeOut;
				ClearWinLoss();
				PerformWipe(WipeMiddle, WipeEnd);
			}

			layer_mark_dirty(ShadowLayer);
		} break;
		
		case FS_TourneyWon:
		{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "FS_TourneyWon %d", StateTimer);
			if(StateTimer > TOURNEYWONTIME) 
			{
				CurrentFightState = FS_WaitForWipeOutAfterTourneyWon;
				PerformWipe(WipeMiddle, WipeEnd);
			}		
		} break;
		
		/*case FS_WaitForWipeOut:
		{
		} break;
		
		case FS_WaitForWipeOutTourneyWon:
		{
		} break;
		
		case FS_WaitForWipeOutAfterTourneyWon:
		{
		} break;

		case FS_WaitForWipe:
		{
		} break;
		
		case FS_End:
		{
		} break;*/
		
		default:
		break;
	}
	
	app_timer_register(67, UpdateFight, NULL); //15fps	
}

void BeginFight()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "BeginFight");
	
	CurrentFightState = FS_WaitForWipe;
	
	DoesPlayerWin();
	
	app_timer_register(67, UpdateFight, NULL); //15fps
	
	ProjectileHandle = Plr2MoveHandle = Plr1MoveHandle = -1;
	
	LoserHealth = HEALTHSIZEX - 4;
	
	StarburstTimer = 3; //start ended
	
	bMoveOver = false;
	bHitOver = false;
	
	if(AlertFight())
	{
		//Turn on back light
		light_enable(true);
	
		//Vibrate to alert user
		vibes_short_pulse();
	}
	
	GetRoundData(&RWins, &RLosses, &Rounds);
	
	//Start a screen wipe
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Fight Wipe start");
	PerformWipe(WipeMiddle, WipeEnd);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Fight Wipe called");
	//WipeMiddle();
	//WipeEnd();
}

void QuickEndFight()
{
	if(CurrentFightState != FS_End)
	{
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Quick Fight End");
		
		//give control of back light back to sdk
		light_enable(false);

		CurrentFightState = FS_End;
		
		ClearWipe();
		
		UnloadNeededCharacter(GetCharacter());
		UnloadNeededCharacter(GetOpponentCharacter());
		
		StopAllAnimations();

		// Destroy GBitmaps
		DeleteBitmaps();
		
		layer_set_hidden(bitmap_layer_get_layer(StageLayer), true);
		layer_set_hidden(bitmap_layer_get_layer(StageLayerB), true);
		SetTempLayersVisible(true);

		//Load plane
		LoadPlaneWindow();
				
		//Load stats
		LoadBetweenWindow();
	}
}

//Init
void LoadMatchEngine(Window *ParentWindow)
{
	CurrentFightState = FS_End;
		
	StageBitmap = NULL;
	StageBitmapB = NULL;
	RoundWinBitmap = NULL;
	RoundBackBitmap = NULL;
	HealthBitmap = NULL;
	StarburstBitmap = NULL;
	
	WinLossBitmap = NULL;
	WinLossLayer = NULL;
	
	ShadowLayer = NULL;
	LargeShadowBitmap = NULL;
	SmallShadowBitmap = NULL;
	StarburstLayer = NULL;
	
	bFontLoaded = false;
	
	struct Layer *RootLayer = window_get_root_layer(ParentWindow);

	StageLayer = bitmap_layer_create(GRect(0, 50, 144, 59));
 	layer_add_child(RootLayer, bitmap_layer_get_layer(StageLayer));  		
	layer_set_hidden(bitmap_layer_get_layer(StageLayer), true);
	StageLayerB = bitmap_layer_create(GRect(0, 109, 144, 59));
 	layer_add_child(RootLayer, bitmap_layer_get_layer(StageLayerB));  		
	layer_set_hidden(bitmap_layer_get_layer(StageLayerB), true);

	RoundsLayer = layer_create(GRect(22, ROUNDICONY, 100, 7));
 	layer_add_child(RootLayer, RoundsLayer);  		
	layer_set_update_proc(RoundsLayer, DrawRounds);

	HealthLayer = layer_create(GRect(0, HEALTHY, 144, HEALTHSIZEY));
 	layer_add_child(RootLayer, HealthLayer);  		
	layer_set_update_proc(HealthLayer, DrawHealthBar);

	ShadowLayer = layer_create(GRect(0, SHADOWSTARTY, 144, 10));
 	layer_add_child(RootLayer, ShadowLayer);  		
	layer_set_update_proc(ShadowLayer, DrawShadows);
	
	SetTempLayersVisible(true);

	bUpdateBackground = false;
}

void UnloadMatchEngine()
{	
	UnloadNeededCharacter(GetCharacter());
	UnloadNeededCharacter(GetOpponentCharacter());
	DeleteLayers();
	DeleteBitmaps();
}