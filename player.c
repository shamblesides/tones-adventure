//standard library stuf
#include <gb/gb.h>
//stuff for chunks!
#include "chunks.h"

//tiles the player uses
#include "tiles/npc_tiles.h"

//define flags...
#define JUMPING 8U
#define WAS_WATER 4U
//hit flags
#define HIT_GROUND 194U

//movement constants 	(walking)
#define JUMP_SPEED 	64	//initial jump velocity
#define HI_JUMP_SPEED	84	//extra high jump
#define WALK_ACCEL 	8	//walking acceleration
#define WALK_SKID 	4 	//walking friction
#define WALK_TOPSPEED 	64	//top speed walking
//			(aerial)
#define AIR_ACCEL	4	//changing direction in air
#define AIR_SKID	2	//horiz. slowdown in air
#define FORCE_G 	6	//gravity acceleration
#define T_VELO 		84	//terminal velocity
//			(swimming)
#define	WATER_V_SKID	2	//vertical slowdown in water
#define WATER_H_SKID	2	//horizontal slowdown
#define WATER_V_ACCEL	4	//movement acceleration
#define WATER_H_ACCEL	4	//movement acceleration
#define WATER_V_TOPSPEED 32	//top speed in water (vertical)
#define WATER_H_TOPSPEED 32	//top speed in water (horiz)
#define	WATER_T_VELO	8	//how fast we normally fall
#define WATER_JUMP_SPEED 40	//how fast he can jump from the ground underwater
#define WATER_BOB	20	//what speed he bobs out of the water with
#define WATER_JUMPOUT	56	//speed at which he totally jumps out of the water
#define WATER_WALL_SPEED 8	//how fast he walks up walls underwater
#define WATER_WALL_ACCEL 3	//accelleration for climbing up walls underwater

//typical averaging on a velocity
#define TO_VELO(x,v,a) \
	if(x!=v) { \
	if((x)<(v)) { \
		(x)+= (a); \
		if((x)> (v)) \
			(x)= (v); \
	} else { \
		(x)-= (a); \
		if((x)< (v)) \
			(x)= (v); \
	} \
	}


//function pointer for act method
void scyfe_act(struct chunk * chk);

//the chunk itself
struct chunk scyfe= {
	0,0,		//x,y
	9,16,3,		//w,h,bev
	0,0,		//vx,vy
	1,		//solid
	0,4,		//first sprite, num_sprites
	-3,0,		//x,yoffsets
	&intro,		//area
	0,		//hit_flags
	0,		//misc_flags
	scyfe_act,	//function pointer
	{0,0,0}		//misc data
			// [0] CYCLE
			// [1] undefined
			// [2] undefined

};

//initialize scyfe!
void init_scyfe(UINT16 lx, UINT16 ty, UINT8 sp1, struct area * place) {
	//position
	scyfe.left_x=lx;
	scyfe.top_y=ty;
	//map
	scyfe.my_area=place;
	//sprites
	scyfe.first_sprite=sp1;
}

//temp variable for calculating the sprites to use
static UINT8 scyfe_tile;
//animation cycle info
// |128 walking
// |64
#define CYCLE chk->data[0]

