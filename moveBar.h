#ifndef MOVEBAR_H
#define MOVEBAR_H

#include "xgpio.h"
#include "drawScreen.h"
#include "xmk.h"
#include <sys/timer.h>



void gpPBIntHandler(void *arg);
void moveBar();
void drawBar();
void initInt();
void initBar();
int getBar_x0();
int getBar_x1();


#endif
