#include "moveBar.h"

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

void moveBar()
{
	if (btn==LEFT_BTN)
	{	
		bar.dir = LEFT_BTN;
		//Decrement x coordinate
		bar.x = bar.x-DIST;
		if (bar.x<60)  //Bar hits left wall
		{		
		bar.x=60;
		}
	}
	else if (btn==RIGHT_BTN)
	{
		bar.dir = RIGHT_BTN;
		//Increment x coordinate
		bar.x = bar.x+DIST;
		if (bar.x>454-LENGTH+1) //Bar hits right wall
		{
			bar.x=545-LENGTH+1;
		}
	}
	
}

void drawBar()
{
	drawRect(bar.x,bar.y,5,LENGTH,bar.col);
	if(bar.dir==LEFT_BTN)
	{
		drawRect(bar.x+LENGTH,bar.y,5,DIST,colBG);
	}
	else if(bar.dir == RIGHT_BTN)
	{
		drawRect(bar.x-DIST,bar.y,5,DIST,colBG); 
	}
}

void initInt()
{
	// Initialise the PB instance
	Status = XGpio_Initialize(&gpPB, XPAR_GPIO_0_DEVICE_ID);
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