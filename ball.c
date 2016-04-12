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
#include "xmbox.h"
#include <sys/timer.h>
#include <sys/types.h>

#define radius 7
#define ballCol 0x0000b200
#define clearCol 0x00000000
//#define MUTEX_DEVICE_ID XPAR_MUTEX_0_IF1_DEVICE_ID
//#define MUTEX_NUM 0

pthread_mutex_t mutex;
pthread_mutex_t m_attr;
pthread_attr_t attr;

typedef struct {
	int dir_x, dir_y, colour, x, y, prevX, prevY, nextX, nextY;
} ball;

typedef struct {
	int id;
	int draw[8];
	int col;
} Brick;

typedef struct {
	int score, brickLeft;
}Result;

volatile Brick brick;
volatile int recalculate = 1;
volatile Result scoreBrick;
volatile int angle = 45;
volatile int ball_speed = 10;

#define MY_CPU_ID XPAR_CPU_ID
#define MBOX_DEVICE_ID XPAR_MBOX_0_DEVICE_ID
static XMbox Mbox;

static ball fishball;
struct sched_param sched_par;
pthread_attr_t attr;
pthread_t tid1, tid2, tid3;

//XMutex Mutex;

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
	fishball.nextX = 226;
	fishball.nextY = 398;
}

float deg2rad(int deg)
{
	float rad;
	rad = deg * M_PI / 180;
	return rad;
}

void calCoord(int recalculate, int* X, int* Y)
{
	float rad;
	rad = deg2rad(angle);
	*X = ball_speed * sin(rad) * fishball.dir_x;
	*Y = ball_speed* cos(rad) * fishball.dir_y;
}

void changeAngle(int plusMinus)
{
	//plus angle
	if(plusMinus == 1 && angle < 75) {
		angle += 15;
	}
	else if(plusMinus == 0 && angle > 15)
	{
		angle -= 15;
	}
}

int collision() {
	int dir_x, dir_y, changed = 0;
	//hit bar
	int centre_x = fishball.nextX;
	int centre_y = fishball.nextY;
	if ((centre_x - radius) > (getBar_x0()-5)
			&& ((centre_x + radius) < getBar_x1()+5)
			&& ((centre_y + radius) >= 407)) {
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x, dir_y);
		if((centre_x+radius)< getBar_x0() + 10)
		{
			//decrease angle
			changeAngle(0);
		}
		else if(centre_x -radius >= getBar_x0()+ 70)
		{
			//increase angle
			changeAngle(1);
		}
		else if(centre_x +radius< getBar_x0()+20)
		{
			//decrease speed
			if (ball_speed > 5)
				ball_speed -= 4;
		}
		else if(centre_x -radius >= getBar_x0()+60)
		{
			//increase speed
			if (ball_speed < 37)
				ball_speed += 4;
		}
		changed = 1;
	}
	//hit left wall
	if (fishball.nextX - radius < 60) {
		fishball.x = 60 + radius;
		dir_x = -1.0 * fishball.dir_x;
		dir_y = fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
	//hit right wall
	} else if (fishball.nextX + radius > 514) {
		fishball.x = 514 - radius;
		dir_x = -1.0 * fishball.dir_x;
		dir_y = fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
	}
	//hit top wall
	if (fishball.nextY - radius < 60) {
		fishball.y = radius + 60;
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
	//hit bottom wall
	} else if (fishball.nextY + radius > 419) {
		fishball.y = -radius + 419;
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
	}
	if (changed) {
		return 1;
	}
	return 0;
}


void updateXY(int x, int y) {
	fishball.prevX = fishball.x;
	fishball.prevY = fishball.y;
	fishball.x += x;
	fishball.y += y;
	fishball.nextX = fishball.x + x;
	fishball.nextY = fishball.y + y;

}

void* bounceBall() {
	int distX = 0, distY = 0;
	initBall();
	drawCircle(fishball.x, fishball.y, radius, ballCol);
	while (1) {
		pthread_mutex_lock(&mutex);
		if (recalculate == 1) {
			calCoord(recalculate, &distX, &distY);
			recalculate = 0;
		}
		updateXY(distX, distY);
		recalculate = collision();
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &fishball, sizeof(fishball));
			//xil_printf("Ball.c SEND dir_x = %d, dir_y = %d\r\n",fishball.dir_x,fishball.dir_y);
		}
		pthread_mutex_unlock(&mutex);
		sleep(40);
	}
}

void* controlBar() {
	int hold = 0;
	long time0 = 0, time1 = 0;
	while (1) {
		pthread_mutex_lock(&mutex);
		hold = moveBar(time1 - time0);
		if (!hold) {
			time0 = xget_clock_ticks();
			time1 = xget_clock_ticks();
		} else {
			time1 = xget_clock_ticks();
		}
		pthread_mutex_unlock(&mutex);
		sleep(40);
	}
}

