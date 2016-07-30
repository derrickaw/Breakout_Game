#include "myLib.h"
#include "font.h"
#include "breakoutStart.h"
#include "breakoutGame.h"
#include "breakoutGameOver.h"
#include "breakoutWin.h"
#include <stdio.h>
#include <stdlib.h>


unsigned short *videoBuffer = (unsigned short *)0x6000000;

/* setPixel
 * Sets pixel at offset in videoBuffer
 * @param row - row in videobuffer to set pixel color at
 * @param col - col in videobuffer to set pixel color at
 * @param color - pixel color to set
 */
void setPixel(int row, int col, u16 color)
{
	videoBuffer[OFFSET(row, col, SCREENWIDTH)] = color;
}

/* getPixel
 * Gets pixel at offset in videobuffer
 * @param row - row in videobuffer to pull pixel color from
 * @param col - col in videobuffer to pull pixel color from
 * return - returns color of pixel in u16
 */
u16 getPixel(int row, int col)
{
	return videoBuffer[OFFSET(row,col,SCREENWIDTH)];
}

/* drawRect
 * Draws a filled rectangle with the specified color
 * @param row - starting row position in videobuffer to draw rect
 * @param col - starting col position in videobuffer to draw rect
 * @param height - rectangle height to draw
 * @param width - rectangle width to draw
 * @param color - color of rectangle to draw
 */
