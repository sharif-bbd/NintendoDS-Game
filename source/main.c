/*
 * Template Nintendo DS
 * May 2011
 */

#include <nds.h>
#include <stdio.h>
#include "battle.h"
#include "skills.h"
#include "menu.h"

#include <maxmod9.h>
#include "soundbank.h"
#include "soundbank_bin.h"





int main(void) {
	
	consoleDemoInit();



	//////set music//////
	mmInitDefaultMem((mm_addr)soundbank_bin);

	//Load module
	//mmLoad(MOD_MUSIC);
	mmLoad(MOD_POKECENTER);
	//Load effect
	mmLoadEffect(SFX_RESULT);

    ///////Upper Image setting////////

    // Enable and configure VRAM A for MAIN
	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;

	//Engine configuration in Rotoscale mode
	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE;

	//Confiure background
	BGCTRL[2]= BG_MAP_BASE(0) | BgSize_B8_256x256;


	//Transfer tiles to VRAM
	swiCopy(battlePal, BG_PALETTE, battlePalLen/2);
	swiCopy(battleBitmap, BG_GFX, battleBitmapLen/2);


	bgTransform[2]->hdx = 1*256;
	bgTransform[2]->vdx = 0*256;
	bgTransform[2]->hdy = 0*256;
	bgTransform[2]->vdy = 1*256;
	bgTransform[2]->dx = 0*256;
	bgTransform[2]->dy = 0*256;



    ///////Lower Background setting////////

	// Enable and configure VRAM C for SUB

//    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;
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

	for(;;) {
		swiWaitForVBlank();
		scanKeys();

		unsigned held = keysHeld();
		u16 keys = keysDown();

		//if (keys&KEY_A){printf("A");}
		if (keys & KEY_TOUCH) {
			touchPosition touch;
			touchRead(&touch);

			if((touch.px>= 31) && (touch.px <= 161) ){
				printf("\x1b[6;5HTouch x = %04X, %04X\n",
							touch.rawx, touch.px);
				    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;

				    //Engine configuration in tile mode
				    REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;

				    //Confiure background
				    BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;

				    // Transfer tiles to VRAM (chosen tile base)
				    swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen/2);
				    swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen/2);

				    swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen/2);


				    mmStart(MOD_POKECENTER,MM_PLAY_LOOP);






			}

			else{
				printf("y");
			}





		}
		else if (keys & KEY_A){
						printf("y");

						VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;


						REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;


						BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;


						swiCopy(menuPal, BG_PALETTE_SUB, menuPalLen/2);
						swiCopy(menuTiles, BG_TILE_RAM_SUB(1), menuTilesLen/2);

						swiCopy(menuMap, BG_MAP_RAM_SUB(0), menuMapLen/2);
					}
	}














    while(1)
            swiWaitForVBlank();


}

//up left box: x= 0~123, y= 28~76
