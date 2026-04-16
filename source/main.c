/*
 * Template Nintendo DS
 * May 2011
 */

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "start.h"
#include "battle.h"


#include "select.h"
#include "findDiff.h"
#include "maths.h"
#include "maze.h"
#include "skills.h"
#include "menu.h"
#include "pedia.h"
#include "gameover.h"
#include "instruction.h"
#include "numbers.h"




#include <maxmod9.h>
#include "soundbank.h"
#include "soundbank_bin.h"

#include "pika.h"
#include "aspicot.h"
#include "piplup.h"
#include "arrow.h"
#include "ditto.h"
#include "mazeball.h"
#include "newball.h"
#include <sys/dir.h>
#include <fat.h>
#include <dirent.h>


#define SCREEN_WIDTH	256
#define	SCREEN_HEIGHT	192

#define	SPRITE_WIDTH	64
#define	SPRITE_HEIGHT	64

#define MAX(a,b) (a>b) ? a : b;

u16 *gfx;

int min, sec, msec;
int cd_min, cd_sec, cd_msec;
int max = 47;
int curr_hp;
int selectedPokemon;
int start = 22;
int music_playing = -1;
int best_min = 59;
int best_sec = 59;
int best_msec = 999;

u8 greenTile[64]=
{
		255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255,
		255,255,255,255,255,255,255,255
};

void end_game();

void read_bestScore(){
	FILE* file = fopen("/flepokemonRecord.txt","r");
	if(file!= NULL){
		fscanf(file, "%i\n %i\n %i", &best_min, &best_sec, &best_msec);
		fclose(file);
	}
}

void write_bestScore(){
	FILE* file = fopen("/flepokemonRecord.txt","w+");
	if(file != NULL){
		fprintf(file, "%i\n %i\n %i", best_min, best_sec, best_msec);
		fclose(file);
	}
}

void update_score(){
	if((min <= best_min) && (sec <= best_sec) && (msec < best_msec)){
		best_min = min;
		best_sec = sec;
		best_msec = msec;
		write_bestScore();
	}
}


void keys_ISR(){
	if(music_playing == -1) return;
	u16 keys = ~(REG_KEYINPUT);
	irqDisable(IRQ_KEYS);
	if(keys & KEY_B){
		if(music_playing == 1){
			mmPause();
			music_playing = 0;
		}else if(music_playing == 0){
			mmResume();
			music_playing = 1;
		}
	}
	irqEnable(IRQ_KEYS);

}
void set_pause_button(){
	REG_KEYCNT = KEY_B | 1 << 14;
	irqSet(IRQ_KEYS, &keys_ISR);
	irqEnable(IRQ_KEYS);
	irqEnable(IRQ_VBLANK);
}

void reduce_health(double fraction){
	if(curr_hp > start){

		double full_hp = (double)(max - start);
		int hp_left = MAX(curr_hp -floor(fraction * full_hp), start);

		for(int i = hp_left; i <= max; ++i){
			BG_MAP_RAM(1)[(35*128) + i] = 0x0000;
			BG_MAP_RAM(1)[(36*128) + i] = 0x0000;
			BG_MAP_RAM(1)[(37*128) + i] = 0x0000;
		}
		curr_hp = hp_left;

		if(curr_hp <= ceil(full_hp/4) + start){
			BG_PALETTE[255] = ARGB16(1, 28, 0, 0);
			for(int i = start; i < curr_hp; ++i){
				BG_MAP_RAM(1)[(35*128) + i] = 0xFFFF;
				BG_MAP_RAM(1)[(37*128) + i] = 0xFFFF;
				BG_MAP_RAM(1)[(36*128) + i] = 0xFFFF;
			}
		}else if(curr_hp <= ceil(full_hp/2) + start){
			BG_PALETTE[255] = ARGB16(1, 26, 26, 0);
			for(int i = start; i < curr_hp; ++i){
				BG_MAP_RAM(1)[(35*128) + i] = 0xFFFF;
				BG_MAP_RAM(1)[(37*128) + i] = 0xFFFF;
				BG_MAP_RAM(1)[(36*128) + i] = 0xFFFF;
			}
		}
	}
	if(curr_hp <= start){
		end_game();
	}

}


