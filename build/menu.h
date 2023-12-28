
//{{BLOCK(menu)

//======================================================================
//
//	menu, 256x192@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 521 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 33344 + 1536 = 35392
//
//	Time-stamp: 2023-12-28, 18:29:34
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MENU_H
#define GRIT_MENU_H

#define menuTilesLen 33344
extern const unsigned int menuTiles[8336];

#define menuMapLen 1536
extern const unsigned short menuMap[768];

#define menuPalLen 512
extern const unsigned short menuPal[256];

#endif // GRIT_MENU_H

//}}BLOCK(menu)
