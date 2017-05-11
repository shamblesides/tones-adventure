//standard library stuf
#include <gb/gb.h>
//stuff for use in the program?
#include "chunks.h"
#include "player.c"

//tiles
#include "tiles/bkg_tiles.h"
#include "tiles/npc_tiles.h"

#include "areas.h"

//camera that follows stuff
UINT16 cam_x, cam_y;
struct chunk * cam_focus;

//map stuff
struct area * my_area;

void init_cam() {
	UINT8 new_col, new_row;
	UINT8 c;

	//set tile data
	SWITCH_ROM_MBC1(BKG_TILES_BANK);
	set_bkg_data(0,water_tiles_length,water_tiles);
	SWITCH_ROM_MBC1(NPC_TILES_BANK);
	set_sprite_data(0,tone_tiles_length, tone_tiles);

	//SELECT MAP.
	my_area= &intro; //in AREAS

	//set what cam's gonna focus on
	cam_focus= &scyfe;
	//set initial position of CAMERA
	cam_x= cam_focus->left_x+(cam_focus->w<<3)-(SCREENWIDTH<<3);
	cam_y= cam_focus->top_y-(SCREENHEIGHT<<3);
	//NOTE: chunks should set their own frames, no problem there...?

	//draw the map at the right place
	new_col= (UINT8)(cam_x>>7);
	new_row= (UINT8)(cam_y>>7);
	//set all the tiles
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

//this is performed when frame refreshes (prototype)
void vbl_int(void);



// |1: odd frame
// |2: ready for next interrupt



UINT16 deadbeef;
UINT16 fps;
UINT16 cycles;
UINT8 main_flags;
//main loop of the game
void main() {
	//turn off the display at first
	DISPLAY_OFF;
	//
	main_flags=2; //not ready yet
	//initialize things
	////TODO SEPARATE THIS OUT elsewhere I Think.
	init_scyfe(2048UL,3584UL,0,&intro);
	init_cam();
	//palettes!
	if(_cpu==CGB_TYPE) {
		set_bkg_palette(0,1, &(my_area->bg_pal[0]));
		set_sprite_palette( 0, 1, &(my_area->sprite_pal[0]));
	}
	//turn on the lights
	SHOW_BKG;
	SHOW_SPRITES;
	DISPLAY_ON;

	//music driver init
	/*clock01=0;
	step=0;
	patern=0;
	stopmusic();
	patern_definition();*/

	//add a VBLANK interrupt action
	wait_vbl_done();
	//disable_interrupts();
	add_VBL(vbl_int);
	//CGB double-speed mode
	cpu_fast();

	//and now the interrupt should be ready
	main_flags=0;
	//the interrupts do the rest!
	
	deadbeef=0xDEADUL;
	//count cycles
	while(1) {
		++cycles;
	}
}

void main_act() {

	//obfuscated pausing loop
	if(joypad() & J_START) {
		BGP_REG= 0xF9U;
		//TODO darken screen
		delay(50);
		while(joypad() & J_START);
		delay(100);
		while((joypad()&J_START) ==0 );
		delay(50);
		while(joypad()&J_START);
	}
	if(joypad() & J_SELECT) {
		//init_scyfe(2048UL,3584UL);
		//init_cam();
		//delay(50);
		scyfe.transp= !scyfe.transp;
		delay(100);
		while(joypad() & J_SELECT);
	}
	//make scyfe move around
	scyfe.act(&scyfe);
	//adjust camera and view stuff
	cam_act();
}

//performed when frame refreshes
void vbl_int(void) {
	UINT8 i,end_sprite;
	struct chunk * chk;

	//if frame is currently occupied
	if(main_flags&2) {
		//do it as soon as possible
		main_flags=2;
		return;
	}
		
	//else if is other frame
	
	else if(main_flags&1) {
		//is ready
		main_flags=0;
		return;
	}
	
	//if is ready
	main_flags=3;	//now occupied!
	fps=cycles;
	cycles=0;

	//move bkg tiles to reflect
	move_bkg((UINT8)(cam_x>>4),(UINT8)(cam_y>>4));
	//move sprites
	//TODO do more than scyfe
	chk= &scyfe;
	end_sprite=(chk->first_sprite)+(chk->num_sprites);
	for(i=chk->first_sprite;i!=end_sprite;i++)
		move_sprite(i, (chk->left_x>>4)+chk->x_off+(i>1?16:8 )-(cam_x>>4),
			       (chk->top_y>>4)+ chk->y_off+(i&1?24:16)-(cam_y>>4));
	//main method stuff
	main_act();
	
	//done. remove occupied flag.
	main_flags &= ~2;
	
	//play music??
	/*if (clock01>=7){clock01=0;}      // set the speed of your music
	if (!clock01) {music();}
	clock01+=1;*/
}
