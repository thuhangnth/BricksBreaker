#include "drawScreen.h"
#include <math.h>
int initTFT()
{
	int Status;
	XTft_Config *TftConfigPtr;

	/*
	 * Get address of the XTft_Config structure for the given device id.
	 */
	TftConfigPtr = XTft_LookupConfig(TFT_DEVICE_ID);
	if (TftConfigPtr == (XTft_Config *)NULL) 
	{
		return XST_FAILURE;
	}

	/*
	 * Initialize all the TftInstance members and fills the screen with
	 * default background color.
	 */
	Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr,
			TftConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) 
	{
		return XST_FAILURE;
	}

	/*
	 * Wait till Vsync(Video address latch) status bit is set before writing
	 * the frame address into the Address Register. This ensures that the
	 * current frame has been displayed and we can display a new frame of
	 * data. Checking the Vsync state ensures that there is no data flicker
	 * when displaying frames in real time though there is some delay due to
	 * polling.
	 */
	while (XTft_GetVsyncStatus(&TftInstance) !=
			XTFT_IESR_VADDRLATCH_STATUS_MASK);

	/*
	 * Change the Video Memory Base Address from default value to
	 * a valid Memory Address and clear the screen.
	 */
	XTft_SetFrameBaseAddr(&TftInstance, TFT_FRAME_ADDR);
	XTft_ClearScreen(&TftInstance);

	XTft_SetPos(&TftInstance, 0,0);
	XTft_SetPosChar(&TftInstance, 0,0);

	XTft_SetColor(&TftInstance, 0x00000000, 0x00ffffff);

	XTft_FillScreen(&TftInstance, 0, 0,639,479,0x00ffffff); // white

	return XST_SUCCESS;
}

void drawHorLine(int x0, int x1, int y, int col)
{
	int i = 0;
	for (i=x0;i<=x1;i++)
	{
		XTft_SetPixel(&TftInstance, i, y, col);
	}
}

void drawRect(int x, int y, int w, int l, int col)
{
	int i = 0;
	int x1 = x;
	int x2 = x+l-1;
	for (i=y;i<y+w;i++)
	{
		drawHorLine(x,x2,i,col);
	}
}

void drawBrickCol(int columnIndex, int brickRemain, int colBrick, int colBG)
{
	int x = 64 + (BRICK_LENGTH+SPACE)*columnIndex;
	int i;
	for (i=0;i<=brickRemain;i++)
	{
		y = 64+(BRICK_WIDHT+SPACE)*i;
		if (i<brickRemain)
		{
			drawRect(x,y,BRICK_WIDTH,BRICK_LENGTH,colBrick);
		}
		else
		{
			drawRect(x,y,BRICK_WIDTH,BRICK_LENGTH,colBG);
		}
	}
}

void drawCircle(int x0, int y0, int r, int col)
{
	int x = r;
	int y =0;
	int re, yChange, xChange, dec;
	drawHorLine(x0-r, x0+r, y0, col);
	while (y<=x)
	{
		y++;
		re =  pow(x,2) + pow(y,2) - pow(r,2);
		yChange = 2*y+1;
		xChange = 1-2*x;
		dec = 2*(re+yChange)+xChange;
		if(dec>0)
		{
			x -= 1;
		}
		drawHorLine(x0-x, x0+x, y0+y, col);
		drawHorLine(x0-x, x0+x, y0-y, col);
		drawHorLine(x0-y, x0+y, y0+x, col);
		drawHorLine(x0-y, x0+y, y0-x, col);
	}
}

void startScreen(int colBG, int colBar, int colBall)
{
	drawRect(59,59,455,480,colBG);
	drawRect(246,405,5,80,colBar);
	drawCircle(246,398,7,colBall);
}