typedef unsigned short u16;

#define REG_DISPCTL *(unsigned short *)0x4000000			//Display controls
#define SCANLINE_COUNTER (*(unsigned short *) 0x4000006)	
		
#define MODE3 3												//Video mode (16 bit color)
#define BG2_ENABLE (1<<10) 									//Background enable
#define COLOR(r, g, b) ((r) | (g)<<5 | (b) <<10)			//Set pixel color with Blue,Green,Red order
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define CYAN COLOR(0, 31, 31)
#define MAGENTA COLOR(31, 0, 31)
#define YELLOW COLOR(31,31,0)
#define WHITE COLOR(31,31,31)
#define BLACK 0

#define OFFSET(r, c, numcols) ((r)*(numcols) + (c))			//Offset for pixel location on screen

// Buttons
#define BUTTON_A		(1<<0)		// Z
#define BUTTON_B		(1<<1)		// X
#define BUTTON_SELECT	(1<<2)		// BACKSPACE
#define BUTTON_START	(1<<3)		// ENTER
#define BUTTON_RIGHT	(1<<4)		
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP		(1<<6)
#define BUTTON_DOWN		(1<<7)
#define BUTTON_R		(1<<8)		// A
#define BUTTON_L		(1<<9)		// S

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)
#define BUTTONS *(volatile unsigned int *)0x4000130

// DMA
typedef struct
{
	const volatile void *src;
	const volatile void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x40000B0)

// Defines
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT 	(0 << 21)
#define DMA_DESTINATION_DECREMENT 	(1 << 21)
#define DMA_DESTINATION_FIXED		(2 << 21)
#define DMA_DESTINATION_RESET		(3 << 21)

#define DMA_SOURCE_INCREMENT		(0 << 23)
#define DMA_SOURCE_DECREMENT 		(1 << 23)
#define DMA_SOURCE_FIXED			(2 << 23)

#define DMA_REPEAT 					(1 << 25)

#define DMA_16						(0 << 26)
#define DMA_32						(1 << 26)

#define DMA_NOW						(0 << 28)
#define DMA_AT_VBLANK				(1 << 28)
#define DMA_AT_HBLANK				(2 << 28)
#define DMA_AT_REFRESH				(3 << 28)

#define DMA_IRQ						(1 << 30)
#define DMA_ON 						(1 << 31)



#define PANELWIDTH 		30
#define PANELHEIGHT		5
#define PANELROWSTART	150
#define PANELCOLSTART	105	
#define PANELCOLOR		RED	
#define PANELSPEED		3
#define SCREENHEIGHT 	160
#define SCREENWIDTH 	240
#define BLOCKHEIGHT		5
#define BLOCKWIDTH		20
#define MAXNUMBLOCKS 	NUMBLOCKS3
#define NUMROWBLOCKS1	3
#define NUMROWBLOCKS2	4
#define NUMROWBLOCKS3	5
#define NUMCOLBLOCKS	11
#define NUMBLOCKS1		(NUMROWBLOCKS1*NUMCOLBLOCKS)
#define NUMBLOCKS2		(NUMROWBLOCKS2*NUMCOLBLOCKS)
#define NUMBLOCKS3		(NUMROWBLOCKS3*NUMCOLBLOCKS)
#define BALLSIZE		5
#define BALLROWSTART	SCREENHEIGHT / 2
#define BALLCOLSTART	SCREENWIDTH / 2	
#define BALLROWDIR		-1
#define BALLCOLDIR		-1
#define BALLCOLOR		YELLOW	
#define LIVES			3
#define LEVELSTART		1
#define SCORESTART	    0
#define BASEPOINTS		25

typedef enum {START,GAMESETUP,GAME,GAMEOVER,WIN}GAMEMODE;
enum {FALSE,TRUE}BOOLEAN;

// Ball
typedef struct {
	int row;
	int col;
	int rowDir;
	int colDir;
	int size;
	u16 color;
} BALL;

// Breakout panel
typedef struct {
	int row;		// Row of upper corner of panel
	int col;		// Col of upper corner of panel
	int height;		// Panel.height
	int width;		// Panel.width
	u16 color;		// Color of panel
} PANEL;

// Blocks
typedef struct {
	int row;
	int col;
	int height;
	int width;
	int active;
	int points;
	u16 color;
	u16* background;
} BLOCKS;


extern unsigned short *videoBuffer;

// Prototypes
void setPixel(int row, int col, u16 color);
void drawRect(int row, int col, int height, int width, u16 color);
void drawChar(int row, int col, char ch, u16 color);
void drawString(int row, int col, char *str, u16 color);
int boundsCheck(BALL* ball,PANEL* panel,BLOCKS* blocks,int* score,int* numBlocksLeft,int numBlocks);
void drawImage3(int r,int c,int width,int height,const u16* image);
void drawScreenBackground(const u16* color);
void storeOldBackground(int r,int c,int height, 
	int width,u16 object[height*width]);
void replaceOldBackground(int oldR,int oldC,int height,int width,
	u16 object[height*width]);
void initializeBlocks(BLOCKS* blocks,BLOCKS* blocksOld,int numRowBlocks);
void drawStartScreen();
void drawGameScreen(int* lives,int* score,int* level);
void drawEndGameScreen(int score,const u16* image);
void movePanelLeft(PANEL* panel);
void movePanelRight(PANEL* panel);
void updateLivesOnScreen(int* livesPrev,int* lives,const u16* color);
void updateScoreOnScreen(int* scorePrev,int* score,const u16* color);
void freeBlocks(BLOCKS* blocks, int numBlocks);
void waitForVblank();
