/*
 * Template Nintendo DS
 * May 2011
 */

#include <nds.h>
#include <stdio.h>

int main(void) {
	
    consoleDemoInit();
    printf("I made changes\n");

    while(1)
        swiWaitForVBlank();	
}
