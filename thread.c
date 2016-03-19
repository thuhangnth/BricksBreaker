#include "xmk.h"
#include <pthread.h>
#include <errno.h>
#include "xmutex.h"
#include "drawScreen.h"


pthread_t tid1,tid2,tid3,tid4,tid5,tid6,tid7,tid8,tid9,tid10;

#define brickColour 0x00a52a2a
#define bgColour 0x0000b200

#define MUTEX_DEVICE_ID XPAR_MUTEX_0_IF_1_DEVICE_ID
#define MUTEX_NUM 0
XMutex Mutex;


void* brickCol_1 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(1, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
		{
			;
		}
	}
}

void* brickCol_2 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(2, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_3 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(3, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_4 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(4, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_5 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(5, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_6 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(6, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_7 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(7, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_8 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(8, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_9 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(9, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
	}
}

void* brickCol_10 () {
	int i;
	XMutex_Lock(&Mutex, MUTEX_NUM);
	drawBrickCol(10, 8, brickColour, bgColour);
	XMutex_Unlock(&Mutex, MUTEX_NUM);
	while(1)
	{
		for(i=0; i<20; i++)
				{
					;
				}
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
	XStatus Status;

	XMutex_Config*ConfigPtr;
	ConfigPtr = XMutex_LookupConfig(MUTEX_DEVICE_ID);
	if (ConfigPtr == (XMutex_Config *)NULL){

		xil_printf("B1-- ERROR  init HW mutex...\r\n");
	}

	Status = XMutex_CfgInitialize(&Mutex, ConfigPtr, ConfigPtr->BaseAddress);

	initTFT();
	startScreen(bgColour, brickColour, brickColour);
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
