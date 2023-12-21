/*
 * Template Nintendo DS
 * May 2011
 */

#include <nds.h>
#include <stdio.h>
#include "battle.h"
#include "skills.h"

int main(void) {
	
    consoleDemoInit();
    //printf("I made changes\n");

    ///////Upper Image setting////////

	VRAM_A_CR = VRAM_ENABLE | VRAM_A_MAIN_BG;

	REG_DISPCNT = MODE_5_2D | DISPLAY_BG2_ACTIVE;

	BGCTRL[2]= BG_MAP_BASE(0) | BgSize_B8_256x256;

	swiCopy(battlePal, BG_PALETTE, battlePalLen/2);
	swiCopy(battleBitmap, BG_GFX, battleBitmapLen/2);


	bgTransform[2]->hdx = 1*256;
	bgTransform[2]->vdx = 0*256;
	bgTransform[2]->hdy = 0*256;
	bgTransform[2]->vdy = 1*256;
	bgTransform[2]->dx = 0*256;
	bgTransform[2]->dy = 0*256;



    ///////Lower Background setting////////
    VRAM_C_CR = VRAM_ENABLE | VRAM_C_SUB_BG;

    REG_DISPCNT_SUB = MODE_0_2D | DISPLAY_BG0_ACTIVE;

    BGCTRL_SUB[0]= BG_COLOR_256 | BG_MAP_BASE(0) | BG_TILE_BASE(1) | BG_32x32;

    swiCopy(skillsPal, BG_PALETTE_SUB, skillsPalLen/2);
    swiCopy(skillsTiles, BG_TILE_RAM_SUB(1), skillsTilesLen/2);

    swiCopy(skillsMap, BG_MAP_RAM_SUB(0), skillsMapLen/2);















    while(1)
            swiWaitForVBlank();


}
