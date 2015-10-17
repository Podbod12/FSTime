#include <pebble.h>
#include "main.h"
#include "Settings.h"
#include "Message.h"
#include "Characters.h"

#define STEPHISTORYMAX 28
	
enum FightTime FTime = FT_Hourly;
uint8_t DailyTime;

uint8_t HourlySleepTimeStart;
uint8_t HourlySleepTimeEnd;

//stats
int16_t Wins;
int16_t Loses;

int16_t WinsThisTourney;

int16_t TourneyWins;
int16_t CurrentTourney;

int LastStepsNotCommited;
int LastSteps;
int NextStepTotal;
int LastStepTime;
int LastFightTimeH;
int LastFightTimeD;

int StepHistory[STEPHISTORYMAX];

enum Characters CurrentCharacter;
enum Characters OpponentCharacter;
uint8_t RoundsWon;
uint8_t RoundsLost;

int16_t CharactersDefeated;
uint8_t DefeatedOrder[C_MAX];

bool bUseAmericanDate;
bool bAlertFight;

//forward dec
bool InSleepTime(int CurrentHour);

//Steps/////////////////////////////////////////////
void AddStep()
{
  time_t Temp = time(NULL); 
  struct tm *TickTime = localtime(&Temp);
	if(InSleepTime(TickTime->tm_hour + 1) == false) //Add one as sleep time was written for checking fights on the hour
	{
		if(FTime == FT_Hourly)
		{
			if((TickTime->tm_hour > LastStepTime) || (LastStepTime == 23 && TickTime->tm_hour != 0))
			{
				LastStepsNotCommited = 0;
				LastSteps = 0;
			}
			
			LastStepTime = TickTime->tm_hour;
		}
		else //daily
		{
			if((TickTime->tm_yday > (LastStepTime + 1)) || (LastStepTime == 365 && TickTime->tm_yday > 2))
			{
				LastStepsNotCommited = 0;
				LastSteps = 0;
			}
			
			LastStepTime = TickTime->tm_yday;
		}
		
		LastStepsNotCommited++;
		LastSteps++;
	}
}

int GetStepHistory(int index)
{
	return StepHistory[index];
}

int GetMaxStepHistory()
{
	return STEPHISTORYMAX;
}

void CommitStepHistory()
{
	for(int index = 0; index < (STEPHISTORYMAX - 1); ++index)
	{
		StepHistory[index] = StepHistory[index + 1];
	}
	
	StepHistory[STEPHISTORYMAX - 1] = LastStepsNotCommited;
	LastStepsNotCommited = 0;
	SaveSettings();
}

bool HasDoneEnoughStepsToWin()
{
#if FORCEWINNER == 1
	return true;
#endif
#if FORCEWINNER == 2
	return false;
#endif
#ifndef FORCEWINNER
	int HourlyMod = 3; //these valuses are less than the exact division as the theory is your daily walk is in bursts
	//float EightMod = 0.4;   //so you'd not make the total as often. See also the rounds no as that goes up to compensate as well
	int DailyMod = 100;
	
	int CompetitionMod = 200; //multiplier for each comptition (not compound)
	int CharacterMod = 5; //multiplier for each character (not compound)
	
	int BaseSteps = 1500; //nice and easy
	
	int TimeMod = 100;

  time_t Temp = time(NULL); 
  struct tm *TickTime = localtime(&Temp);

	switch(FTime)
  {
    case FT_Hourly:
    {
      TimeMod = HourlyMod;
    } break;
    
    //case FT_Eight:
    //{
    //  TimeMod = EightMod;
    //} break;
    
    case FT_Daily:
    {
      TimeMod = DailyMod;
    } break;
    
   default:
   {
   } break;
  }
	
	LastFightTimeH = TickTime->tm_hour;
	LastFightTimeD = TickTime->tm_yday;
	
	int StepsTemp = LastSteps;
	LastSteps = 0;
	int StepsNeeded = ((BaseSteps * ((CharacterMod * WinsThisTourney) + (CompetitionMod * CurrentTourney) + (TimeMod)))) / 100;
	bool returnVal = StepsTemp > StepsNeeded;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps check - StepsNeeded=%d, returnVal=%d, stepsTemp=%d", StepsNeeded, returnVal, StepsTemp);
	return returnVal;
	//return StepsTemp > ((BaseSteps * ((CharacterMod * WinsThisTourney) + (CompetitionMod * CurrentTourney) + (TimeMod))) / 100);
#endif
}

