#include "xgpio.h"
#include "drawScreen.h"
#include "xmk.h"
#include <sys/timer.h>
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

static void gpPBIntHandler(void *arg) //Should be very short (in time). In a practical program, don't print etc.
{
	unsigned char val;
	//clear the interrupt flag. if this is not done, gpio will keep interrupting the microblaze.--
	// --Possible to use (XGpio*)arg instead of &gpPB
	XGpio_InterruptClear(&gpPB,1);
	long int_time = xget_clock_ticks();
	val = XGpio_DiscreteRead(&gpPB, 1);
	if (val>0) {
		if (int_time - last_int_time1>20 && int_time - last_int_time0> 20)
		{
			//Read the state of the push buttons.
			btn = val;
		}
		last_int_time1 = int_time;
	}
	else {
		if (int_time - last_int_time0 > 20)
		{
			//Read the state of the push buttons.
			btn = val;
		}
		last_int_time0 = int_time;
	}
}
