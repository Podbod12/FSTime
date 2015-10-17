#pragma once

#define NumSquares 31

struct SquarePos
{
  int16_t StartXPos;
  
  int16_t EndYPos;
  int16_t EndXPos;
  
  int16_t CurrentYPos;
  int16_t CurrentXPos;

  uint8_t CurrentFrame;

	bool bBeingUsed;
};

struct SquarePos *Squares;

void AllocateWipe();

void LoadScreenWipe(Window *ParentWindow);
void UnloadScreenWipe();
void PerformWipe(void (*WipeMiddleCallback), void (*WipeOverCallback));

void ClearWipe();