
//{{BLOCK(menu)

//======================================================================
//
//	menu, 256x192@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 438 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 28032 + 1536 = 30080
//
//	Time-stamp: 2024-01-06, 00:04:45
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MENU_H
#define GRIT_MENU_H

#define menuTilesLen 28032
extern const unsigned int menuTiles[7008];

#define menuMapLen 1536
extern const unsigned short menuMap[768];

#define menuPalLen 512
extern const unsigned short menuPal[256];

#endif // GRIT_MENU_H

//}}BLOCK(menu)