void drawRect(int row, int col, int height, int width, u16 color)
{
	for(int r=0;r<height;r++)
	{
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(row+r,col,240)];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

/* drawChar
 * Draws a character on the screen
 * @param row - starting row to draw character
 * @param col - starting col to draw character
 * @param ch - character to draw
 * @param color - character color to draw
 */
void drawChar(int row, int col, char ch, u16 color)
{
	int r,c;
	for(r=0; r<8; r++)
	{
		for(c=0; c<6; c++)
		{
			if(fontdata_6x8[OFFSET(r,c,6) + 48*ch])
			{
				setPixel(r + row, c + col, color);
			}	
		}
	}
}

/* drawString
 * Draws a string on the screen
 * @param row - starting row to draw string on
 * @param col - starting col to draw string on
 * @param str - string to write to screen
 * @param color - string color
 */
void drawString(int row, int col, char *str, u16 color)
{
	int chars_drawn = 0;
	while(*str)
	{
		drawChar(row, col + 6*chars_drawn++, *str++, color);
	}
}

/* boundsCheck
 * Checks boundaries to correctly bounce ball off objects
 * @param ball - ball that is bouncing on screen
 * @param panel - panel that is used to get ball active
 * @param blocks - blocks on screen
 * @param score - current score in game
 * @param numBlocksLeft - number of blocks left on screen
 * return - returns 0 if ball did not hit bottom screen, return 1 if it did
 */
int boundsCheck(BALL* ball,PANEL* panel,BLOCKS* blocks,int* score,int* numBlocksLeft,int numBlocks)
{
	// Top screen check
	if(ball->row < 10)
	{
		ball->row = 10;
		ball->rowDir = -ball->rowDir;
	}
	// Bottom check - about to go off screen and lose
	if(ball->row > SCREENHEIGHT - ball->size + 1) 
	{
		return 1;
	}
	// Left screen check
	if(ball->col < 1)
	{
		ball->col = 0;
		ball->colDir = -ball->colDir;
	}
	// Right screen check 
	if(ball->col + ball->size + 1 > SCREENWIDTH)
	{
		ball->col =  SCREENWIDTH - ball->size - 1;
		ball->colDir = -ball->colDir;
	}
	// Panel check
	if(ball->row + ball->size - 1 >= panel->row
		&& ball->row <= panel->row + panel->height - 1
		&& ball->col + ball->size - 1 >= panel->col
		&& ball->col <= panel->col + panel->width - 1)
	{
		ball->row = panel->row - ball->size;
		ball->rowDir = -ball->rowDir;
	}
	// blocks check
	for(int i = 0; i < numBlocks; i++)
	{
		if((blocks+i)->active == TRUE)
		{
			if(ball->row + ball->size - 1 >= (blocks+i)->row
				&& ball->row <= (blocks+i)->row + (blocks+i)->height - 1
				&& ball->col + ball->size - 1 >= (blocks+i)->col
				&& ball->col <= (blocks+i)->col + (blocks+i)->width - 1)
			{
				//ball->row = (blocks+i)->row + (blocks+i)->height;
				ball->rowDir = -ball->rowDir;
				(blocks+i)->active = FALSE;
				*score += (blocks+i)->points;
				(*numBlocksLeft)--;
			}
		}
	}
	return 0;
}


/* drawImage3
 * A function that will draw an arbitrary sized image
 * onto the screen (with DMA).
 * @param r - row to draw the image
 * @param c - column to draw the image
 * @param width - width of the image
 * @param height - height of the image
 * @param image - pointer to the first element of the image.
 */
void drawImage3(int r,int c,int width,int height,const u16* image)
{
	for(int i = 0; i < height; i++)
	{
		DMA[3].src = image + (width*i);
		DMA[3].dst = videoBuffer + (SCREENWIDTH*r) + (SCREENWIDTH*i) + c;
		DMA[3].cnt = width | DMA_ON;
	}
}


/* drawScreenBackground
 * A function that will draw an arbitrary sized background
 * onto the screen (with DMA).
 * @param color - pointer to the color background.
 */
void drawScreenBackground(const u16* color)
{
	for(int i = 0; i < SCREENHEIGHT; i++)
	{
		DMA[3].src = color;
		DMA[3].dst = videoBuffer + (SCREENWIDTH*i);
		DMA[3].cnt = SCREENWIDTH | DMA_ON | DMA_SOURCE_FIXED;
	}
}

/* storeOldBackground
 * Stores old background in array to use later for fixing game background
 * @param r - row starting location on screen to store pixel data
 * @param c - col starting location on screen to store pixel data
 * @param height - height of background to store
 * @param width - width of background to store
 * @param object - object to use to store background from screen
 */
void storeOldBackground(int r,int c,int height, 
	int width,u16 object[height*width])
{
	for(int i = 0;i < height;i++)
	{
		DMA[3].src = &videoBuffer[OFFSET(i+r,c,SCREENWIDTH)];
		DMA[3].dst = &object[OFFSET(i,0,width)];
		DMA[3].cnt = width | DMA_ON;
	}
}

/* replaceOldBackground
 * Replaces old background on screen
 * @param oldR - starting row location to draw old background
 * @param oldC - starting col location to draw old background
 * @param height - height of old background to draw
 * @param width - width of old background to draw
 * @param object - pixel object to use to draw to screen
 */
void replaceOldBackground(int oldR,int oldC,int height,int width,
	u16 object[height*width])
{
	for(int i = 0; i < height; i++)
	{
		DMA[3].src = &object[OFFSET(i,0,width)];
		DMA[3].dst = &videoBuffer[OFFSET(i+oldR,oldC,SCREENWIDTH)];
		DMA[3].cnt = width | DMA_ON;
	}
}

/* initializeBlocks
 * Initializes blocks on screen
 * @param blocks - blocks to initialize
 * @param blocksOld - old blocks as a storage vessel for flagging
 */
void initializeBlocks(BLOCKS* blocks,BLOCKS* blocksOld,int numRowBlocks)
{
	for(int i = 0; i < numRowBlocks; i++)
	{
		for(int j = 0; j < NUMCOLBLOCKS; j++)
		{
			int spot = i*NUMCOLBLOCKS + j;
			blocks[spot].row = (i+5) * 5;
			blocks[spot].col = 20 * j + 10;
			blocks[spot].height = BLOCKHEIGHT;
			blocks[spot].width = BLOCKWIDTH;
			blocks[spot].active = TRUE;
			blocks[spot].points = BASEPOINTS * (numRowBlocks-i);
			if(i == 0)
			{
				blocks[spot].color = GREEN;
			} else if(i == 1)
			{
				blocks[spot].color = YELLOW;
			} else if(i == 2)
			{
				blocks[spot].color = CYAN;
			} else if(i == 3)
			{
				blocks[spot].color = MAGENTA;
			} else
			{
				blocks[spot].color = RED;
			}
			blocks[spot].background = malloc(BLOCKHEIGHT*BLOCKWIDTH*sizeof(u16));
			blocksOld[spot] = blocks[spot];
		}
	}
}

/* drawStartScreen
 * Draws start screen
 */
void drawStartScreen()
{
	char buffer[80];
	// Draw start screen
	drawImage3(0,0,SCREENWIDTH,SCREENHEIGHT,breakoutStart);
	sprintf(buffer,"PRESS START");
	drawString(40,20,buffer,RED);
	sprintf(buffer,"TO BEGIN");
	drawString(40,160,buffer,RED);
}

/* drawGameScreen
 * Draws game screen
 * @param lives - current lives
 * @param score - current score
 * @param level - current level
 */
void drawGameScreen(int* lives,int* score,int* level)
{
	char buffer[80];
	// Lives left in game
	sprintf(buffer,"Lives:%i",*lives);
	drawString(0,0,buffer,RED);
	// Score in game
	sprintf(buffer,"Score:%i",*score);
	drawString(0,88,buffer,RED);
	// Level # at top of screen
	sprintf(buffer,"Level:%i",*level);
	drawString(0,192,buffer,RED);

	// Draw game background
	drawImage3(10,1,238,150,breakoutGame);
}

/* drawEndGameScreen
 * Draws end game screen
 * @param score - current score
 * @param image - image to display
 */
void drawEndGameScreen(int score,const u16* image)
{
	char buffer[80];
	// Draw game over or win screen
	drawImage3(0,0,SCREENWIDTH,SCREENHEIGHT,image);
	// Final Score in game
	sprintf(buffer,"Final Score:%i",score);
	drawString(0,80,buffer,RED);
	// Draw instructions to begin play again
	sprintf(buffer,"PRESS SELECT TO PLAY AGAIN");
	drawString(120,40,buffer,RED);
}

/* movePanelLeft
 * Move panel to the left
 * @param panel - panel object
 */
void movePanelLeft(PANEL* panel)
{
	// Move panel left
	panel->col -= PANELSPEED;	
	if(panel->col < 1) 
	{
		panel->col = 1;
	}
}

/* movePanelRight
 * Move panel to the right
 * @param panel - panel object
 */
void movePanelRight(PANEL* panel)
{
	// Move panel right
	panel->col += PANELSPEED;
	if(panel->col+panel->width > 239)
	{
		panel->col = 239 - panel->width;
	}
}

/* updateLivesOnScreen
 * Update the number of lives on the screen
 * @param livesPrev - previous lives shown on screen
 * @param lives - current lives to be shown on screen
 * @param color - character color
 */
void updateLivesOnScreen(int* livesPrev,int* lives,const u16* color)
{
	char buffer[80];
	// Lives left in game
	sprintf(buffer,"%i",*livesPrev);
	drawString(0,36,buffer,*color);
	sprintf(buffer,"%i",*lives);
	drawString(0,36,buffer,RED);
	*livesPrev = *lives;
}

/* updateScoreOnScreen
 * Update score on screen
 * @param scorePrev - previous score shown on screen
 * @param score - current score to be shown on screen
 * @param color - character color
 */
void updateScoreOnScreen(int* scorePrev,int* score,const u16* color)
{
	char buffer[80];
	// Score in game
	sprintf(buffer,"%i",*scorePrev);
	drawString(0,124,buffer,*color);
	sprintf(buffer,"%i",*score);
	drawString(0,124,buffer,RED);
	*scorePrev = *score;
}

/* freeBlocks
 * Free memory used for block background
 * @param blocks - blocks to free
 */
void freeBlocks(BLOCKS* blocks, int numBlocks)
{
	for(int i = 0; i<numBlocks;i++)
	{
		free(blocks[i].background);
	}
}

/* waitForVblank()
 * Wait for the Vblank to draw on screen next
 */
void waitForVblank()
{
	while(SCANLINE_COUNTER > SCREENHEIGHT);
	while(SCANLINE_COUNTER < SCREENHEIGHT);
}
