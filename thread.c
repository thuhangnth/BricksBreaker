#include "xmk.h"
#include <pthread.h>
#include <errno.h>
#include "semaphore.h"
#include "xmbox.h"
#include <xparameters.h>
#include <sys/process.h>

#define MY_CPU_ID XPAR_CPU_ID
#define MBOX_DEVICE_ID XPAR_MBOX_0_DEVICE_ID
#define brickColour 0x00a52a2a
#define GOLD 0x00FFD700
#define bgColour 0x0000b200
#define radius 7

pthread_t tid1, tid2, tid3, tid4, tid5, tid6, tid7, tid8, tid9, tid10, tid11;

typedef struct {
	int id;
	int draw[8];
	int col;
} Brick;

typedef struct {
	int dir_x, dir_y, colour, x, y, prevX, prevY, nextX, nextY;
} ball;

XMbox Mbox;
sem_t /*sem,*/ updateBall, sem_1, sem_2, sem_3, sem_4, sem_5, sem_6, sem_7, sem_8,
		sem_9, sem_10, sem_gold;
static ball fishball;
volatile int thread = -1;
volatile int score = 0;
int to_release[] = {0,0,0,0,0,0,0,0,0,0};

static void Mailbox_Receive(XMbox *MboxInstancePtr, ball *ball_pointer) {
	XMbox_ReadBlocking(MboxInstancePtr, ball_pointer, 36);
}

void initializeBrick(Brick* brickPtr)
{
	int i;
	for(i=0; i<8; i++)
	{
		brickPtr->draw[i] = 1;
	}
	brickPtr->col = brickColour;
}

void writeFishball(int dirArray[])
{
	fishball.dir_x = dirArray[0];
	fishball.dir_y = dirArray[1];
	xil_printf("Thread.c dir_x = %d, dir_y = %d\r\n",fishball.dir_x,fishball.dir_y);
}

void changeBall(int whichSide, int dirX, int dirY, int dirArray[]) {
	//top
	xil_printf("changeBall dir x = %d, dir y = %d\r\n",dirX,dirY);
	xil_printf("whichSide = %d\r\n",whichSide);
	if (whichSide == 0) {
		dirArray[0] = dirX;
		dirArray[1] = dirY * -1;
	}
	//btm
	else if (whichSide == 1) {
		dirArray[0] = dirX;
		dirArray[1] = dirY * -1;
	}
	//left
	else if (whichSide == 2) {
		dirArray[0] = dirX * -1;
		dirArray[1] = dirY;
	}
	//right
	else {
		dirArray[0] = dirX * -1;
		dirArray[1] = dirY;
	}
}