//Tourney/////////////////////////////////////////////
void PickNextOpponent()
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "PickNextOpponent WinsThisTourney=%d, Wins=%d, CharactersDefeated=%d", WinsThisTourney, Wins, CharactersDefeated);
	DefeatedOrder[WinsThisTourney] = OpponentCharacter;
	CharactersDefeated = CharactersDefeated | (1 << OpponentCharacter);
	WinsThisTourney++;
	Wins++;
	
	if(CharactersDefeated == 255)
	{
		//Tourney won
		CharactersDefeated = 0;
		WinsThisTourney = 0;
		CurrentTourney++;
		if(CurrentTourney > TourneyWins)
			TourneyWins = CurrentTourney;
	}
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "PickNextOpponent2, CharactersDefeated = %d -- %d, %d, %d", CharactersDefeated, OpponentCharacter, (1 << 0), (1 << 1));
	enum Characters NewOpponent;
	do
	{
		NewOpponent = rand() % C_MAX;
	} while ((CharactersDefeated & (1 << NewOpponent)) > 0);
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "PickNextOpponent3");

	OpponentCharacter = NewOpponent;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "OpponentCharacter4=%d", OpponentCharacter);
	
	RoundsLost = 0;
	RoundsWon = 0;
}

void FightLost()
{
	if(CharactersDefeated == 0)
	{
		//Drop down a tournament if you lost in the first fight
		CurrentTourney--;
		if(CurrentTourney < 0)
			CurrentTourney = 0;
	}	
	
	CharactersDefeated = 0;
	WinsThisTourney = 0;
	Loses++;
	
	OpponentCharacter = rand() % C_MAX;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "OpponentCharacter3=%d", OpponentCharacter);
	
	RoundsLost = 0;
	RoundsWon = 0;
}

uint8_t RoundsToWin()
{
  switch(FTime)
  {
    case FT_Hourly:
    {
      return 3;
    } break;
    
  /*  case FT_Eight:
    {
			return 2;
    } break;*/
    
    case FT_Daily:
    {
      return 1;
    } break;
    
   default:
   {
   } break;
  }
  
  return 1;
}

void GetRoundData(uint8_t *RWins, uint8_t *RLosses, uint8_t *Rounds)
{
	*RWins = RoundsWon;
	*RLosses = RoundsLost;
	*Rounds = RoundsToWin();
}

inline bool FightOverAfterThisRound(bool bPlayerWon)
{
	if(bPlayerWon)
	{
		if((RoundsWon + 1) >= RoundsToWin())
			return true;
	}
	else
	{
		if((RoundsLost + 1) >= RoundsToWin())
			return true;
	}
	
	return false;
}

inline bool IsLastOpponent()
{
	return (CharactersDefeated | (1 << OpponentCharacter)) == 255;
}

inline void CommitResult(bool bPlayerWon)
{
	if(bPlayerWon)
	{
		RoundsWon++;
		if(RoundsWon >= RoundsToWin())
			PickNextOpponent();
	}
	else
	{
		RoundsLost++;
		if(RoundsLost >= RoundsToWin())
			FightLost();
	}
}

int GetTourneysWon()
{
	return TourneyWins;
}

int GetCurrentTournament()
{
	return CurrentTourney;
}

void GetWinLoss(int16_t *FWins, int16_t *FLosses)
{
	*FWins = Wins;
	*FLosses = Loses;
}

inline bool AlertFight()
{
	return bAlertFight;
}

inline void SetAlertFight(bool bNewVal)
{
	bAlertFight = bNewVal;
}

//Chars/////////////////////////////////////////////
void SetCharacter(enum Characters NewCharacter)
{
  CurrentCharacter = NewCharacter;
}

enum Characters GetCharacter()
{
#ifdef FORCECHARACTER
	return FORCECHARACTER;
#endif
	
  return CurrentCharacter;
}

enum Characters GetOpponentCharacter()
{
#ifdef FORCEOPPONENT
	return FORCEOPPONENT;
#endif
	
	return OpponentCharacter;
}

enum Characters GetFightHistory(int index)
{
	if(index < WinsThisTourney)
		return DefeatedOrder[index];
	else if(index == WinsThisTourney)
		return GetOpponentCharacter();
	
	return C_MAX;
}

//Times/////////////////////////////////////////////
void SetSleepStart(int newTime)
{
	persist_write_int(MK_SETTINGS_SLEEPSTART, newTime);
	HourlySleepTimeStart = newTime;
}

void SetSleepEnd(int newTime)
{
	persist_write_int(MK_SETTINGS_SLEEPEND, newTime);
	HourlySleepTimeEnd = newTime;
}