//actions taken each step for what scyfe does/ his frames
void scyfe_act(struct chunk * chk) {
	//what buttons are pressed RIGHT NOW
	UBYTE pad;
	//tiles; used at the end
	UINT8 t;
	//---

	pad=joypad();
	//first do movement stuff!
	// is it water movement or regular?
	if(chk->chk_flags&FLAG_WATER) { //WATER MOVEMENT
		//horiz
		if(pad&J_RIGHT) {
			TO_VELO(chk->vx,WATER_H_TOPSPEED,WATER_H_ACCEL);
			chk->chk_flags|=FACING_RIGHT;
			chk->x_off= -4;
		}
		else if(pad&J_LEFT) {
			TO_VELO(chk->vx,-WATER_H_TOPSPEED,WATER_H_ACCEL);
			chk->chk_flags&= ~FACING_RIGHT;
			chk->x_off= -3;
		}
		else {
			//slow down
			TO_VELO(chk->vx,0,WATER_H_SKID);
		}
		//verti
		if(pad&J_DOWN) {
			TO_VELO(chk->vy,WATER_V_TOPSPEED,WATER_V_ACCEL);
		}
		else if(pad&J_UP) {
			TO_VELO(chk->vy,-WATER_V_TOPSPEED,WATER_V_ACCEL);
		}
		else if(chk->hit_flags&(HIT_LEFT|HIT_RIGHT)) {
			//running against a wall makes you ascend then jump out of water
			TO_VELO(chk->vy,-WATER_WALL_SPEED,WATER_WALL_ACCEL);
		}
		else {
			//slow down
			TO_VELO(chk->vy,WATER_T_VELO,WATER_V_SKID);
		}
		if(pad&J_A) {
			if(!(chk->chk_flags&JUMPING)) {
				if(chk->hit_flags&HIT_GROUND) { //jumping from ground
					chk->vy= -WATER_JUMP_SPEED;
				}
			}
			chk->chk_flags |=JUMPING;
		} else {
			chk->chk_flags &= ~JUMPING;
		}

	}
	//REGULAR MOVEMENT
	else {
		//popping out of the water
		if(chk->chk_flags&WAS_WATER) {
			//hopping out
			if(chk->hit_flags&(HIT_LEFT|HIT_RIGHT)) {
				chk->vy= -WATER_JUMPOUT;
			}
			//bobbing at the surface
			else {
				chk->vy= -chk->vy;
				chk->vy= -(chk->vy>>2) -(WATER_BOB)+(WATER_BOB>>2);
			}
		}
		//set direction and movement.
		if(pad&J_RIGHT) {
			TO_VELO(chk->vx,WALK_TOPSPEED,WALK_ACCEL);
			chk->chk_flags|=FACING_RIGHT;
			chk->x_off= -4;
		}
		else if(pad&J_LEFT) {
			TO_VELO(chk->vx,-WALK_TOPSPEED,WALK_ACCEL);
			chk->chk_flags&= ~FACING_RIGHT;
			chk->x_off= -3;
		}
		else if(chk->vx) {//if it's moving
			//slow down
			TO_VELO(chk->vx,0,((chk->hit_flags&HIT_GROUND)?WALK_SKID:AIR_SKID));
		}
		//jumping
		if(pad&J_A) {
			if(!(chk->chk_flags&JUMPING)) {
				if(chk->hit_flags&HIT_GROUND) { //jumping from ground
					chk->vy= (pad&J_UP)?-HI_JUMP_SPEED:-JUMP_SPEED;
				}
			}
			chk->chk_flags |=JUMPING;
		} else {
			chk->chk_flags &= ~JUMPING;
		}
		//speed downward
		TO_VELO(chk->vy,T_VELO,FORCE_G);
	}

	//was in water
	if(chk->chk_flags&FLAG_WATER)
		chk->chk_flags|= WAS_WATER;
	else
		chk->chk_flags&= ~WAS_WATER;
	//do_actual_movement.
	GO_CHUNK(chk);

	//now do frame stuff
	// non-ground (swimming/aerial)
	if( !(chk->hit_flags&HIT_GROUND)
			&& !(chk->hit_flags&(HIT_LEFT|HIT_RIGHT) && chk->chk_flags&FLAG_WATER) ) {
		if(chk->chk_flags&FLAG_WATER) {
			++CYCLE;
			if((CYCLE&208U)!=192U )
				CYCLE=192U;
			scyfe_tile= ((chk->chk_flags&FACING_RIGHT)?92:28)-(CYCLE&8?4:0);
		}
		else {
			//go to a feet-up position
			if((CYCLE&135U)==132U ) {//already there
				//nothing new
			}
			else if(CYCLE&128U) {//already walking
				CYCLE&=143U;
				++CYCLE;
			}else {//start walking
				CYCLE=128U;
			}
			//calculate frame
			scyfe_tile= ( ((chk->chk_flags&FACING_RIGHT)?96:32) | (CYCLE&8?12:0) )
				+((CYCLE&6)? ((CYCLE&6)==6?8:4):0);
		}

	}
	//walking frames
	else if((pad&J_RIGHT) || (pad&J_LEFT)) {
		if(CYCLE&128U) {//already walking
			CYCLE&=143U;
			++CYCLE;
		}else {//start walking
			CYCLE=128U;
		}
		//calculate frame
		scyfe_tile= ( ((chk->chk_flags&FACING_RIGHT)?96:32) | (CYCLE&8?12:0) )
			+((CYCLE&6)? ((CYCLE&6)==6?8:4):0);
	}
	//idle frames
	else {
		//update the count
		++CYCLE;
		//just barely becoming idle, as well as rollover
		if(CYCLE&128U) CYCLE=0;
		//calculate frame (I am so sorry.)
		scyfe_tile= ((chk->chk_flags&FACING_RIGHT)?64:0) |
			( (CYCLE&112)==112?((CYCLE&118)==116?20:16): //blinking
			((CYCLE&96)==64)&&((CYCLE&96)!=96)?((CYCLE&6)<<1): //spinning
			0 ); //still
	}


	//set the tiles
	set_sprite_tile(t=chk->first_sprite, scyfe_tile++);
	set_sprite_tile(++t, scyfe_tile++);
	set_sprite_tile(++t, scyfe_tile++);
	set_sprite_tile(++t, scyfe_tile++);

}
