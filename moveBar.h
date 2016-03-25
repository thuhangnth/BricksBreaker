#include "xgpio.h"

static void gpPBIntHandler(void *arg);
void moveBar();
void drawBar();
void initInt();


XGpio gpPB; //PB device instance.
long last_int_time1 = 0, last_int_time0 = 0;
int btn;

def struct{
	int x;
	int y = 450;
	int dir, col;
}Bar;

Bar bar;

const int LEFT_BTN = 4;
const int RIGHT_BTN = 8;
const int LENGTH = 80;
const int DIST=10;