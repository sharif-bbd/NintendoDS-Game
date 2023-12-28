
//{{BLOCK(findDiff)

//======================================================================
//
//	findDiff, 256x192@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 363 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 23232 + 1536 = 25280
//
//	Time-stamp: 2023-12-28, 18:31:47
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FINDDIFF_H
#define GRIT_FINDDIFF_H

#define findDiffTilesLen 23232
extern const unsigned int findDiffTiles[5808];

#define findDiffMapLen 1536
extern const unsigned short findDiffMap[768];

#define findDiffPalLen 512
extern const unsigned short findDiffPal[256];

#endif // GRIT_FINDDIFF_H

//}}BLOCK(findDiff)
