#include "xmk.h"
#include <pthread.h>
#include <errno.h>
#include "semaphore.h"
#include "xmbox.h"
#include <xparameters.h>

#define MY_CPU_ID XPAR_CPU_ID
#define MBOX_DEVICE_ID XPAR_MBOX_0_DEVICE_ID
#define brickColour 0x00a52a2a
#define GOLD 0x00FFD700
#define bgColour 0x0000b200
#define radius 7

pthread_t tid1,tid2,tid3,tid4,tid5,tid6,tid7,tid8,tid9,tid10,tid11;

typedef struct
{
	int id;
	int draw[8];
	int col;
}Brick;

typedef struct {
	int dir_x,dir_y,colour,x,y,prevX,prevY,nextX,nextY;
}ball;


XMbox Mbox;
sem_t sem, updateBall;
static ball fishball;
volatile int thread = -1;

static void Mailbox_Receive(XMbox *MboxInstancePtr, ball *ball_pointer)
{
	XMbox_ReadBlocking(MboxInstancePtr,	ball_pointer,36);
}

void changeBall(int whichSide, int dirX, int dirY, int dirArray[]) {
	//top
	if(whichSide == 0) {
		dirArray[0] = dirX;
		dirArray[1] = dirY * -1;
	}
	//btm
	else if(whichSide == 1) {
		dirArray[0] = dirX;
		dirArray[1] = dirY * -1;
	}
	//left
	else if(whichSide == 2) {
		dirArray[0] = dirX * -1;
		dirArray[1] = dirY;
	}
	//right
	else {
		dirArray[0] = dirX * -1;
		dirArray[1] = dirY;
	}
}

void detectCollisionColumn(int column, int y, int x, int dirX, int dirY, int dirArray[], int draw[]) {
	int index = -1;
	int leftBound = column*65;
	int rightBound = leftBound+39;
	index = (y-65)/20;
	if (index<8) {
		//collision from top/btm
		if(x-radius<=rightBound || x+radius>=leftBound) {
			//coming from top
			if (dirY == 1) {
				index += 1;
				if (draw[index]) {
					if(y+radius >= index*20+65) {
						changeBall(0,dirX,dirY,dirArray);
						draw[index] = 0;
					}
				}
			}
			//coming from btm
			else {
				if(draw[index]) {
					if(y-radius <= index*20+79) {
						changeBall(1,dirX,dirY,dirArray);
						draw[index] = 0;
					}
				}
			}
		}
		//collision from side
		else {
			//coming from left
			if(dirX == 1) {
				if(x+radius >= leftBound) {
					//topBrick
					if(y-radius <= index*20+79) {
						if(draw[index]) {
							changeBall(2,dirX,dirY,dirArray);
							draw[index] = 0;
						}
					}
					//bottomBrick
					else if(y+radius >= index*20+65) {
						index += 1;
						if(draw[index]) {
							changeBall(2,dirX,dirY,dirArray);
							draw[index] = 0;
						}
					}
				}
			}
			else {
				//coming from right
				if(x-radius <= rightBound) {
					//topBrick
					if(y-radius <= index*20+79) {
						if(draw[index]) {
							changeBall(3,dirX,dirY,dirArray);
							draw[index] = 0;
						}
					}
					//bottomBrick
					else if(y+radius >= index*20+65) {
						index += 1;
						if(draw[index]) {
							changeBall(3,dirX,dirY,dirArray);
							draw[index] = 0;
						}
					}
				}
			}
		}
	}
}

int detectCollisionThread(int x, int dirX) {
	int index = -1;
	index = (x-65)/45;
	if(dirX == 1) {
		index += 1;
	}
	return index;

}

void* signalThread() {
	sem_wait(&updateBall);
	Mailbox_Receive(&Mbox, &fishball);
	thread = detectCollisionThread(fishball.nextX, fishball.dir_x);
	sem_post(&updateBall);

}



void* brickCol_1 () {
	Brick brick1;
	brick1.id = 1;
	while(1)
	{
		sem_wait(&sem);
		//change To Gold
		brick1.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick1, 12);
		}
		sleep(1000);

		sem_post(&sem);
		//change Back
		brick1.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick1, 12);
		}
	}
}

void* brickCol_2 () {
	Brick brick2;
	brick2.id = 2;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick2.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick2, 12);
		}
		sleep(789);

		sem_post(&sem);
		//change Back
		brick2.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick2, 12);
		}
	}
}

void* brickCol_3 () {
	Brick brick3;
	brick3.id = 3;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick3.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick3, 12);
		}
		sleep(345);
		sem_post(&sem);

		//change Back
		brick3.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick3, 12);
		}
	}
}

void* brickCol_4 () {
	Brick brick4;
	brick4.id = 4;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick4.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick4, 12);
		}
		sleep(479);
		sem_post(&sem);

		//change Back
		brick4.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick4, 12);
		}
	}
}

