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

#define ball_speed 100
#define radius 7
#define ballCol 0x000000FF
#define clearCol 0x0000FF00
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

volatile Brick brick;
volatile int recalculate = 1;

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

int collision() {
	int dir_x, dir_y, changed = 0;
	if ((fishball.nextX - radius) > getBar_x0()
			&& (fishball.nextX + radius) < getBar_x1()
			&& (fishball.nextY + radius) >= 405) {
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
	}
	if (fishball.nextX - radius < 60) {
		fishball.x = 60 + radius;
		dir_x = -1.0 * fishball.dir_x;
		dir_y = fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
	} else if (fishball.nextX + radius > 514) {
		fishball.x = 514 - radius;
		dir_x = -1.0 * fishball.dir_x;
		dir_y = fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
	}
	if (fishball.nextY - radius < 60) {
		fishball.y = radius + 60;
		dir_x = fishball.dir_x;
		dir_y = -1.0 * fishball.dir_y;
		changeDir(dir_x, dir_y);
		changed = 1;
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

void calCoord(int recalculate, int* X, int* Y) {
	float scale, w;
	w = fishball.dir_x * fishball.dir_x + fishball.dir_y * fishball.dir_y;
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
	fishball.nextX = fishball.x + 0.5 * x;
	fishball.nextY = fishball.y + 0.5 * y;

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
	while (1) {
		pthread_mutex_lock(&mutex);
		while (!XMbox_IsEmpty(&Mbox)) {
			//xil_printf("Reading mail..\r\n");
			Mailbox_ReceiveBrick(&Mbox, &brick);

			Mailbox_ReceiveBall(&Mbox, &fishball);
			recalculate = 1;
			//xil_printf("Done reading..\r\n");
		}
		//xil_printf("brickid=%d",brick.id);
		if(brick.id !=0)drawBrickCol(brick.id, &brick.draw, brick.col, clearCol);
		if(brick.id==6)
		{
			xil_printf("ballx=%d, bally=%d",fishball.x,fishball.y);
		}
		drawCircle(fishball.prevX, fishball.prevY, radius, clearCol);
		drawCircle(fishball.x, fishball.y, radius, ballCol);
		drawBar();
		pthread_mutex_unlock(&mutex);
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

	startScreen(clearCol,0x00a52a2a);

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
