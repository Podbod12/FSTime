#pragma once
  
//Compile options
#define PHONEBUILD
#define FINALBUILD
	
#ifndef FINALBUILD
	#define FORCECHARACTER 2
	#define FORCEOPPONENT 2
	//#define FORCEMOVE 1
	//#define FORCEWINNER 1
	#define FORCEMATCH
#else
	//#define FORCEMATCH
	//#define FORCE5MINMATCH
#endif
	
#define CHARACTERMEMORYSAVE 1
	
void UpdateWeatherImage(); 

void LoadPlaneWindow();
void UnloadPlaneWindow();
void LoadBetweenWindow();
void UnloadBetweenWindow();

void ReserveAnimationMemory();
void ReleaseMemory();
void ReserveBackMemory();
void ReleaseBackMemory();

void RestartAfterSettingsChange();