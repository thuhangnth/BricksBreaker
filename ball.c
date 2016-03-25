/*
 * ball.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Mason
 */

#include <pthread.h>
#include <math.h>
#include "moveBar.h"
#include "xmutex.h"


#define ball_speed 100
#define radius 7
#define ballCol 0x000000FF
#define clearCol 0x0000FF00
#define MUTEX_DEVICE_ID XPAR_MUTEX_0_IF_1_DEVICE_ID
#define MUTEX_NUM 0

typedef struct {
	int dir_x,dir_y,colour,x,y,prevX,prevY;
}ball;

static ball fishball;

pthread_t tid1,tid2,tid3;

XMutex Mutex;

void changeDir(int x, int y) {
	fishball.dir_x = x;
	fishball.dir_y = y;
}

void initBall() {
	fishball.dir_x = 1;
	fishball.dir_y = 1;
	fishball.x = 226;
	fishball.y = 398;
	fishball.prevX = 0;
	fishball.prevY = 0;
}

int collision() {
	int dir_x,dir_y,changed = 0;
	if ((fishball.x - radius) > getBar_x0() && (fishball.x + radius) < getBar_x1() && (fishball.y + radius) >= 410)
	{
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x,dir_y);
		changed = 1;
	}
	if (fishball.x - radius < 60) {
		fishball.x = 60+radius;
		dir_x = -1.0 * fishball.dir_x;
		dir_y = fishball.dir_y;
		changeDir(dir_x,dir_y);
		changed = 1;
	}
	else if (fishball.x + radius > 454)
	{
		fishball.x = 454-radius;
		dir_x = -1.0 * fishball.dir_x;
		dir_y = fishball.dir_y;
		changeDir(dir_x,dir_y);
		changed = 1;
	}
	if(fishball.y - radius < 60) {
		fishball.y = radius + 60;
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x,dir_y);
		changed = 1;
	}
	else if(fishball.y + radius >419)
	{
		fishball.y = -radius + 419;
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x,dir_y);
		changed = 1;
	}
	if (changed) {
		return 1;
	}
	return 0;
}

void calCoord(int recalculate, int* X, int* Y) {
	float scale, w;
	w = fishball.dir_x*fishball.dir_x + fishball.dir_y*fishball.dir_y;
	scale = ball_speed / w;
	scale = sqrt(scale);
	*X = scale * fishball.dir_x;
	*Y = scale * fishball.dir_y;
}

void updateXY(int x, int y) {
	fishball.prevX = fishball.x;
	fishball.prevY = fishball.y;
	fishball.x += x;
	fishball.y += y;

}

void* bounceBall() {
	int recalculate = 1, distX=0, distY=0;
	initBall();
	drawCircle(fishball.x,fishball.y,radius,ballCol);
	while(1) {
		if (recalculate == 1)
		{
			calCoord(recalculate, &distX, &distY);
			recalculate = 0;
		}
		updateXY(distX,distY);
		recalculate = collision();
		sleep(40);
	}
}

void* controlBar() {
	while(1) {
		moveBar();
		sleep(40);
	}
}

void* disp() {
	while(1) {
		drawCircle(fishball.prevX,fishball.prevY,radius,clearCol);
		drawCircle(fishball.x,fishball.y,radius,ballCol);
		drawBar();
		sleep(40);
	}
}
int main_prog(void);
int main (void) {
	print("-- Entering main() uB0 RECEIVER--\r\n");
	xilkernel_init();
	xmk_add_static_thread(main_prog,0);
	xilkernel_start();
	//Start Xilkernel
	xilkernel_main ();

	//Control does not reach here
	return 0;
}

int main_prog(void) {
	int ret;
	initTFT();
	initInt();
	initBar();
	startScreen(clearCol,clearCol,clearCol);
	XStatus Status;

	XMutex_Config*ConfigPtr;
	ConfigPtr = XMutex_LookupConfig(MUTEX_DEVICE_ID);
	if (ConfigPtr == (XMutex_Config *)NULL){

		xil_printf("B1-- ERROR  init HW mutex...\r\n");
	}

	Status = XMutex_CfgInitialize(&Mutex, ConfigPtr, ConfigPtr->BaseAddress);

	ret = pthread_create (&tid1, NULL, (void*)bounceBall, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching bounceBall...\r\n", ret);
	}
	else {
		xil_printf ("bounceBall launched with ID %d \r\n", tid1);
	}

	ret = pthread_create (&tid2, NULL, (void*)controlBar, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching controlBar...\r\n", ret);
	}
	else {
		xil_printf ("controlBar launched with ID %d \r\n", tid2);
	}

	ret = pthread_create (&tid3, NULL, (void*)disp, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching disp...\r\n", ret);
	}
	else {
		xil_printf ("disp launched with ID %d \r\n", tid3);
	}
}

