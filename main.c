#include "myLib.h"
#include "breakoutStart.h"
#include "breakoutGame.h"
#include "breakoutGameOver.h"
#include "breakoutWin.h"
#include <stdio.h>
#include <stdlib.h>


int main()
{
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	u16 bgcolor = BLACK;
	GAMEMODE mode;
	mode = START;

	// BALL
	int colDir[] = {-1,1};
	int numDir = sizeof(colDir) / sizeof(colDir[0]);
	BALL ball;
	ball.size = BALLSIZE;
	ball.color = BALLCOLOR;
	u16 ballBackground[BALLSIZE*BALLSIZE];
	int oldBallRow;
	int oldBallCol;

	// PANEL
	PANEL panel;
	panel.height = PANELHEIGHT;
    panel.width = PANELWIDTH;
	panel.color = PANELCOLOR;
	u16 panelBackground[PANELHEIGHT*PANELWIDTH];
	int oldPanelRow;
	int oldPanelCol;

	// Setup of game
	int lives;
	int livesPrev;
	int score;
	int scorePrev;
	int level;

	// Blocks
	int numBlocks = NUMBLOCKS1;
	int numBlocksLeft = NUMBLOCKS1;
	int numRowBlocks = NUMROWBLOCKS1;
	BLOCKS blocks[MAXNUMBLOCKS];
	BLOCKS blocksOld[MAXNUMBLOCKS];

	// Overall game loop 
	while(1)
	{
		switch(mode)
		{
			case(START):
				numBlocks = NUMBLOCKS1;
				numBlocksLeft = NUMBLOCKS1;
				numRowBlocks = NUMROWBLOCKS1;
				lives = LIVES;
				livesPrev = LIVES;
				level = LEVELSTART;
				score = SCORESTART;
				scorePrev = score;
				ball.row = BALLROWSTART;
				ball.col = BALLCOLSTART;
				panel.row = PANELROWSTART;
				panel.col = PANELCOLSTART;
				oldBallRow = ball.row;
				oldBallCol = ball.col;
				oldPanelRow = panel.row;
				oldPanelCol = panel.col;

				// Draw Start screen
				drawStartScreen();

				// Wait for user to hit START BUTTON to begin game
				while(!KEY_DOWN_NOW(BUTTON_START)){}
				mode = GAMESETUP;
				break;

			case(GAMESETUP):
				ball.row = BALLROWSTART;
				ball.col = BALLCOLSTART;

				// Clear background to background color
				drawScreenBackground(&bgcolor);
				
				waitForVblank();

				// Draw game background
				drawGameScreen(&lives,&score,&level);
		
				// initialize blocks
				initializeBlocks(blocks,blocksOld,numRowBlocks);

				waitForVblank();
				// Store old backgrounds before moving into game
				for(int i = 0; i < numBlocks;i++)
				{
					storeOldBackground(blocks[i].row,blocks[i].col,
						blocks[i].height,blocks[i].width,blocks[i].background);
				}
				storeOldBackground(panel.row,panel.col,panel.height,
					panel.width,panelBackground);
				drawRect(panel.row,panel.col,PANELHEIGHT,PANELWIDTH,panel.color);

				storeOldBackground(ball.row,ball.col,ball.size,
					ball.size,ballBackground);
				drawRect(ball.row,ball.col,ball.size,ball.size,ball.color);

				// move ball
				ball.rowDir = BALLROWDIR;
				ball.colDir = BALLCOLDIR * colDir[rand()%numDir];

				// Draw blocks initially
				for(int i = 0; i < numBlocks; i++)
				{
					drawRect(blocks[i].row,blocks[i].col,blocks[i].height,
						blocks[i].width,blocks[i].color);
				}
				mode = GAME;
				break;

			case(GAME):
				oldBallRow = ball.row;
				oldBallCol = ball.col;
				oldPanelRow = panel.row;
				oldPanelCol = panel.col;

				// Move panel left
				if(KEY_DOWN_NOW(BUTTON_LEFT))
				{
					movePanelLeft(&panel);
				}

				// Move panel right
				if(KEY_DOWN_NOW(BUTTON_RIGHT))
				{
					movePanelRight(&panel);	
				}

				waitForVblank();
				// Only update lives on screen 
				// when there is a change
				if(livesPrev != lives)
				{
					updateLivesOnScreen(&livesPrev,&lives,&bgcolor);
				}

				// Only update score on screen
				// when there is a change
				if(scorePrev != score)
				{
					updateScoreOnScreen(&scorePrev,&score,&bgcolor);
				}
			
				// update ball direction
				ball.row = ball.row + ball.rowDir;
				ball.col = ball.col + ball.colDir;

				// ball hit the bottom screen, lose of life
				if(boundsCheck(&ball,&panel,blocks,&score,&numBlocksLeft,numBlocks))
				{
					lives--;
					replaceOldBackground(oldBallRow,oldBallCol,
						ball.size,ball.size,ballBackground);
					ball.row = BALLROWSTART;
					ball.col = BALLCOLSTART;
					ball.rowDir = BALLROWDIR;
					ball.colDir = BALLCOLDIR * colDir[rand()%numDir];;
				}

				
				
				// Take out all blocks that don't need showing
				for(int i = 0; i < numBlocks; i++)
				{
					// block is gone, replace background with original
					if(blocks[i].active == FALSE && blocksOld[i].active == TRUE)
					{
						blocksOld[i].active = FALSE;
						replaceOldBackground(blocks[i].row,
							blocks[i].col,blocks[i].height,
							blocks[i].width,blocks[i].background);
					}
				}

				// Draw old and new ball locations
				replaceOldBackground(oldBallRow,oldBallCol,
					ball.size,ball.size,ballBackground);
				storeOldBackground(ball.row,ball.col,
					ball.size,ball.size,ballBackground);
				drawRect(ball.row,ball.col,ball.size,ball.size,ball.color);

				// Draw old and new panel locations
				replaceOldBackground(oldPanelRow,oldPanelCol,
					panel.height,panel.width,panelBackground);
				storeOldBackground(panel.row,panel.col,panel.height,
					panel.width,panelBackground);
				drawRect(panel.row,panel.col,PANELHEIGHT,PANELWIDTH,panel.color);

				// exit current game
				if(KEY_DOWN_NOW(BUTTON_SELECT))
				{
					mode = START;
					freeBlocks(blocks,numBlocks);
				}
				// game over condition
				if(lives == 0) 
				{
					mode = GAMEOVER;
				}
				// You win or next level
				if(numBlocksLeft == 0)
				{
					if(level == 1)
					{
						level++;
						freeBlocks(blocks,numBlocks);
						numBlocks = NUMBLOCKS2;
						numBlocksLeft = NUMBLOCKS2;
						numRowBlocks = NUMROWBLOCKS2;
						mode = GAMESETUP;
					}else if(level == 2)
					{
						level++;
						freeBlocks(blocks,numBlocks);
						numBlocks = NUMBLOCKS3;
						numBlocksLeft = NUMBLOCKS3;
						numRowBlocks = NUMROWBLOCKS3;
						mode = GAMESETUP;
					}else
					{
						level = LEVELSTART;
						mode = WIN;
					}
				}
				break;

			case(GAMEOVER):
				drawEndGameScreen(score,breakoutGameOver);
				mode = START;
				freeBlocks(blocks,numBlocks);
				while(!KEY_DOWN_NOW(BUTTON_SELECT)){}
				break;

			case(WIN):
				drawEndGameScreen(score,breakoutWin);
				mode = START;
				freeBlocks(blocks,numBlocks);
				while(!KEY_DOWN_NOW(BUTTON_SELECT)){};
				break;
		}
	}
}


