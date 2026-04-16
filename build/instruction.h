
//{{BLOCK(instruction)

//======================================================================
//
//	instruction, 256x192@8, 
//	Transparent color : 00,00,00
//	+ palette 256 entries, not compressed
//	+ 732 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 46848 + 1536 = 48896
//
//	Time-stamp: 2024-01-06, 21:44:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_INSTRUCTION_H
#define GRIT_INSTRUCTION_H

#define instructionTilesLen 46848
extern const unsigned int instructionTiles[11712];

#define instructionMapLen 1536
extern const unsigned short instructionMap[768];

#define instructionPalLen 512
extern const unsigned short instructionPal[256];

#endif // GRIT_INSTRUCTION_H

//}}BLOCK(instruction)
