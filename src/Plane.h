#pragma once

void UpdatePlane(struct tm *tick_time);

void UpdateFlags();

void RestartPlane();
	
void LoadPlane(Window *ParentWindow, bool bFirstInit);

void UnloadPlane();