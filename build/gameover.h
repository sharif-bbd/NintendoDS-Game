
//{{BLOCK(gameover)

//======================================================================
//
//	gameover, 256x192@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 672 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 43008 + 1536 = 45056
//
//	Time-stamp: 2024-01-06, 00:04:45
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_GAMEOVER_H
#define GRIT_GAMEOVER_H

#define gameoverTilesLen 43008
extern const unsigned int gameoverTiles[10752];

#define gameoverMapLen 1536
extern const unsigned short gameoverMap[768];

#define gameoverPalLen 512
extern const unsigned short gameoverPal[256];

#endif // GRIT_GAMEOVER_H

//}}BLOCK(gameover)
