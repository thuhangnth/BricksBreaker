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

void drawBrickCol(int columnIndex, int brick[], int colBrick, int colBG)
{
	int x = 65 + (BRICK_LENGTH+SPACE)*(columnIndex-1); //Index starts from 1
	int i, y;
	for (i=0;i<8;i++)
	{
		y = 65+(BRICK_WIDTH+SPACE)*i;
		if (brick[i] != 0)
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

void writeString(char arr[], int x, int y)
{
	XTft_SetPosChar(&TftInstance, x,y);
	int i;
	for(i=0; i<strlen(arr); i++)
	{
		XTft_Write(&TftInstance,arr[i]);
	}
}

void startScreen(int colBG, int colBrick)
{
	drawRect(60,60,360,455,colBG);
	int brick[]={1,1,1,1,1,1,1,1};
	int i;
	for(i=1;i<=10;i++)
	{
		drawBrickCol(i,&brick,colBrick,colBG);
	}
	writeString("SCORE:", TEXT_X, TEXT_YSTART); writeString("000", TEXT_X+5, TEXT_YSTART+15);
	writeString("TIME:", TEXT_X, TEXT_YSTART+70); writeString("00:00", TEXT_X+5, TEXT_YSTART +85);
	writeString("BALL SPEED:", TEXT_X, TEXT_YSTART+140); writeString("000", TEXT_X+5, TEXT_YSTART +155);
	writeString("BRICKS LEFT:", TEXT_X, TEXT_YSTART+210); writeString("080", TEXT_X+5, TEXT_YSTART +225);
	writeString("FPS:", TEXT_X, TEXT_YSTART+280); writeString("00", TEXT_X+5, TEXT_YSTART + 295);
}

void toString(int number, char string[], int digit)
{
	int x;
	if(digit == 3)
	{
		string[3] = 0;   //end string
		x = number/100;     //1st digit
		string[0] = x+48;
		x = (number/10)%10; //2nd digit
		string[1] = x+48;
		x = number%10;      //3rd digit
		string[2] = x+48;
	}
	else if(digit ==2)
	{
		string[2] = 0;  //end string
		x=number/10;        //1st digit
		string[0] = x+48;
		x=number%10;  		//2nd digit
		string[1] = x+48;
	}
}
void writeScore(int score)
{
	char string[4];
	toString(score, string,3);
	writeString(string, TEXT_X+5, TEXT_YSTART+15);
}

void writeSpeed(int speed)
{
	char string[4];
	toString(speed, string,3);
	writeString(string, TEXT_X+5, TEXT_YSTART+155);
}

void writeBrickNo(int brickNo)
{
	char string[4];
	toString(brickNo, string,3);
	writeString(string, TEXT_X+5, TEXT_YSTART+225);
}
void writeTime(int minute, int second)
{
	char string[3];
	toString(minute, string,2);
	writeString(string, TEXT_X+5, TEXT_YSTART+85);
	writeString(":", TEXT_X+23, TEXT_YSTART+85);
	toString(second, string,2);
	writeString(string, TEXT_X+30, TEXT_YSTART+85);
}
void writeFPS(int frames)
{
	char string[3];
	toString(frames,string,2);
	writeString(string, TEXT_X+5, TEXT_YSTART+295);
}