int detectCollisionColumn(int column, int x, int y, int dirX, int dirY,
		int dirArray[], int draw[]) {
	int index = -1;
	int leftBound = 65+45*(column-1);
	int rightBound = leftBound + 39;
	index = (y - 65) / 20;  //row index
	if (index < 8) {
		//collision from top/btm
		if (x - radius <= rightBound || x + radius >= leftBound) {
			//xil_printf("a");
			//coming from top
			if (dirY == 1) {
				//xil_printf("a1");
				//index += 1;
				if (draw[index]) {
					//xil_printf("a2");
					if (y + radius >= index * 20 + 65) {
						//xil_printf("a3:%d:%d",column,index);
						changeBall(0, dirX, dirY, dirArray);
						draw[index] = 0;
						return 1;
					}
				}
			}
			//coming from btm
			else {
				if (draw[index]) {
					//xil_printf("b");
					if (y - radius <= index * 20 + 79) {
						changeBall(1, dirX, dirY, dirArray);
						draw[index] = 0;
						//xil_printf("b1");
						return 1;
					}
				}
			}
		}
		//collision from side
		else {
			//coming from left
			if (dirX == 1) {
				if (x + radius >= leftBound) {
					//topBrick
					if (y - radius <= index * 20 + 79) {
						if (draw[index]) {
							changeBall(2, dirX, dirY, dirArray);
							draw[index] = 0;
							return 1;
						}
					}
					//bottomBrick
					else if (y + radius >= index * 20 + 65) {
						index += 1;
						if (draw[index]) {
							changeBall(2, dirX, dirY, dirArray);
							draw[index] = 0;
							return 1;
						}
					}
				}
			} else {
				//coming from right
				if (x - radius <= rightBound) {
					//topBrick
					if (y - radius <= index * 20 + 79) {
						if (draw[index]) {
							changeBall(3, dirX, dirY, dirArray);
							draw[index] = 0;
							return 1;
						}
					}
					//bottomBrick
					else if (y + radius >= index * 20 + 65) {
						index += 1;
						if (draw[index]) {
							changeBall(3, dirX, dirY, dirArray);
							draw[index] = 0;
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}

int detectCollisionThread(int x, int dirX) {
	int index = -1;
	index = (x - 65) / 45;
	/*if (dirX == 1) {
		index += 1;
	}*/
	return index;

}

void* signalThread() {
	while (1) {
		sem_wait(&updateBall);
		//xil_printf("Prev Dir_x = %d, Prev Dir_Y = %d\r\n", fishball.dir_x, fishball.dir_y);
		Mailbox_Receive(&Mbox, &fishball);
		//xil_printf("Curr Dir_x = %d, Curr Dir_Y = %d\r\n", fishball.dir_x, fishball.dir_y);
		thread = detectCollisionThread(fishball.nextX, fishball.dir_x);
		sem_post(&updateBall);
		switch (thread) {
		case 0:
			sem_post(&sem_1);
			break;
		case 1:
			sem_post(&sem_2);
			break;
		case 2:
			sem_post(&sem_3);
			break;
		case 3:
			sem_post(&sem_4);
			break;
		case 4:
			sem_post(&sem_5);
			break;
		case 5:
			sem_post(&sem_6);
			break;
		case 6:
			sem_post(&sem_7);
			break;
		case 7:
			sem_post(&sem_8);
			break;
		case 8:
			sem_post(&sem_9);
			break;
		case 9:
			sem_post(&sem_10);
			break;
		}
		if(score!=0 && score%10==0)
		{
			int i;
			for (i=0;i<10;i++)
			{
				to_release[i] =1;
			}
		}
		sleep(40);
	}
}

void doit(sem_t* sem, int changed, int dirArray[], Brick* b)
{
	sem_wait(sem);

		changed = detectCollisionColumn(b->id, fishball.nextX, fishball.nextY,
				fishball.dir_x, fishball.dir_y, dirArray, b->draw);
		xil_printf("%d %d\r\n",b->id,changed);
		if (changed) {
			xil_printf("c");
			sem_wait(&updateBall);
			xil_printf("d");
			writeFishball(dirArray);
			score += 1;
			sem_post(&updateBall);
		} 
		
		if(b->col==GOLD && to_release[(b->id)-1]==1)
		{
			sem_post(&sem_gold);
			b->col=brickColour;
		}
		if (sem_trywait(&sem_gold)==0)
		{
			b->col = GOLD;
			to_release[(b->id)-1] = 0;
		}
		
		if (!XMbox_IsFull(&Mbox) && changed) {
			XMbox_WriteBlocking(&Mbox, &b, sizeof(Brick));
			XMbox_WriteBlocking(&Mbox, &fishball, sizeof(ball));
		}
		
		
		/*sem_trywait(&sem);
		 //change To Gold
		 brick1.col = GOLD;
		 changed = 1;
		 //Send Mail
		 if (!XMbox_IsFull(&Mbox)) {
		 XMbox_WriteBlocking(&Mbox, &brick1, sizeof(brick1));
		 }
		 sleep(1000);

		 brick1.col = brickColour;
		 changed = 1;
		 sem_post(&sem);
		 //change Back

		 //Send Mail
		 if (!XMbox_IsFull(&Mbox)) {
		 XMbox_WriteBlocking(&Mbox, &brick1, sizeof(brick1));
		 }*/
		sleep(20);
}
void* brickCol_1() {
	int changed = 0;
	int dirArray[2];
	Brick brick1;
	brick1.id = 1;
	initializeBrick(&brick1);
	while (1) {
		doit(&sem_1, changed, &dirArray, &brick1);
	}
}

void* brickCol_2() {
	int changed = 0;
	int dirArray[2];
	Brick brick2;
	brick2.id = 2;
	initializeBrick(&brick2);
	while (1) {
		doit(&sem_2, changed, &dirArray, &brick2);
	}
}

void* brickCol_3() {
	int changed = 0;
	int dirArray[2];
	Brick brick3;
	brick3.id = 3;
	initializeBrick(&brick3);
	while (1) {
		doit(&sem_3, changed, &dirArray, &brick3);
	}
}

void* brickCol_4() {
	int changed = 0;
	int dirArray[2];
	Brick brick4;
	brick4.id = 4;
	initializeBrick(&brick4);
	while (1) {
		doit(&sem_4, changed, &dirArray, &brick4);
	}
}

void* brickCol_5() {
	int changed = 0;
	int dirArray[2];
	Brick brick5;
	brick5.id = 5;
	initializeBrick(&brick5);
	while (1) {
		doit(&sem_5, changed, &dirArray, &brick5);
	}
}

void* brickCol_6() {
	int changed = 0;
	int dirArray[2];
	Brick brick6;
	brick6.id = 6;
	initializeBrick(&brick6);
	while (1) {
		doit(&sem_6, changed, &dirArray, &brick6);
	}
}

void* brickCol_7() {
	int changed = 0;
	int dirArray[2];
	Brick brick7;
	brick7.id = 7;
	initializeBrick(&brick7);
	while (1) {
		doit(&sem_7, changed, &dirArray, &brick7);
	}
}

void* brickCol_8() {
	int changed = 0;
	int dirArray[2];
	Brick brick8;
	brick8.id = 8;
	initializeBrick(&brick8);
	while (1) {
		doit(&sem_8, changed, &dirArray, &brick8);
	}
}

void* brickCol_9() {
	int changed = 0;
	int dirArray[2];
	Brick brick9;
	brick9.id = 9;
	initializeBrick(&brick9);
	while (1) {
		doit(&sem_9, changed, &dirArray, &brick9);
	}
}

void* brickCol_10() {
	int changed = 0;
	int dirArray[2];
	Brick brick10;
	brick10.id = 10;
	initializeBrick(&brick10);
	while (1) {
		doit(&sem_10, changed, &dirArray, &brick10);
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
	int Status;
	XMbox_Config *ConfigPtr;
	ConfigPtr = XMbox_LookupConfig(MBOX_DEVICE_ID);
	if (ConfigPtr == (XMbox_Config*) NULL ) {
		print("--Error configuring Mbox Sender--\r\n");
		return XST_FAILURE;
	}

	Status = XMbox_CfgInitialize(&Mbox, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		print("--Error initializing Mbox Sender--\r\n");
		return XST_FAILURE;
	}

	/*if (sem_init(&sem, 1, 2) < 0) {
		print("Error while initializing semaphore sem. \r\n");
	}*/

	if (sem_init(&updateBall, 1, 1) < 0) {
		print("Error while initializing semaphore updateBall. \r\n");
	}

	if (sem_init(&sem_1, 1, 0) < 0) {
		print("Error while initializing semaphore sem_1. \r\n");
	}

	if (sem_init(&sem_2, 1, 0) < 0) {
		print("Error while initializing semaphore sem_2. \r\n");
	}

	if (sem_init(&sem_3, 1, 0) < 0) {
		print("Error while initializing semaphore sem_3. \r\n");
	}
	if (sem_init(&sem_4, 1, 0) < 0) {
		print("Error while initializing semaphore sem_4. \r\n");
	}

	if (sem_init(&sem_5, 1, 0) < 0) {
		print("Error while initializing semaphore sem_5. \r\n");
	}

	if (sem_init(&sem_6, 1, 0) < 0) {
		print("Error while initializing semaphore sem_6. \r\n");
	}

	if (sem_init(&sem_7, 1, 0) < 0) {
		print("Error while initializing semaphore sem_7. \r\n");
	}

	if (sem_init(&sem_8, 1, 0) < 0) {
		print("Error while initializing semaphore sem_8. \r\n");
	}

	if (sem_init(&sem_9, 1, 0) < 0) {
		print("Error while initializing semaphore sem_9. \r\n");
	}

	if (sem_init(&sem_10, 1, 0) < 0) {
		print("Error while initializing semaphore sem_10. \r\n");
	}
	
	if (sem_init(&sem_gold, 1, 2)<0){
		print("Error while initializing semaphore sem_gold. \r\n");
	}

	ret = pthread_create(&tid11, NULL, (void*) signalThread, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching signalThread...\r\n", ret);
	} else {
		xil_printf("Thread signalThread launched with ID %d \r\n", tid11);
	}

	ret = pthread_create(&tid1, NULL, (void*) brickCol_1, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_1...\r\n", ret);
	} else {
		xil_printf("Thread 1 launched with ID %d \r\n", tid1);
	}

	ret = pthread_create(&tid2, NULL, (void*) brickCol_2, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_2...\r\n", ret);
	} else {
		xil_printf("Thread 2 launched with ID %d \r\n", tid2);
	}

	ret = pthread_create(&tid3, NULL, (void*) brickCol_3, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_3...\r\n", ret);
	} else {
		xil_printf("Thread 3 launched with ID %d \r\n", tid3);
	}

	ret = pthread_create(&tid4, NULL, (void*) brickCol_4, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_4...\r\n", ret);
	} else {
		xil_printf("Thread 4 launched with ID %d \r\n", tid4);
	}

	ret = pthread_create(&tid5, NULL, (void*) brickCol_5, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_5...\r\n", ret);
	} else {
		xil_printf("Thread 5 launched with ID %d \r\n", tid5);
	}

	ret = pthread_create(&tid6, NULL, (void*) brickCol_6, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_6...\r\n", ret);
	} else {
		xil_printf("Thread 6 launched with ID %d \r\n", tid6);
	}

	ret = pthread_create(&tid7, NULL, (void*) brickCol_7, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_7...\r\n", ret);
	} else {
		xil_printf("Thread 7 launched with ID %d \r\n", tid7);
	}

	ret = pthread_create(&tid8, NULL, (void*) brickCol_8, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_8...\r\n", ret);
	} else {
		xil_printf("Thread 8 launched with ID %d \r\n", tid8);
	}

	ret = pthread_create(&tid9, NULL, (void*) brickCol_9, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_9...\r\n", ret);
	} else {
		xil_printf("Thread 9 launched with ID %d \r\n", tid9);
	}

	ret = pthread_create(&tid10, NULL, (void*) brickCol_10, NULL );
	if (ret != 0) {
		xil_printf("-- ERROR (%d) launching brickCol_10...\r\n", ret);
	} else {
		xil_printf("Thread 10 launched with ID %d \r\n", tid10);
	}
	return 0;
}
