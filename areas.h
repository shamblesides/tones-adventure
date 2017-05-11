/*
 * AREAS.H
 *
 * Provides the struct for all area metadata,
 * references to methods used to work with
 * tiledata, as well as references to all
 * external map tile and property data.
 *
 */

#ifndef AREAS_H
#define AREAS_H

#include <gb/gb.h>

//the struct containing all the basic info about the different areas
struct area {
	//height and width of whole map
	UINT8 map_cols;
	UINT8 map_rows;
	//segmentation of the map; segments should be 64px wide
	//map data itself for the segement
	UINT8* tiles_start;
	UINT8* props_start;
	//banks for each segment
	UINT8 tiles_bank;
	UINT8 props_bank;
	//palettes for the areas
	UWORD bg_pal[4];
	UWORD sprite_pal[4];
};

//useful methods for extracting data
UINT8 * get_tile_pointer(struct area * place, UINT8 col, UINT8 row);

UINT8 * get_prop_pointer(struct area * place, UINT8 col, UINT8 row);

//maps!
extern struct area intro;



#endif
