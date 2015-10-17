#pragma once

//init
void LoadMatchEngine(Window *ParentWindow);
void UnloadMatchEngine();
	
//move callbacks
void WinPoseOver(int FinalXPos, int FinalYPos);

//Fight funcs
void BeginFight();
void QuickEndFight();