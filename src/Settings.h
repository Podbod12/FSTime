#pragma once
	
#include "main.h"
#include "Characters.h"

enum FightTime
{
  FT_Hourly,
 // FT_Eight,
  FT_Daily,
	FT_MAX,
};

void CommitResult(bool bPlayerWon);
void GetRoundData(uint8_t *RWins, uint8_t *RLosses, uint8_t *Rounds);
bool HasDoneEnoughStepsToWin();

int GetTourneysWon();
int GetCurrentTournament();
void GetWinLoss(int16_t *FWins, int16_t *FLosses);
bool FightOverAfterThisRound(bool bPlayerWon);
bool IsLastOpponent();
bool AlertFight();
void SetAlertFight(bool bNewVal);

void SetCharacter(enum Characters NewCharacter);
enum Characters GetCharacter();
enum Characters GetOpponentCharacter();
enum Characters GetFightHistory(int index);
bool IsTimeForFight(struct tm *CurrentTime);
float GetTimeTillFightAsPerc(struct tm *CurrentTime);
void SetFightTime(enum FightTime NewTime);
bool UseAmericanDateFormat();
void SetUSDateFormat(bool bNewVal);
bool InSleepTime(int CurrentHour);
void SetSleepStart(int newTime);
void SetSleepEnd(int newTime);

void AddStep();
int GetStepHistory(int index);
int GetMaxStepHistory();
void CommitStepHistory();

void LoadSettings();
void SaveSettings();