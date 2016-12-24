#include "myLib.h"
#include "ball.h"
//Kalya Subramanian
unsigned short *videoBuffer = (unsigned short *)0x6000000;


//Kalya Subramanian
void drawImage3(int r, int c, int width, int height, const u16* image)
{
	for(int x = 0; x <height; x++) 
	{
		REG_DMA3SAD = (u32)(&(image[x * width]));
		REG_DMA3DAD = (u32)(&videoBuffer[OFFSET(r + x, c, 240)]);
		REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_INCREMENT;
	}
}

void setPixel(int row, int col, unsigned short color)
{
	videoBuffer[OFFSET(row,col, 240)] = color;
}

void drawRect(int row, int col, int width, int height, volatile unsigned short color)
{
	for(int r=0; r<height; r++)
	{
		
		REG_DMA3SAD = (u32)&color;
		REG_DMA3DAD = (u32)(&videoBuffer[OFFSET(row+r, col, 240)]);
		REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void resetBall(BALL *x, PADDLE *y)
{
	BALL x1 = *x;
	PADDLE y1 = *y;
	x1.row =  y1.row - BALL_HEIGHT - 1;
	x1.col =  240/2 - BALL_WIDTH/2;
	x1.width = BALL_WIDTH;
	x1.height = BALL_HEIGHT;
	x1.rd = 0;
	x1.cd = 0;
}



void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

