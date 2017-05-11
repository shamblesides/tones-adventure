//standard library stuf
#include <gb/gb.h>
//stuff for use in the program?
#include "tile_mem.h"
#include "chunks.h"
#include "areas.h"
#include "cam.h"
#include "npcs.h"

#include "tiles/npc_tiles.h" //TODO remove


//objects in the map! implements stack for now.
static UINT8 num_entities=0;
static struct chunk entities[4];

static struct * chunk alloc_chunk() {
	return &entities[num_entities++];
}

void init_world(struct area * my_area) {
	//set stuff to "not ready" mode
	// turn off screen
	DISPLAY_OFF;

	//create player object for the world!
	SWITCH_ROM_MBC1(NPC_TILES_BANK);
	{
		UINT8 i=push_OBJ_tiles(tone_tiles, tone_tiles_length);
		make_player(alloc_chunk(),2048UL,3584UL,&intro,i,0);
		make_player(alloc_chunk(),2548UL,3484UL,&intro,i,4);
	}

	//focus camera on the player
	set_cam(&intro,&entities[0]);
	
	//turn on the lights
	SHOW_BKG;
	SHOW_SPRITES;
	DISPLAY_ON;
}

void world_act() {
	UINT8 i;
	struct chunk * chk;

	/* MUSIC STUFF GETS PRIORITY */
	//TODO maybe put in vblank
	UWORD freq = 1750;
	NR10_REG = 0;      // No frequency sweep 
	NR11_REG = 0x40;   // 25% duty cycle 
	NR12_REG = 0xa6;   // Maximum volume 
	NR13_REG = (UINT8)freq; 
	//NR14_REG = 0x80 | (freq>>8); // Start output 

	/* FIRST DO THE SCREEN STUFF */
	//update bkg position
	move_bkg((UINT8)(cam_x>>4),(UINT8)(cam_y>>4));
	//move sprites
	//TODO do more than just one
	for(i=0;i<numEntities;++i) {
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

	/* WORLD STEP */
	
	//obfuscated pausing loop; to remove
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
	//disable collisions (testing)
	if(joypad() & J_SELECT) {
		entities[0].transp= !entities[0].transp;
		delay(100);
		while(joypad() & J_SELECT);
	}
	//handle entities
	for(i=0;i<numEntities;++i) {
		entities[i].act(&entities[i]);
	}

	//update camera
	cam_act();
}

//main loop of the game
void main() {

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

//performed when frame refreshes
void vbl_int(void) {
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
	
	//do thing depending on mode.
	

	//done. remove occupied flag.
	main_flags &= ~2;
}