void countdown_ISR(){
	if(msec <= 0){
		msec = 999;
		if(sec <= 0){
			sec = 59;
			if(min > 0){
				min -= 1;
			}
		}else{
			if(sec > 0){
				sec -= 1;
			}
		}
	}else{
		if(msec > 0){
			msec -=1;
		}
	}
}

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
		for(i = 0; i<4;i++){
			for(j = 0; j<2; j++){
				map[(i + y)*32+j+x] = (number >= 0) ? (u16)(i*2+j)+8*number : 87;
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
		map[tile] = 87;
	}

	/*****MINUTES******/
	number = min;
	if(min > 59) min = number = -1;
	//First digit
	x = 6; y = 12;
	if(min>=0) number = min/10;
	printDigit(map, number, x,y);
	//Second digit
	x = 8; y = 12;
	if(min>=0) number = min %10;
	printDigit(map, number, x,y);

	/*****COLON******/

	x = 11; y = 12;
	for(i = 0; i<4;i++){
		for(j = 0; j<1; j++){
			map[(i + y)*32+j+x] = (u16)(i*2+j)+16*5+1;
		}
	}


	/*****SECONDS******/
	number = sec;
	if(sec > 59) sec = number = -1;
	//First digit
	x = 12; y = 12;
	if(sec>=0) number = sec / 10;
	printDigit(map, number, x,y);
	//Second digit
	x = 14; y = 12;
	if(sec>=0) number = sec % 10;
	printDigit(map, number, x,y);

	/*****POINT MSEC******/

	x = 17; y = 12;
	for(i = 0; i<4;i++){
		for(j = 0; j<1; j++){
			map[(i + y)*32+j+x] = (u16)(i*2+j)+16*5;
		}
	}

	/*****M.SECONDS******/
	number = msec;
	if(msec > 999) msec = number = -1;
	//First digit
	x = 18; y = 12;
	if(msec>=0) number = msec / 100;
	printDigit(map, number, x,y);

	//Second digit
	x = 20; y = 12;
	if(msec>=0) number = (msec % 100) / 10;
	printDigit(map, number, x,y);

	//Third digit
	x = 22; y = 12;
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

void setup_countdown(){
	cd_min = 0;
	cd_sec = 10;
	cd_msec = 0;
	irqSet(IRQ_TIMER1, &countdown_ISR);
	TIMER_DATA(1) = TIMER_FREQ_64(1000);
	TIMER_CR(1) = TIMER_ENABLE | TIMER_DIV_64 | TIMER_IRQ_REQ ;

	irqEnable(IRQ_TIMER1);
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
	if(selectedPokemon==1){
		swiCopy(pikaPal, SPRITE_PALETTE, pikaPalLen / 2);
			swiCopy(pikaTiles, gfx, pikaTilesLen / 2);
	}else if(selectedPokemon==2){
	swiCopy(aspicotPal, SPRITE_PALETTE, aspicotPalLen / 2);
	swiCopy(aspicotTiles, gfx, aspicotTilesLen / 2);
	}else{
		swiCopy(piplupPal, SPRITE_PALETTE, piplupPalLen / 2);
			swiCopy(piplupTiles, gfx, piplupTilesLen / 2);
	}
}

void configureSelectSprites() {
	//Set up memory bank to work in sprite mode (offset since we are using VRAM A for backgrounds)
	VRAM_B_CR = VRAM_ENABLE | VRAM_B_MAIN_SPRITE_0x06400000;

	//Initialize sprite manager and the engine
	oamInit(&oamMain, SpriteMapping_1D_32, false);

	//Allocate space for the graphic to show in the sprite
	gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32,
			SpriteColorFormat_256Color);

	//Copy data for the graphic (palette and bitmap)
	swiCopy(arrowPal, SPRITE_PALETTE, arrowPalLen / 2);
	swiCopy(arrowTiles, gfx, arrowTilesLen / 2);
}






void configureMazeSprite() {
	//Set up memory bank to work in sprite mode (offset since we are using VRAM A for backgrounds)
	VRAM_D_CR = VRAM_ENABLE | VRAM_D_SUB_SPRITE;
	//VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;


	//Initialize sprite manager and the engine
	oamInit(&oamSub, SpriteMapping_1D_32, false);

	//Allocate space for the graphic to show in the sprite
	gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16,
			SpriteColorFormat_256Color);

	//Copy data for the graphic (palette and bitmap)

		swiCopy(newballPal, SPRITE_PALETTE, newballPalLen / 2);
		swiCopy(newballTiles, gfx, newballTilesLen / 2);
}

