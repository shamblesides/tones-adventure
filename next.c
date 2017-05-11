//standard library stuf
#include <gb/gb.h>
//stuff for use in the program?
#include "tile_mem.h"
#include "chunks.h"
#include "areas.h"
#include "cam.h"
#include "npcs.h"

#include "tiles/npc_tiles.h" //TODO remove

// |1: odd frame
// |2: ready for next interrupt
//UINT16 deadbeef;
//UINT16 fps;
//UINT16 cycles;
UINT8 main_flags;
//this is performed when frame refreshes (prototype)
void vbl_int(void);
//list of objects in game!
////TODO put this in some kinda world file
struct chunk entities[2];

//main loop of the game
void main() {
	UINT8 i;
	//set stuff to "not ready" mode
	// turn off screen
	DISPLAY_OFF;
	// frame update stuff
	main_flags=2;

	//create player object for the world!
	////TODO put this in the world class thing
	SWITCH_ROM_MBC1(NPC_TILES_BANK);
	i=push_OBJ_tiledata(tone_tiles, tone_tiles_length);
	make_player(&entities[0],2048UL,3584UL,&intro,i,0);
	make_player(&entities[1],2548UL,3484UL,&intro,i,4);

	//focus camera on the player
	set_cam(&intro,&entities[0]);
	
	//turn on the lights
	SHOW_BKG;
	SHOW_SPRITES;
	DISPLAY_ON;
	

	//add a VBLANK interrupt action
	wait_vbl_done();
	//disable_interrupts();
	add_VBL(vbl_int);
	//CGB double-speed mode
	cpu_fast();

	//mus
	NR52_REG = 0x80; //activate sound controller
	NR50_REG = 0xFF; 
	NR51_REG = 0xFF; 
	
	//and now the interrupt should be ready
	main_flags=0;
	//the interrupts do the rest!
}

void main_act() {
	UWORD freq = 1750;
	NR10_REG = 0;      // No frequency sweep 
	NR11_REG = 0x40;   // 25% duty cycle 
	NR12_REG = 0xa6;   // Maximum volume 
	NR13_REG = (UINT8)freq; 
	//NR14_REG = 0x80 | (freq>>8); // Start output 

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
		entities[0].transp= !entities[0].transp;
		delay(100);
		while(joypad() & J_SELECT);
	}
	//make scyfe move around
	entities[0].act(&entities[0]);
	entities[1].act(&entities[1]);
	//adjust camera and view stuff
	cam_act();
}

//performed when frame refreshes
void vbl_int(void) {
	UINT8 i;
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
	//fps=cycles;
	//cycles=0;

	//move bkg tiles to reflect
	move_bkg((UINT8)(cam_x>>4),(UINT8)(cam_y>>4));
	//move sprites
	//TODO do more than just one
	for(i=0;i<2;++i) {
		chk= &entities[i];
		//end_sprite=(chk->first_sprite)+(chk->num_sprites);
		move_sprite(chk->first_sprite,
				(chk->left_x>>4)+chk->x_off+8-(cam_x>>4),
			       	(chk->top_y>>4)+ chk->y_off+16-(cam_y>>4));
		move_sprite(chk->first_sprite+1,
				(chk->left_x>>4)+chk->x_off+8-(cam_x>>4),
			       	(chk->top_y>>4)+ chk->y_off+24-(cam_y>>4));
		move_sprite(chk->first_sprite+2,
				(chk->left_x>>4)+chk->x_off+16-(cam_x>>4),
			       	(chk->top_y>>4)+ chk->y_off+16-(cam_y>>4));
		move_sprite(chk->first_sprite+3,
				(chk->left_x>>4)+chk->x_off+16-(cam_x>>4),
			       	(chk->top_y>>4)+ chk->y_off+24-(cam_y>>4));
	}
	//main method stuff
	main_act();
	
	//done. remove occupied flag.
	main_flags &= ~2;
}