void SetFightTime(enum FightTime NewTime)
{
  FTime = NewTime;
}

enum FightTime GetFightTime()
{
  return FTime;
}

bool InSleepTime(int CurrentHour)
{
	if(FTime == FT_Hourly && HourlySleepTimeStart != HourlySleepTimeEnd)
	{
		if(HourlySleepTimeStart < HourlySleepTimeEnd && (CurrentHour > HourlySleepTimeStart && CurrentHour <= HourlySleepTimeEnd)) //during day
		{
			return true;
		}
		if(HourlySleepTimeStart > HourlySleepTimeEnd && (CurrentHour > HourlySleepTimeStart || CurrentHour <= HourlySleepTimeEnd)) //across midnight
		{
			return true;
		}
	}
	
	return false;
}

int HourForFight(struct tm *CurrentTime, bool bFightCheck)
{
	int CurrentHour = CurrentTime->tm_hour;
  if(!bFightCheck)
		CurrentHour++;
	
  switch(FTime)
  {
    case FT_Hourly:
    {
			if(InSleepTime(CurrentHour))
			{
				CurrentHour = HourlySleepTimeEnd + 1;
				if(CurrentHour > 23)
					CurrentHour -= 24;
			}
		} break;
    
   /* case FT_Eight:
    {
			if(CurrentHour <= 8 && CurrentHour > 20)
        return 8;
			else
				return 20
    } break;*/
    
    case FT_Daily:
    {
        return DailyTime;
    } break;
    
    default:
    {
    } break;
  }	
	
  return CurrentHour;
}

bool IsTimeForFight(struct tm *CurrentTime)
{
	if(LastFightTimeH == CurrentTime->tm_hour && LastFightTimeD == CurrentTime->tm_yday)
		return false;

	if(CurrentTime->tm_hour == HourForFight(CurrentTime, true))
   return true;
	
  return false;
}

float GetTimeTillFightAsPerc(struct tm *CurrentTime)
{
	int FightHour = HourForFight(CurrentTime, false);
	
	int MinsToGo;
	if(CurrentTime->tm_hour < FightHour)
		MinsToGo = ((FightHour-1) - CurrentTime->tm_hour) * 60;
	else
		MinsToGo = ((23 + FightHour) - CurrentTime->tm_hour) * 60;

	MinsToGo += (60-CurrentTime->tm_min);
	
	int MinsToGoPerc = 0;
	switch(FTime)
  {
    case FT_Hourly:
    {
        MinsToGoPerc = ((float)MinsToGo / 60.0f) * 100.0f;
				if(MinsToGoPerc > 100)
					MinsToGoPerc = 100; //because of sleep timer
		} break;
    
   /* case FT_Eight:
    {
    } break;*/
    
    case FT_Daily:
    {
        MinsToGoPerc = ((float)MinsToGo / 1440.0f) * 100.0f;
    } break;
    
    default:
    {
    } break;
  }
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Mins to go update=%d, %d, %d", MinsToGo, FightHour, MinsToGoPerc);
	
	return MinsToGoPerc;
}

inline bool UseAmericanDateFormat()
{
	return bUseAmericanDate;
}

inline void SetUSDateFormat(bool bNewVal)
{
 	persist_write_bool(MK_SETTINGS_USDATEFORMAT, bNewVal);
	bUseAmericanDate = bNewVal;
}