bool isWall(int x, int y){
	bool wall = false;

	//horizontal walls

	if((y>4)&&(y<8)){
		if (((x>7)&&(x<101)) || ((x>126)&&(x<251))){
			wall = true;
		}
	}
	if((y>26)&&(y<30)){
		if (((x>7)&&(x<42)) || ((x>97)&&(x<163))){
				wall = true;
			}
		}
	if((y>49)&&(y<53)){
		if (((x>36)&&(x<72)) || ((x>127)&&(x<250))){
					wall = true;
				}
			}
	if((y>72)&&(y<76)){
		if (((x>7)&&(x<42)) || ((x>67)&&(x<132))){
			wall = true;
		}
	}
	if((y>94)&&(y<98)){
		if (((x>36)&&(x<132)) || ((x>156)&&(x<222))){
			wall = true;
		}
	}
	if((y>117)&&(y<121)){
		if (((x>36)&&(x<71)) || ((x>127)&&(x<190))){
			wall = true;
		}
	}
	if((y>140)&&(y<144)){
		if (((x>67)&&(x<191)) || ((x>216)&&(x<250))){
			wall = true;
		}
	}
	if((y>163)&&(y<167)){
			if (((x>37)&&(x<101)) || ((x>127)&&(x<161)) || ((x>186)&&(x<222))){
				wall = true;
			}
		}
	if((y>184)&&(y<188)){
			if (((x>7)&&(x<132)) || ((x>157)&&(x<251))){
				wall = true;
			}
		}
//	//vertical walls
	if(x<12){
		wall= true;
	}
	if((x>67)&&(x<72)){
		if (((y>26)&&(y<76)) || ((y>95)&&(y<117))){
				wall = true;
			}
		}

	if((x>=98)&&(x<101)){
		if (((y>4)&&(y<53)) || ((y>119)&&(y<144)) || ((y>164)&&(y<188))){
				wall = true;
			}
		}
	if((x>37)&&(x<42)){
		if ((y>103)&&(y<166)){
				wall = true;
			}
		}
	if((x>127)&&(x<132)){
		if (((y>49)&&(y<76)) || ((y>95)&&(y<121)) || ((y>140)&&(y<167))){
				wall = true;
			}
		}
	if((x>156)&&(x<161)){
		if (((y>71)&&(y<99)) || ((y>163)&&(y<189))){
				wall = true;
			}
		}
	if((x>187)&&(x<192)){
		if (((y>4)&&(y<32)) || ((y>49)&&(y<76)) || ((y>95)&&(y<121)) || ((y>140)&&(y<167))){
				wall = true;
			}
		}
	if((x>216)&&(x<221)){
		if (((y>26)&&(y<54)) || ((y>71)&&(y<99)) || ((y>117)&&(y<144))){
				wall = true;
			}
		}
	if((x>246)){
				wall = true;

		}




	return wall;
}








