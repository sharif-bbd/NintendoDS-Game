
//{{BLOCK(start)

//======================================================================
//
//	start, 256x192@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 377 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 24128 + 1536 = 26176
//
//	Time-stamp: 2024-01-06, 00:04:45
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_START_H
#define GRIT_START_H

#define startTilesLen 24128
extern const unsigned int startTiles[6032];

#define startMapLen 1536
extern const unsigned short startMap[768];

#define startPalLen 512
extern const unsigned short startPal[256];

#endif // GRIT_START_H

//}}BLOCK(start)
