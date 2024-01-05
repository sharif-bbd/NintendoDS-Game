
//{{BLOCK(findDiff)

//======================================================================
//
//	findDiff, 256x192@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 456 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 29184 + 1536 = 31232
//
//	Time-stamp: 2024-01-06, 00:04:45
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FINDDIFF_H
#define GRIT_FINDDIFF_H

#define findDiffTilesLen 29184
extern const unsigned int findDiffTiles[7296];

#define findDiffMapLen 1536
extern const unsigned short findDiffMap[768];

#define findDiffPalLen 512
extern const unsigned short findDiffPal[256];

#endif // GRIT_FINDDIFF_H

//}}BLOCK(findDiff)