int main(void) {

	consoleDemoInit();

	set_pause_button();
	fatInitDefault();

	//////set music//////
	mmInitDefaultMem((mm_addr) soundbank_bin);

	//Load module
	//mmLoad(MOD_MUSIC);
	mmLoad(MOD_POKECENTER);
	//Load effect
	mmLoadEffect(SFX_RESULT);
	mmLoadEffect(SFX_RUN);
	mmLoadEffect(SFX_SELECT);
	mmEffectVolume(SFX_RUN,255);
	// inMenu = false;

	///////Upper Image setting////////

	// Enable and configure VRAM A for MAIN
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;

	//Engine configuration in Rotoscale mode
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG1_ACTIVE;

	//Confiure background
	BGCTRL[2] = BgSize_B8_256x256 |BG_BMP_BASE(0);
	BGCTRL[1] = BG_COLOR_256 | BG_TILE_BASE(4) | BG_MAP_BASE(24);

	uint16 * myPalette = BG_PALETTE;
	for(int i = 0; i < 255; ++i){
		myPalette[i] = selectPal[i];
	}
	//swiCopy(selectPal, BG_PALETTE, selectPalLen / 2);
	swiCopy(selectBitmap, BG_GFX, selectBitmapLen / 2);

	bgTransform[2]->hdx = 1 * 256;
	bgTransform[2]->vdx = 0 * 256;
	bgTransform[2]->hdy = 0 * 256;
	bgTransform[2]->vdy = 1 * 256;
	bgTransform[2]->dx = 0 * 256;
	bgTransform[2]->dy = 0 * 256;

	configureSelectSprites();
	u16 keys = keysDown();

	VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;

			//Engine configuration in tile mode
			REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;

			//Confiure background
			BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1)
					| BG_32x32;

	swiCopy(pediaPal, BG_PALETTE_SUB, pediaPalLen / 2);
				swiCopy(pediaTiles, BG_TILE_RAM_SUB(1), pediaTilesLen / 2);

				swiCopy(pediaMap, BG_MAP_RAM_SUB(0), pediaMapLen / 2);





	int xs = 125;
	while (1) {

		//Read held keys
		scanKeys();
		keys = keysDown();

		//Modify position of the sprite accordingly
		if ((keys & KEY_RIGHT)
				&& (xs < 187)){
			xs += 80;
		mmEffect(SFX_SELECT);
		}

		if ((keys & KEY_LEFT) && (xs > 67)){
			xs -= 80;
			mmEffect(SFX_SELECT);
}

		oamSet(&oamMain, 	// oam handler
				0,				// Number of sprite
				xs, 160,			// Coordinates
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
		if (keys & KEY_A) {
			if(xs<120){
				selectedPokemon=1;
			}
			else if((xs>120)&&(xs<187)){
				selectedPokemon=2;
			}else{
				selectedPokemon=3;
			}
			for(int i = 0; i < 255; ++i){
				myPalette[i] = battlePal[i];
			}
			//swiCopy(battlePal, BG_PALETTE, battlePalLen / 2);


			swiCopy(battleBitmap, BG_GFX, battleBitmapLen / 2);
			dmaCopy(greenTile, BG_TILE_RAM(24), 64);
			myPalette[255] = ARGB16(1, 0, 25, 0);

			for(int i = 22; i <= 47; i++){
				BG_MAP_RAM(1)[(35*128) + i] = 0xFFFF;
				BG_MAP_RAM(1)[(36*128) + i] = 0xFFFF;
				BG_MAP_RAM(1)[(37*128) + i] = 0xFFFF;
			}


			break;
		}
	}










	//configureSelectSprites();


	//Transfer tiles to VRAM
//	swiCopy(battlePal, BG_PALETTE, battlePalLen / 2);
//	swiCopy(battleBitmap, BG_GFX, battleBitmapLen / 2);





	///////Lower Background setting////////

	// Enable and configure VRAM C for SUB

//    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
//	VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
//
//			//Engine configuration in tile mode
//			REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;
//
//			//Confiure background
//			BGCTRL_SUB[0] = BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1)
//					| BG_32x32;

	swiCopy(menuPal, BG_PALETTE_SUB, menuPalLen / 2);
				swiCopy(menuTiles, BG_TILE_RAM_SUB(1), menuTilesLen / 2);

				swiCopy(menuMap, BG_MAP_RAM_SUB(0), menuMapLen / 2);

				bool inMenu = true;

				mmStart(MOD_POKECENTER,MM_PLAY_LOOP);
				music_playing = 1;
				//u16* P_Graphics_mainBuffer = buffer;
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
	curr_hp = max;

	setup_timer();
	for (;;) {
		swiWaitForVBlank();
		scanKeys();


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



				if ((touch.px >= 2) && (touch.px <= 123) && (touch.py >= 28)
						&& (touch.py <= 76) && inMenu == false) {

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
								3,				// Number of sprite
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
						if (keys & KEY_X ) {
							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
							swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);
							swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							break;
						}
						if((x >= 130 && x <= 190) && (y >= 20 && y <= 90)){
							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
							swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);
							swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							reduce_health(0.25);
							break;
						}
					}
				} else if ((touch.px >= 133) && (touch.px <= 251) && (touch.py >= 28) && (touch.py <= 76) && (inMenu == false)) {
					mmEffect(SFX_RESULT);

					//P_Graphics_mainBuffer[10] = 0;
					int body = 0;
					int face = 0;
					int tail = 0;
					int count = 0;

					swiCopy(findDiffPal, BG_PALETTE_SUB, findDiffPalLen / 2);
					swiCopy(findDiffTiles, BG_TILE_RAM_SUB(1),
							findDiffTilesLen / 2);
					swiCopy(findDiffMap, BG_MAP_RAM_SUB(0), findDiffMapLen / 2);
					swiWaitForVBlank();
					while (1) {
						scanKeys();
						keys = keysHeld();
						touchPosition t;
						touchRead(&t);
						if((t.px >= 188 && t.px<= 192) && (t.py >= 49 && t.py <= 58) && face == 0){
							face = 1;
							if(count == 0){
								reduce_health(0.10);
							}else if(count ==1){
								reduce_health(0.20);
							}else if(count == 2){
								reduce_health(0.30);
							}
							count +=1;
						}else if((t.px >= 222 && t.px <= 238) && (t.py >= 134 && t.py <= 146) && tail == 0){
							tail = 1;
							if(count == 0){
								reduce_health(0.10);
							}else if(count ==1){
								reduce_health(0.20);
							}else if(count == 2){
								reduce_health(0.30);
							}
							count +=1;
						}else if((t.px >= 177 && t.px <= 187) && (t.py >= 103 && t.py <= 125) && body == 0){
							body = 1;
							if(count == 0){
								reduce_health(0.10);
							}else if(count ==1){
								reduce_health(0.20);
							}else if(count == 2){
								reduce_health(0.30);
							}
							count +=1;
						}

						if ((keys & KEY_X || (count ==3)) && (curr_hp > start)) {
							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
							swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);
							swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							break;
						}
					}

				}
				else if(((touch.px >= 3) && (touch.px <= 123) && (touch.py >= 92) && (touch.py <= 141)&& (inMenu == false))){
					mmEffect(SFX_RESULT);

					swiCopy(mathsPal, BG_PALETTE_SUB, mathsPalLen / 2);
					swiCopy(mathsTiles, BG_TILE_RAM_SUB(1),mathsTilesLen / 2);
					swiCopy(mathsMap, BG_MAP_RAM_SUB(0), mathsMapLen / 2);
					//printDigit(BG_MAP_RAM(0), 3, 0,0);
					swiWaitForVBlank();
					while (1) {
						scanKeys();
						keys = keysHeld();
						if(((touch.py >= 110) && (touch.px <= 131) )){
							if(((touch.px >= 30) && (touch.px <= 44) )){

							}else if(((touch.px >= 46) && (touch.px <= 61) )){

							}
							else if(((touch.px >= 63) && (touch.px <= 79) )){

							}
						}
						if (keys & KEY_X) {
							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
							swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);
							swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							break;
						}
				}

				}else if(((touch.px >= 132) && (touch.px <= 254) && (touch.py >= 92) && (touch.py <= 141)&& (inMenu == false))){
					mmEffect(SFX_RESULT);

					swiCopy(mazePal, BG_PALETTE_SUB, mazePalLen / 2);
					swiCopy(mazeTiles, BG_TILE_RAM_SUB(1),mazeTilesLen / 2);
					swiCopy(mazeMap, BG_MAP_RAM_SUB(0), mazeMapLen / 2);

					configureMazeSprite();

					swiWaitForVBlank();
					int x = 131, y = 177;
					while (1) {
						//Read held keys
						scanKeys();
						keys = keysHeld();


						//Modify position of the sprite accordingly
						if ((keys & KEY_RIGHT)&&(x < 255)){
							if((isWall(x+17,y)==false) && (isWall(x+17,y+1)==false) && (isWall(x+17,y+2)==false)&&(isWall(x+17,y+3)==false)
									&&(isWall(x+17,y+5)==false) && (isWall(x+17,y+6)==false) && (isWall(x+17,y+7)==false) && (isWall(x+17,y+8)==false)
									&& (isWall(x+17,y+9)==false) && (isWall(x+17,y+10)==false) && (isWall(x+17,y+11)==false)){

								x += 2;
							}else{
								x -= 2;
							}
						}
						if ((keys & KEY_DOWN)&&(y < 191)){
//																																	if((isWall(x,y+15)==false)){
							if((isWall(x,y+15)==false) && (isWall(x+1,y+15)==false) && (isWall(x+2,y+15)==false)&&(isWall(x+3,y+15)==false)
									&&(isWall(x+4,y+15)==false) && (isWall(x+5,y+15)==false) && (isWall(x+6,y+15)==false) && (isWall(x+7,y+15)==false)
									&& (isWall(x+8,y+15)==false) && (isWall(x+9,y+15)==false) && (isWall(x+10,y+15)==false)&&(isWall(x+11,y+15)==false)
									&& (isWall(x+12,y+15)==false) && (isWall(x+13,y+15)==false) && (isWall(x+14,y+15)==false) && (isWall(x+15,y+15)==false)){
								y += 2;}
							else{
								y -= 2;
							}
						}
						if ((keys & KEY_LEFT) && (x > 0)){

							if((isWall(x-2,y)==false) && (isWall(x-2,y+1)==false) && (isWall(x-2,y+2)==false)&&(isWall(x-2,y+3)==false)
									&&(isWall(x-2,y+5)==false) && (isWall(x-2,y+6)==false) && (isWall(x-2,y+7)==false) && (isWall(x-2,y+8)==false)
									&& (isWall(x-2,y+9)==false) && (isWall(x-2,y+10)==false) && (isWall(x-2,y+11)==false)&&(isWall(x-2,y+12)==false) && (isWall(x-2,y+13)==false)){
								x -= 2;
							}else{
								x += 2;
							}
						}

						if ((keys & KEY_UP) && (y > 0) ){
							//if((isWall(x,y-2)==false)){

							if((isWall(x,y-2)==false) && (isWall(x+1,y-2)==false) && (isWall(x+2,y-2)==false)&&(isWall(x+3,y-2)==false)
									&&(isWall(x+4,y-2)==false) && (isWall(x+5,y-2)==false) && (isWall(x+6,y-2)==false) && (isWall(x+7,y-2)==false)
									&& (isWall(x+8,y-2)==false) && (isWall(x+9,y-2)==false) && (isWall(x+10,y-2)==false)&&(isWall(x+11,y-2)==false)
									&& (isWall(x+12,y-2)==false) && (isWall(x+13,y-2)==false) && (isWall(x+14,y-2)==false) && (isWall(x+15,y-2)==false)){
								y -= 2;
							}else{
								y += 2;
							}
						}


						oamSet(&oamSub, 	// oam handler
								1,				// Number of sprite
								x, y,			// Coordinates
								0,				// Priority
								0,				// Palette to use
								SpriteSize_16x16,			// Sprite size
								SpriteColorFormat_256Color,	// Color format
								gfx,			// Loaded graphic to display
								-1,			// Affine rotation to use (-1 none)
								false,			// Double size if rotating
								false,			// Hide this sprite
								false, false,	// Horizontal or vertical flip
								false			// Mosaic
								);

						swiWaitForVBlank();

						if((x>= 109 && x <= 121) && (y >= 3 && y <= 18)){
							oamClearSprite(&oamSub, 1);
							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
							swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);
							swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							reduce_health(0.4);
							break;
						}
						//Update the sprites
						oamUpdate(&oamSub);
						if (keys & KEY_X) {
							oamClearSprite(&oamSub, 1);

							swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
							swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);
							swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
							break;
						}
					}
				}
				else if ((touch.px >= 26) && (touch.px <= 229) && (touch.py >= 43) && (touch.py <= 125)) {
					//while(1){

	//				    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
	//
	//				    //Engine configuration in tile mode
	//				    REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;
	//
	//				    //Confiure background
	//				    BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;
					mmEffect(SFX_RESULT);
					// Transfer tiles to VRAM (chosen tile base)
					swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen / 2);
					swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen / 2);

					swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen / 2);
					inMenu = false;

				}




		} else if (keys & KEY_Y) {

			inMenu = true;
			mmEffect(SFX_RESULT);

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

			end_game();
			mmEffect(SFX_RUN);


			break;

		}
	}


	while (1)
		swiWaitForVBlank();

}

void end_game(){
	mmUnload(MOD_POKECENTER);


	REG_DISPCNT = MODE_5_2D | DISPLAY_BG1_ACTIVE;
	BGCTRL[1] = BG_32x32 | BG_COLOR_256 | BG_TILE_BASE(1) | BG_MAP_BASE(0);

	swiCopy(numbersPal, BG_PALETTE, numbersPalLen/2);
	swiCopy(numbersTiles, BG_TILE_RAM(1), numbersTilesLen/2);
	irqDisable(IRQ_TIMER0);
	updateChronoDisp(BG_MAP_RAM(0), min,  sec, msec);
	update_score();

	swiCopy(gameoverPal, BG_PALETTE_SUB, gameoverPalLen / 2);
	swiCopy(gameoverTiles, BG_TILE_RAM_SUB(1), gameoverTilesLen / 2);

	swiCopy(gameoverMap, BG_MAP_RAM_SUB(0), gameoverMapLen / 2);
}

