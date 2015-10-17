#pragma once
  
int GetCurrentWeatherResourceID();

bool NoWeatherIconSet();

void SetWeather(char *WeatherConditions);
void UpdateWeather();

void UpdateWeatherImage();

void AllocateWeather();

void LoadWeather(Window *ParentWindow);
void UnloadWeather();