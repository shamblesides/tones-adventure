//standard library stuf
#include <gb/gb.h>
//stuff for use in the program?
#include "chunks.h"
#include "areas.h"

//tiles
#include "tiles/bkg_tiles.h" //TODO remove

//camera vars
UINT16 cam_x, cam_y;
static struct area * my_area;
static struct chunk * cam_focus;

void set_cam(struct area * new_area, struct chunk * new_focus) {
	UINT8 new_col, new_row;
	UINT8 c;

	//first thing's first. area.
	my_area= new_area;

	//set tile data
	//TODO load from area data
	SWITCH_ROM_MBC1(BKG_TILES_BANK);
	set_bkg_data(0,water_tiles_length,water_tiles);

	//set what cam's gonna focus on
	//TODO determine this from area somehow??
	cam_focus= new_focus;
	//set initial position of CAMERA
	cam_x= cam_focus->left_x+(cam_focus->w<<3)-(SCREENWIDTH<<3);
	cam_y= cam_focus->top_y-(SCREENHEIGHT<<3);
	//NOTE: chunks should set their own frames, no problem there...?

	//draw the map at the right place
	new_col= (UINT8)(cam_x>>7);
	new_row= (UINT8)(cam_y>>7);
	//set all the tiles on-screen
	SWITCH_ROM_MBC1(
		my_area->tiles_bank
	);
	for(c=0;c!=21;++c) {
		set_bkg_tiles(
				(new_col+c)&31,
				new_row&31,
				1,19,
				get_tile_pointer(my_area,new_col+c,new_row)
		);
	}
	//palettes!
	if(_cpu==CGB_TYPE) {
		set_bkg_palette(0,1, &(my_area->bg_pal[0]));//TODO simpler syntax
		set_sprite_palette( 0, 1, &(my_area->sprite_pal[0]));
	}
	
}

void set_cam_focus(struct chunk * new_focus) {
	cam_focus= new_focus;
}

//store where the camera was at previously
UINT16 old_x, old_y;
void cam_act() {
	UINT16 target_x, target_y;
	UINT8 old_row, new_row, old_col, new_col,c;
	UINT8 * t;
	//remember where it was previously for the next call
	old_x= cam_x;
	old_y= cam_y;
	//where the target for the camera will be
	//target_x= cam_target_x();
	//target_y= cam_target_y();
	if((SCREENWIDTH<<3) > cam_focus->left_x )
		target_x=0;
	//NOTE: for interesting effect, remove the chk->w bit from the next line.
	else if(((UINT16)my_area->map_cols<<7)-(cam_focus->w<<3)-(SCREENWIDTH<<3) < cam_focus->left_x)
		target_x=((UINT16)my_area->map_cols<<7)-(SCREENWIDTH<<4);
	else	
		target_x= cam_focus->left_x+(cam_focus->w<<3)-(SCREENWIDTH<<3);
	if(SCREENHEIGHT<<3 > cam_focus->top_y )
		target_y=0;
	else if(((UINT16)my_area->map_rows<<7)-(SCREENHEIGHT<<3) < cam_focus->top_y)
		target_y=((UINT16)my_area->map_rows<<7)-(SCREENHEIGHT<<4);
	else
		target_y= cam_focus->top_y-(SCREENHEIGHT<<3);
	//scroll camera along towards it
	cam_x=(old_x<<1)-(old_x>>1)+(target_x>>1)>>1;
	cam_y=(old_y<<1)-(old_y>>1)+(target_y>>1)>>1;
	//blit updated tiles as needed
	old_col= old_x>>7;
	new_col= cam_x>>7;
	old_row= old_y>>7;
	new_row= cam_y>>7;
	//switch to bank with tiledata
	SWITCH_ROM_MBC1(
		my_area->tiles_bank
	);
	//scrolling left
	if(new_col<old_col) {
		t= get_tile_pointer(
			my_area,
			new_col,
			new_row
		);
		c= new_row&31;
		set_bkg_tiles(new_col&31,c, 1,32-c, t);
		if(c>13)
			set_bkg_tiles(new_col&31,0, 1,c-13, t+32-c);
	}
	//scrolling right
	else if(old_col!=new_col) {
		t= get_tile_pointer(
			my_area,
			new_col+20,
			new_row
		);
		c= new_row&31;
		set_bkg_tiles((new_col+20)&31,c, 1,32-c, t);
		if(c>13)
			set_bkg_tiles((new_col+20)&31,0, 1,c-13, t+32-c);
	}
	//scrolling up
	if(new_row<old_row) {
		for(c=0;c!=21;++c) {
			t= get_tile_pointer(
					my_area,
					new_col+c,
					new_row
			);
			set_bkg_tiles((new_col+c)&31,new_row&31, 1,1,t);
		}
	}
	//scrolling down
	else if(old_row!=new_row) {
		for(c=0;c!=21;++c) {
			t= get_tile_pointer(
					my_area,
					new_col+c,
					new_row+18
			);
			set_bkg_tiles((new_col+c)&31,(new_row+18)&31, 1,1,t);
		}
	}
}
