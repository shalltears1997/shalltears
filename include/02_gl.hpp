#pragma once

#include "c_struct_position.hpp"

void myReshape(int ww, int hh);
void drawGoal(Position current);
void myDisplay(void);
void myInit();
void myTimer(int value);

void myKeyboard(unsigned char key, int x, int y);
void myMouseFunc(int button, int state, int xx, int yy);
void myMouseMotion(int xx , int yy);