void* brickCol_5 () {
	Brick brick5;
	brick5.id = 5;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick5.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick5, 12);
		}
		sleep(777);
		sem_post(&sem);

		//change Back
		brick5.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick5, 12);
		}
	}
}

void* brickCol_6 () {
	Brick brick6;
	brick6.id = 6;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick6.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick6, 12);
		}
		sleep(890);
		sem_post(&sem);

		//change Back
		brick6.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick6, 12);
		}
	}
}

void* brickCol_7 () {
	Brick brick7;
	brick7.id = 7;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick7.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick7, 12);
		}
		sleep(1123);
		sem_post(&sem);

		//change Back
		brick7.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick7, 12);
		}
	}
}

void* brickCol_8 () {
	Brick brick8;
	brick8.id = 8;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick8.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick8, 12);
		}
		sleep(608);
		sem_post(&sem);

		//change Back
		brick8.col = brickColour;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick8, 12);
		}
	}
}

void* brickCol_9 () {
	Brick brick9;
	brick9.id = 9;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick9.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick9, 12);
		}
		sleep(945);
		sem_post(&sem);

		//change Back
		brick9.col = brickColour;
		//Send Mail
		XMbox_WriteBlocking(&Mbox, &brick9, 12);
	}
}

void* brickCol_10 () {
	Brick brick10;
	brick10.id = 10;
	while(1)
	{

		sem_wait(&sem);
		//change To Gold
		brick10.col = GOLD;
		//Send Mail
		if (!XMbox_IsFull(&Mbox)) {
			XMbox_WriteBlocking(&Mbox, &brick10, 12);
		}
		sleep(1234);
		sem_post(&sem);

		//change Back
		brick10.col = brickColour;
		//Send Mail
		XMbox_WriteBlocking(&Mbox, &brick10, 12);
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
	int Status;
	XMbox_Config *ConfigPtr;
	ConfigPtr = XMbox_LookupConfig(MBOX_DEVICE_ID);
	if (ConfigPtr == (XMbox_Config*)NULL)
	{
		print("--Error configuring Mbox Sender--\r\n");
		return XST_FAILURE;
	}

	Status = XMbox_CfgInitialize(&Mbox, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		print("--Error initializing Mbox Sender--\r\n");
		return XST_FAILURE;
	}

	if(sem_init(&sem, 1, 2) <0)
	{
		print("Error while initializing semaphore sem. \r\n");
	}

	if(sem_init(&updateBall, 1, 1 <0))
	{
		print("Error while initializing semaphore updateBall. \r\n");
	}

	ret = pthread_create (&tid11, NULL, (void*)signalThread, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching signalThread...\r\n", ret);
	}
	else {
		xil_printf ("Thread signalThread launched with ID %d \r\n", tid11);
	}

	ret = pthread_create (&tid1, NULL, (void*)brickCol_1, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_1...\r\n", ret);
	}
	else {
		xil_printf ("Thread 1 launched with ID %d \r\n", tid1);
	}

	ret = pthread_create (&tid2, NULL, (void*)brickCol_2, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_2...\r\n", ret);
	}
	else {
		xil_printf ("Thread 2 launched with ID %d \r\n", tid2);
	}

	ret = pthread_create (&tid3, NULL, (void*)brickCol_3, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_3...\r\n", ret);
	}
	else {
		xil_printf ("Thread 3 launched with ID %d \r\n", tid3);
	}

	ret = pthread_create (&tid4, NULL, (void*)brickCol_4, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_4...\r\n", ret);
	}
	else {
		xil_printf ("Thread 4 launched with ID %d \r\n", tid4);
	}

	ret = pthread_create (&tid5, NULL, (void*)brickCol_5, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_5...\r\n", ret);
	}
	else {
		xil_printf ("Thread 5 launched with ID %d \r\n", tid5);
	}

	ret = pthread_create (&tid6, NULL, (void*)brickCol_6, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_6...\r\n", ret);
	}
	else {
		xil_printf ("Thread 6 launched with ID %d \r\n", tid6);
	}

	ret = pthread_create (&tid7, NULL, (void*)brickCol_7, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_7...\r\n", ret);
	}
	else {
		xil_printf ("Thread 7 launched with ID %d \r\n", tid7);
	}

	ret = pthread_create (&tid8, NULL, (void*)brickCol_8, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_8...\r\n", ret);
	}
	else {
		xil_printf ("Thread 8 launched with ID %d \r\n", tid8);
	}

	ret = pthread_create (&tid9, NULL, (void*)brickCol_9, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_9...\r\n", ret);
	}
	else {
		xil_printf ("Thread 9 launched with ID %d \r\n", tid9);
	}

	ret = pthread_create (&tid10, NULL, (void*)brickCol_10, NULL);
	if (ret != 0) {
		xil_printf ("-- ERROR (%d) launching brickCol_10...\r\n", ret);
	}
	else {
		xil_printf ("Thread 10 launched with ID %d \r\n", tid10);
	}
	return 0;
}
