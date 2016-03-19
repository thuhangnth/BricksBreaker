/***************************** Include Files ********************************/

#include "xtft.h"
#include "xparameters.h"
#include <string.h>

/************************** Constant Definitions ****************************/
/**
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define TFT_DEVICE_ID    XPAR_TFT_0_DEVICE_ID
#define DDR_HIGH_ADDR    XPAR_PS7_DDR_0_S_AXI_HIGHADDR

#ifndef DDR_HIGH_ADDR
#warning "CHECK FOR THE VALID DDR ADDRESS IN XPARAMETERS.H"
#endif

#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480


/**
 * User has to specify a 2MB memory space for filling the frame data.
 * This constant has to be updated based on the memory map of the
 * system.
 */
#define TFT_FRAME_ADDR        0x10000000

/**************************** Type Definitions ******************************/

/************************** Function Prototypes *****************************/

int initTFT();
void drawBrickCol(int columnIndex, int brickRemain, int colBrick, int colBG); //columnIndex = 0..9
void drawCircle(int x0, int y0, int r, int col); //(x0,y0) is the centre, r is radius
void drawRect(int x, int y, int w, int l, int col); //(x,y) is the top left corner of the rectangle
void drawHorLine(int x1, int x2, int y, int col);
void startScreen(int colBG, int colBar, int colBall);


/************************** Variable Definitions ****************************/

#define MAX_ROW  8
#define BRICK_WIDTH  5
#define BRICK_LENGTH  40
#define SPACE  5
static XTft TftInstance;
