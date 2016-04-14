#include "moveBar.h"

#define colBG 0x00000000
#define colBar 0x00FF0000
#define colSpeed 0x00ffff00
#define LEFT_BTN 4
#define RIGHT_BTN 8
#define RELEASE 0
#define LENGTH 80
#define DIST 25
#define HOLD_DIST 8

XGpio gpPB; //PB device instance.
long lastlast_int_time1 = 0, lastlast_int_time0 = 0;
int btn;
int start =0;

typedef struct{
	int x;
	int y;
	int dir, col;
}Bar;

static Bar bar;

int moveBar(long time)
{
	if (btn==LEFT_BTN)
	{
		bar.dir = LEFT_BTN;
		//Decrement x coordinate
		if (time < 25) {
			bar.x = bar.x-DIST;
		}
		else
		{
			bar.x = bar.x-HOLD_DIST;
		}
		if (bar.x<61)  //Bar hits left wall
		{
		bar.x=61;
		}
		return 0;
	}
	else if (btn==RIGHT_BTN)
	{
		bar.dir = RIGHT_BTN;
		//Increment x coordinate
		if (time < 25) {
			bar.x = bar.x+DIST;
		}
		else
		{
			bar.x = bar.x+HOLD_DIST;
		}
		if (bar.x>514-LENGTH) //Bar hits right wall
		{
			bar.x=514-LENGTH;
		}
		return 0;
	}
	else if (btn == RELEASE)
	{
		return 1;
	}

}

void drawBar()
{
	drawRect(bar.x,bar.y,5,LENGTH,bar.col);
	drawRect(bar.x+10,bar.y,5,10,colSpeed);
	drawRect(bar.x+60,bar.y,5,10,colSpeed);
	if(bar.dir==LEFT_BTN)
	{
		if (bar.x+LENGTH+DIST > 514 ) {
			drawRect(bar.x+LENGTH,bar.y,5,514-(bar.x+LENGTH),colBG);
		}
		else {
			drawRect(bar.x+LENGTH,bar.y,5,DIST,colBG);
		}
	}
	else if(bar.dir == RIGHT_BTN)
	{
		if (bar.x - DIST < 61 )
		{
			drawRect(61,bar.y,5,bar.x-61,colBG);
		}
		else
		{
			drawRect(bar.x-DIST,bar.y,5,DIST,colBG);
		}
	}
}

void initBar() {
	bar.x = 226;
	bar.y = 405;
	bar.col = colBar;
}

void initInt()
{
	// Initialise the PB instance
	XGpio_Initialize(&gpPB, XPAR_GPIO_0_DEVICE_ID);
	// set PB gpio direction to input.
	XGpio_SetDataDirection(&gpPB, 1, 0x000000FF);

	xil_printf("Enabling PB interrupts\r\n");
	//global enable
	XGpio_InterruptGlobalEnable(&gpPB);
	// interrupt enable. both global enable and this function should be called to enable gpio interrupts.
	XGpio_InterruptEnable(&gpPB,1);
	//register the handler with xilkernel
	register_int_handler(XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_0_IP2INTC_IRPT_INTR, gpPBIntHandler, &gpPB);
	//enable the interrupt in xilkernel
	enable_interrupt(XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_0_IP2INTC_IRPT_INTR);
}

void gpPBIntHandler(void *arg) //Should be very short (in time). In a practical program, don't print etc.
{
	start = 1;
	unsigned char val;
	//clear the interrupt flag. if this is not done, gpio will keep interrupting the microblaze.--
	// --Possible to use (XGpio*)arg instead of &gpPB
	XGpio_InterruptClear(&gpPB,1);
	long int_time = xget_clock_ticks();
	val = XGpio_DiscreteRead(&gpPB, 1);
	/*if (val>0) {
		if (int_time - lastlast_int_time1>5 && int_time - lastlast_int_time0> 5)
		{
			//Read the state of the push buttons.
			btn = val;
		}
		lastlast_int_time1 = int_time;
	}
	else {
		if (int_time - lastlast_int_time0 > 5)
		{
			//Read the state of the push buttons.
			btn = val;
		}
		lastlast_int_time0 = int_time;
	}*/
	btn = val;
}

int getBar_x0()
{
	return bar.x;
}

int getBar_x1()
{
	return bar.x+LENGTH-1;
}