//Load/Save/////////////////////////////////////////////
void LoadSettings()
{
	CurrentCharacter = persist_read_int(MK_SETTINGS_CHAR);
	FTime = persist_read_int(MK_SETTINGS_TIMETYPE);
	DailyTime = persist_read_int(MK_SETTINGS_TIMEEXACT);
	HourlySleepTimeStart = persist_read_int(MK_SETTINGS_SLEEPSTART);
	HourlySleepTimeEnd = persist_read_int(MK_SETTINGS_SLEEPEND);
	bUseAmericanDate = persist_read_bool(MK_SETTINGS_USDATEFORMAT);
	bAlertFight = persist_read_bool(MK_SETTINGS_FIGHTRUMBLEOFF) == false;

#ifndef FINALBUILD
	for(int index = 0; index < STEPHISTORYMAX; ++index)
	{
		StepHistory[index] = rand() % 3000; //testing
	}
#endif

	if(persist_read_bool(MK_SETTINGS_RESETSETTINGS) || persist_read_int(MK_SETTINGS_VER) == 0)
	{
		persist_write_bool(MK_SETTINGS_RESETSETTINGS, false);
		persist_write_int(MK_SETTINGS_WINS, 0);
		persist_write_int(MK_SETTINGS_LOSES, 0);
		persist_write_int(MK_SETTINGS_WINSTHISTOURNEY, 0);
		persist_write_int(MK_SETTINGS_TOURNEYWINS, 0);
		persist_write_int(MK_SETTINGS_CHARACTERSBEATEN, 0);
		persist_write_int(MK_SETTINGS_LASTSTEPS, 0);
		persist_write_int(MK_SETTINGS_ROUNDWINS, 0);
		persist_write_int(MK_SETTINGS_ROUNDLOSES, 0);
		persist_write_int(MK_SETTINGS_CURRENTTOURNEY, 0);
		persist_write_int(MK_SETTINGS_LASTSTEPTIME, -1);
		persist_write_int(MK_SETTINGS_LASTFIGHTTIMEH, -1);
		persist_write_int(MK_SETTINGS_LASTFIGHTTIMED, -1);

		OpponentCharacter = rand() % C_MAX;
		persist_write_int(MK_SETTINGS_OPPONENT, OpponentCharacter);
	}
	
	Wins = persist_read_int(MK_SETTINGS_WINS);
	Loses = persist_read_int(MK_SETTINGS_LOSES);
	WinsThisTourney = persist_read_int(MK_SETTINGS_WINSTHISTOURNEY);
	TourneyWins = persist_read_int(MK_SETTINGS_TOURNEYWINS);
	LastSteps = persist_read_int(MK_SETTINGS_LASTSTEPS);
	CharactersDefeated = persist_read_int(MK_SETTINGS_CHARACTERSBEATEN);
	RoundsWon = persist_read_int(MK_SETTINGS_ROUNDWINS);
	RoundsLost = persist_read_int(MK_SETTINGS_ROUNDLOSES);
	CurrentTourney = persist_read_int(MK_SETTINGS_CURRENTTOURNEY);
	LastStepTime = persist_read_int(MK_SETTINGS_LASTSTEPTIME);
	LastFightTimeH = persist_read_int(MK_SETTINGS_LASTFIGHTTIMEH);
	LastFightTimeD = persist_read_int(MK_SETTINGS_LASTFIGHTTIMED);

	OpponentCharacter = persist_read_int(MK_SETTINGS_OPPONENT);
	for(int HistoryIdx = 0; HistoryIdx < WinsThisTourney; ++HistoryIdx)
	{
		DefeatedOrder[HistoryIdx] = persist_read_int(MK_SETTINGS_TOURNEYHISTORY1 + HistoryIdx);
	}

	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Load Settings Wins=%d, Loses=%d, TourneyWins=%d, LastSteps=%d, CharactersDefeated=%d, CurrentChar=%d, FTime=%d, DailyTime=%d", Wins, Loses, TourneyWins, LastSteps, CharactersDefeated, CurrentCharacter, FTime, DailyTime);
}

void SaveSettings()
{
		persist_write_int(MK_SETTINGS_WINS, Wins);
		persist_write_int(MK_SETTINGS_LOSES, Loses);
		persist_write_int(MK_SETTINGS_WINSTHISTOURNEY, WinsThisTourney);
		persist_write_int(MK_SETTINGS_TOURNEYWINS, TourneyWins);
		persist_write_int(MK_SETTINGS_LASTSTEPS, LastSteps);
		persist_write_int(MK_SETTINGS_CHARACTERSBEATEN, CharactersDefeated);
		persist_write_int(MK_SETTINGS_ROUNDWINS, RoundsWon);
		persist_write_int(MK_SETTINGS_ROUNDLOSES, RoundsLost);
		persist_write_int(MK_SETTINGS_OPPONENT, OpponentCharacter);
		persist_write_int(MK_SETTINGS_CURRENTTOURNEY, CurrentTourney);
		persist_write_int(MK_SETTINGS_LASTSTEPTIME, LastStepTime);
		persist_write_int(MK_SETTINGS_LASTFIGHTTIMEH, LastFightTimeH);
		persist_write_int(MK_SETTINGS_LASTFIGHTTIMED, LastFightTimeD);

		for(int HistoryIdx = 0; HistoryIdx < WinsThisTourney; ++HistoryIdx)
		{
			persist_write_int(MK_SETTINGS_TOURNEYHISTORY1 + HistoryIdx, DefeatedOrder[HistoryIdx]);
		}
	
		persist_write_int(MK_SETTINGS_VER, 1);
}