void* disp() {
	int prevTime = 0;
	int nextTime = 0;
	int totalTime = 0;
	int frameCount = 0;
	while (1) {
		frameCount += 1;
		nextTime = xget_clock_ticks();
		if(nextTime - prevTime >=100) {
			prevTime = xget_clock_ticks();
			writeFPS(frameCount);
			frameCount = 0;
			totalTime += 1;
		}
		pthread_mutex_lock(&mutex);
		while (!XMbox_IsEmpty(&Mbox)) {
			xil_printf("Reading mail..\r\n");
			Mailbox_ReceiveBrick(&Mbox, &brick);

			Mailbox_ReceiveBall(&Mbox, &fishball);
			Mailbox_ReceiveScore(&Mbox, &scoreBrick);
			xil_printf("Done reading..\r\n");
			recalculate = 1;
			writeScore(scoreBrick.score);
			writeBrickNo(scoreBrick.brickLeft);

		}
		//xil_printf("brickid=%d",brick.id);
		if(brick.id !=0)drawBrickCol(brick.id, &brick.draw, brick.col, clearCol);
		drawCircle(fishball.prevX, fishball.prevY, radius, clearCol);
		drawCircle(fishball.x, fishball.y, radius, ballCol);
		drawBar();
		xil_printf("speed = %d",ball_speed);
		writeSpeed(ball_speed*25);
		writeTime(totalTime/60, totalTime%60);
		pthread_mutex_unlock(&mutex);
		xil_printf("Angle la: %d\r\n",angle);
		sleep(40);
	}
}
int main_prog(void);
int main(void) {
	print("-- Entering main() uB0 RECEIVER--\r\n");
	xilkernel_init();
	xmk_add_static_thread(main_prog, 0);
	xilkernel_start();
	//Start Xilkernel
	xilkernel_main();

	//Control does not reach here
	return 0;
}

int main_prog(void) {
	int ret;
	int i;
	initTFT();
	initInt();
	initBar();
	pthread_mutexattr_init(&m_attr);
	pthread_mutex_init(&mutex, &m_attr);

	startScreen(clearCol,0x00FF0000);

	XStatus Status;

	XMbox_Config *ConfigPtr;
	ConfigPtr = XMbox_LookupConfig(MBOX_DEVICE_ID);
	XMbox_CfgInitialize(&Mbox, ConfigPtr, ConfigPtr->BaseAddress);

	//XMutex_Config*ConfigPtr_Mutex;
	/*ConfigPtr_Mutex = XMutex_LookupConfig(MUTEX_DEVICE_ID);
	 if (ConfigPtr_Mutex == (XMutex_Config *)NULL){

	 xil_printf("B1-- ERROR  init HW mutex...\r\n");
	 }*/

	//Status = XMutex_CfgInitialize(&Mutex, ConfigPtr_Mutex, ConfigPtr_Mutex->BaseAddress);
	//pthread_attr_init (&attr);
	//sched_par.sched_priority = 2;
	//pthread_attr_setschedparam(&attr,&sched_par);
	ret = pthread_create(&tid1, NULL, (void*) bounceBall, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching bounceBall...\r\n", ret);
	} else {
		xil_printf("bounceBall launched with ID %d \r\n", tid1);
	}

	//sched_par.sched_priority = 2;
	//pthread_attr_setschedparam(&attr,&sched_par);
	ret = pthread_create(&tid2, NULL, (void*) controlBar, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching controlBar...\r\n", ret);
	} else {
		xil_printf("controlBar launched with ID %d \r\n", tid2);
	}

	//sched_par.sched_priority = 1;
	//pthread_attr_setschedparam(&attr,&sched_par);
	ret = pthread_create(&tid3, NULL, (void*) disp, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching disp...\r\n", ret);
	} else {
		xil_printf("disp launched with ID %d \r\n", tid3);
	}
}

void Mailbox_ReceiveBrick(XMbox *MboxInstancePtr, Brick *brick) {
	//int* bytes;
	//*bytes = 12;
	Brick temp;
	int i;
	XMbox_ReadBlocking(MboxInstancePtr, &temp, sizeof(Brick));
	//xil_printf("Temp id = %d\r\n",temp.id);
	brick->id = temp.id;
	for(i=0; i<8; i++)
	{
		brick->draw[i] = temp.draw[i];
	}
	brick->col = temp.col;
	//xil_printf("Brick id = %d\r\n",brick->id);
}

void Mailbox_ReceiveBall(XMbox *MboxInstancePtr, ball *fishball) {
	ball temp;
	XMbox_ReadBlocking(MboxInstancePtr, &temp, sizeof(ball));
	changeDir(temp.dir_x, temp.dir_y);
	//xil_printf("temp.dir_x = %d, temp.dir_y = %d\r\n",temp.dir_x,temp.dir_y);
	//xil_printf("fishball.dir_x = %d, fishball.dir_y = %d\r\n",fishball->dir_x,fishball->dir_y);
	//xil_printf("Ball.c dir_x = %d, dir_y = %d\r\n",fishball->dir_x,fishball->dir_y);
}

void Mailbox_ReceiveScore(XMbox *MboxInstancePtr, Result *score) {
	Result temp;
	XMbox_ReadBlocking(MboxInstancePtr, &temp, sizeof(Result));
	score->score = temp.score;
	score->brickLeft = temp.brickLeft;
}
