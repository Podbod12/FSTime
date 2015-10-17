#include <pebble.h>
#include "main.h"
#include "AutoAnimationEngine.h"
#include "Ryu.h"
#include "Zangief.h"
#include "WipeEffect.h"
#include "Weather.h"
#include "Settings.h"
#include "Message.h"
#include "Time.h"
#include "Plane.h"
#include "MatchEngine.h"
#include "Step.h"
#include "BetweenFightPage.h"
#include "Battery.h"

//Main window
static Window *MainWindow;

static void MinuteTick(struct tm *tick_time, TimeUnits units_changed)
{
	// Update time every min
  UpdateTime();
	
	UpdatePlane(tick_time);
  
	// Get weather & Battery update every 30 minutes
  if(tick_time->tm_min % 30 == 0 || NoWeatherIconSet())
  {
		UpdateWeather();
		UpdateBattery();
	}
	
	//update steps bars
  UpdateStats();
	
	//Fight?
#ifdef FORCE5MINMATCH
	if(tick_time->tm_min % 5 == 0)
		BeginFight();
#else
	if(tick_time->tm_min == 0 && IsTimeForFight(tick_time))
		BeginFight();
#endif
}

#ifdef FORCEMATCH
static void TestThings()
{
  BeginFight();
}
#endif

//These are needed elsewhere
void LoadPlaneWindow()
{
	LoadPlane(MainWindow, false);
}
void UnloadPlaneWindow()
{
	UnloadPlane();
}
void LoadBetweenWindow()
{
	LoadBetweenFight(MainWindow, false);
}
void UnloadBetweenWindow()
{
	UnloadBetweenFight();
}

inline void InitCharacters()
{
	for(int index = 0; index < C_MAX; ++index)
	{
		CharacterAnims[index] = NULL;
	}
}

void RestartAfterSettingsChange()
{
	LoadSettings();
	
	if(IsInStatsPage())
	{
		UnloadBetweenWindow();
		LoadBetweenWindow();
		UpdateFlags();
	}
	
	QuickEndFight();
}

static void MainWindowLoad(Window *window)
{
#ifdef FORCEMATCH
 	app_timer_register(5000, TestThings, NULL);
#endif
	
	AllocateTime();
	AllocateWeather();
	AllocateWipe();
	AllocateBattery();
	
	LoadSettings();
	LoadMatchEngine(MainWindow);
  LoadAutoAnimation(MainWindow);
	LoadPlane(MainWindow, true);
	LoadBetweenFight(MainWindow, true);
  LoadScreenWipe(MainWindow);
	LoadTime(MainWindow);
  LoadWeather(MainWindow);
	LoadBattery(MainWindow);
	
	StartSteps();
	
	InitCharacters();
	
	//APP_LOG(APP_LOG_LEVEL_INFO, "Memory start %d %d", heap_bytes_free(), heap_bytes_used());
}

static void MainWindowUnload(Window *window)
{
 	//Unload other stuff
	UnloadMatchEngine();
	UnloadTime();
	UnloadWeather();
	UnloadBattery();
  UnloadScreenWipe();
	UnloadPlaneWindow();
  UnloadAutoAnimation();
 	UnloadBetweenFight();
	EndSteps();
}

static void Init()
{
  // Create main Window element and assign to pointer
  MainWindow = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(MainWindow, (WindowHandlers) {
    .load = MainWindowLoad,
    .unload = MainWindowUnload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(MainWindow, true);
  
  //Register javascript callbacks
  RegisterMessageCallbacks();
  
  // Make sure the time is displayed from the start
  UpdateTime();

	app_timer_register(2000, UpdateWeather, NULL);
	
  // Register with TickTimerService
  //tick_timer_service_subscribe(SECOND_UNIT, SecondTick);
  tick_timer_service_subscribe(MINUTE_UNIT, MinuteTick);
}

static void Deinit()
{
  window_destroy(MainWindow);
}

int main(void)
{
	//Sneaky stack usage
	//struct SquarePos MainSquares[NumSquares];
	//Squares = MainSquares;
	Squares = malloc(sizeof(struct SquarePos) * NumSquares);
	
  Init();
  
  app_event_loop();
  
  Deinit();
	
	free(Squares);
}