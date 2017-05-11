/*
 * AREADATA.C
 *
 * This file contains metadata used for areas in NEXT.C,
 * including their dimensions, and how they are stored
 * in their memory banks.
 *
 * @author Nigel
 */

//standard gameboy library (izziss necessary?)
#include <gb/gb.h>
#include "areas.h"

//method for getting the tile for a certain part of the map
UINT8 * get_tile_pointer(struct area * place, UINT8 col, UINT8 row) {
	UINT8 * ptr;
	ptr= (place->tiles_start);
	ptr= ptr +(UINT16)(col)*place->map_rows +row;
	return ptr;
}

//OKAY THIS ONE gets the tile properties at a certain part
UINT8 * get_prop_pointer(struct area * place, UINT8 col, UINT8 row) {
	UINT8 * ptr;
	ptr=place->props_start;
	ptr= ptr +(UINT16)((UINT16)col)*place->map_rows +row;
	return ptr;
}

//intro
extern unsigned char intro_tiles[];
extern unsigned char intro_props[];
struct area intro= {
	120,48, 	//map_cols, map_rows
	intro_tiles,	//tiles_start
	intro_props,	//props_start
	4,4,		//tiles_bank,props_bank
	{ 32767UL, 671UL  , 337UL  , 0UL }, //bg palette
	{ 6076UL , 32767UL, 15136UL, 0UL } //sprite palette
};

//title
// extern unsigned char title_tiles[];
// extern unsigned char title_props[];
// struct area title= {
// 	96,18, 	//map_cols, map_rows
// 	title_tiles,	//tiles_start
// 	title_props,	//props_start
// 	5,5,		//tiles_bank,props_bank
// 	{ 32767UL, 671UL  , 337UL  , 0UL }, //bg palette
// 	{ 6076UL , 32767UL, 15136UL, 0UL } //sprite palette
// };
