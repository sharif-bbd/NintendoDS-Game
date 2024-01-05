/*
 * Template Nintendo DS
 * May 2011
 */

#include <nds.h>
#include <stdio.h>
#include <stdbool.h>
#include "start.h"
#include "battle.h"
#include "findDiff.h"
#include "skills.h"
#include "menu.h"
#include "gameover.h"
#include "instruction.h"
#include "numbers.h"

#include <maxmod9.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "aspicot.h"

#define SCREEN_WIDTH	256
#define	SCREEN_HEIGHT	192

#define	SPRITE_WIDTH	64
#define	SPRITE_HEIGHT	64

u16 *gfx;
int min, sec, msec;

void timer_ISR(){
	if(msec >= 999){
		msec = 0;
		if(sec >= 59){
			sec = 0;
			min += 1;
		}else{
			sec +=1;
		}
	}else{
		msec += 1;
	}
	return;
}

void printDigit(u16* map, int number, int x, int y){
	int i,j;

	if(number >= 0 && number < 10){
		for(i = 0; i<8;i++){
			for(j = 0; j<4; j++){
				map[(i + y)*32+j+x] = (number >= 0) ? (u16)(i*4+j)+32*number : 32;
			}

		}
	}
}

void updateChronoDisp(u16* map,int min, int sec, int msec)
{
	int x = 0, y = 0;
	int number;
	int tile, i, j;

	//Clear the map
	for(tile = 0; tile <1024; tile++){
		map[tile] = 32;
	}

	/*****MINUTES******/
	number = min;
	if(min > 59) min = number = -1;
	//First digit
	x = 0; y = 8;
	if(min>=0) number = min/10;
	printDigit(map, number, x,y);
	//Second digit
	x = 4; y = 8;
	if(min>=0) number = min %10;
	printDigit(map, number, x,y);

	/*****COLON******/
	x = 8; y = 8;
	for(i = 0; i<8;i++){
		for(j = 0; j<2; j++){
			map[(i + y)*32+j+x] = (u16)(i*4+j)+32*10+2;
		}
	}

	/*****SECONDS******/
	number = sec;
	if(sec > 59) sec = number = -1;
	//First digit
	x = 10; y = 8;
	if(sec>=0) number = sec / 10;
	printDigit(map, number, x,y);
	//Second digit
	x = 14; y = 8;
	if(sec>=0) number = sec % 10;
	printDigit(map, number, x,y);

	/*****POINT MSEC******/
	x = 18; y = 8;
	for(i = 0; i<8;i++){
		for(j = 0; j<2; j++){
			map[(i + y)*32+j+x] = (u16)(i*4+j)+32*10;
		}
	}

	/*****M.SECONDS******/
	number = msec;
	if(msec > 999) msec = number = -1;
	//First digit
	x = 20; y = 8;
	if(msec>=0) number = msec / 100;
	printDigit(map, number, x,y);

	//Second digit
	x = 24; y = 8;
	if(msec>=0) number = (msec % 100) / 10;
	printDigit(map, number, x,y);

	//Third digit
	x = 28; y = 8;
	if(msec>=0) number = (msec % 10) % 10;
	printDigit(map, number, x,y);
}
void setup_timer(){
	min = 0;
	sec = 0;
	msec = 0;
	irqSet(IRQ_TIMER0, &timer_ISR);
	TIMER_DATA(0) = TIMER_FREQ_64(1000);
	TIMER_CR(0) = TIMER_ENABLE | TIMER_DIV_64 | TIMER_IRQ_REQ;

	irqEnable(IRQ_TIMER0);
}
void configureSprites() {
	//Set up memory bank to work in sprite mode (offset since we are using VRAM A for backgrounds)
	VRAM_B_CR = VRAM_ENABLE | VRAM_B_MAIN_SPRITE_0x06400000;

	//Initialize sprite manager and the engine
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	//Allocate space for the graphic to show in the sprite
	gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32,
			SpriteColorFormat_256Color);

	//Copy data for the graphic (palette and bitmap)
	swiCopy(aspicotPal, SPRITE_PALETTE, aspicotPalLen / 2);
	swiCopy(aspicotTiles, gfx, aspicotTilesLen / 2);
}


int main(void) {

	consoleDemoInit();

	//////set music//////
	mmInitDefaultMem((mm_addr) soundbank_bin);

	//Load module
	//mmLoad(MOD_MUSIC);
	mmLoad(MOD_POKECENTER);
	//Load effect
	mmLoadEffect(SFX_RESULT);
	bool inMenu = false;

	///////Upper Image setting////////

	// Enable and configure VRAM A for MAIN
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;

	//Engine configuration in Rotoscale mode
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE;

	//Confiure background
	BGCTRL[2] = BG_MAP_BASE(0) | BgSize_B8_256x256;

	//Transfer tiles to VRAM
	swiCopy(battlePal, BG_PALETTE, battlePalLen / 2);
	swiCopy(battleBitmap, BG_GFX, battleBitmapLen / 2);


	bgTransform[2]->hdx = 1 * 256;
	bgTransform[2]->vdx = 0 * 256;
	bgTransform[2]->hdy = 0 * 256;
	bgTransform[2]->vdy = 1 * 256;
	bgTransform[2]->dx = 0 * 256;
	bgTransform[2]->dy = 0 * 256;

	///////Lower Background setting////////

	// Enable and configure VRAM C for SUB

//    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
	VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;

			//Engine configuration in tile mode
			REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;

			//Confiure background
			BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1)
					| BG_32x32;

	swiCopy(menuPal, BG_PALETTE_SUB, menuPalLen / 2);
				swiCopy(menuTiles, BG_TILE_RAM_SUB(1), menuTilesLen / 2);

				swiCopy(menuMap, BG_MAP_RAM_SUB(0), menuMapLen / 2);

//
//    //Engine configuration in tile mode
//    REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;

//
//    //Confiure background
//    BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;
//
//    // Transfer tiles to VRAM (chosen tile base)
//    swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen/2);
//    swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen/2);
//
//    swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen/2);

	/////////Configure touch screens////////


	for (;;) {
		swiWaitForVBlank();
		scanKeys();

		unsigned held = keysHeld();
		u16 keys = keysDown();

		configureSprites();
		//inMenu = false;

		VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;

		//Engine configuration in tile mode
		REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;

		//Confiure background
		BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1)
				| BG_32x32;

		// Transfer tiles to VRAM (chosen tile base)
//						    swiCopy(startPal, BG_PALETTE_SUB, startPalLen/2);
//						    swiCopy(startTiles, BG_TILE_RAM_SUB(1), startTilesLen/2);
//
//						    swiCopy(startMap, BG_MAP_RAM_SUB(0), startMapLen/2);



		if (keys & KEY_TOUCH) {
			touchPosition touch;
			touchRead(&touch);

			if ((touch.px >= 26) && (touch.px <= 229) && (touch.py >= 43) && (touch.py <= 125)) {
				//while(1){

//				    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
//
//				    //Engine configuration in tile mode
//				    REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;
//
//				    //Confiure background
//				    BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;

				// Transfer tiles to VRAM (chosen tile base)
				swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
				swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);

				swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);

				mmStart(MOD_POKECENTER, MM_PLAY_LOOP);

				if ((touch.px >= 2) && (touch.px <= 123) && (touch.py >= 28)
						&& (touch.py <= 76)) {

					int x = 90, y = 100;
					swiCopy(instructionPal, BG_PALETTE_SUB,
							instructionPalLen / 2);
					swiCopy(instructionTiles, BG_TILE_RAM_SUB(1),
							instructionTilesLen / 2);
					swiCopy(instructionMap, BG_MAP_RAM_SUB(0),
							instructionMapLen / 2);
					while (1) {
						//Read held keys
						scanKeys();
						keys = keysHeld();

						//Modify position of the sprite accordingly
						if ((keys & KEY_RIGHT)
								&& (x < (SCREEN_WIDTH - SPRITE_WIDTH)))
							x += 2;
						if ((keys & KEY_DOWN)
								&& (y < (SCREEN_HEIGHT - SPRITE_HEIGHT)))
							y += 2;
						if ((keys & KEY_LEFT) && (x > 0))
							x -= 2;
						if ((keys & KEY_UP) && (y > 0))
							y -= 2;

						oamSet(&oamMain, 	// oam handler
								0,				// Number of sprite
								x, y,			// Coordinates
								0,				// Priority
								0,				// Palette to use
								SpriteSize_32x32,			// Sprite size
								SpriteColorFormat_256Color,	// Color format
								gfx,			// Loaded graphic to display
								-1,			// Affine rotation to use (-1 none)
								false,			// Double size if rotating
								false,			// Hide this sprite
								false, false,	// Horizontal or vertical flip
								false			// Mosaic
								);
						swiWaitForVBlank();

						//Update the sprites
						oamUpdate(&oamMain);
						if (keys & KEY_X) {
							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
											swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);

											swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							break;
						}
					}
				}
				if ((touch.px >= 133) && (touch.px <= 251) && (touch.py >= 28)
						&& (touch.py <= 76)) {

					swiCopy(findDiffPal, BG_PALETTE_SUB, findDiffPalLen / 2);
					swiCopy(findDiffTiles, BG_TILE_RAM_SUB(1),
							findDiffTilesLen / 2);
					swiCopy(findDiffMap, BG_MAP_RAM_SUB(0), findDiffMapLen / 2);
					swiWaitForVBlank();
					while (1) {
						scanKeys();
						keys = keysHeld();
						if (keys & KEY_X) {
							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
											swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);
											swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							break;
						}
					}

				}
			}

			//}

		} else if (keys & KEY_Y) {

			inMenu = true;

//						VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
//
//
//						REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;
//
//
//						BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;

			swiCopy(menuPal, BG_PALETTE_SUB, menuPalLen / 2);
			swiCopy(menuTiles, BG_TILE_RAM_SUB(1), menuTilesLen / 2);

			swiCopy(menuMap, BG_MAP_RAM_SUB(0), menuMapLen / 2);

		}

		touchPosition touch;
		touchRead(&touch);
		if ((inMenu == true) && (touch.px >= 92) && (touch.px <= 163)
				&& (touch.py >= 151) && (touch.py <= 190)) {

			touchPosition touch;

			touchRead(&touch);

			//if((touch.px>= 1) && (touch.px <= 20)&&(touch.py >= 150) ){

			//								VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
			//
			//
			//								REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;
			//
			//
			//								BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;

			REG_DISPCNT = MODE_5_2D | DISPLAY_BG1_ACTIVE;
			BGCTRL[1] = BG_32x32 | BG_COLOR_256 | BG_TILE_BASE(1) | BG_MAP_BASE(0);

			swiCopy(numbersPal, BG_PALETTE, numbersPalLen/2);
			swiCopy(numbersTiles, BG_TILE_RAM(1), numbersTilesLen/2);

			updateChronoDisp(BG_MAP_RAM(0), 0,  0, 0);

			swiCopy(gameoverPal, BG_PALETTE_SUB, gameoverPalLen / 2);
			swiCopy(gameoverTiles, BG_TILE_RAM_SUB(1), gameoverTilesLen / 2);

			swiCopy(gameoverMap, BG_MAP_RAM_SUB(0), gameoverMapLen / 2);
			mmStop();
			break;
			//
		}
	}

	while (1)
		swiWaitForVBlank();

}

//up left box: x= 0~123, y= 28~76
