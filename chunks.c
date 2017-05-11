/*
 * CHUNKS.C
 * 
 * Contains definitions for a chunk structure,
 * as well as the (very lengthy) routine for
 * handling collision and velocity.
 *
 */

//standard library stuf
#include <gb/gb.h>

//info for chunk/area
#include "chunks.h"
#include "areas.h" //actually already included in chunks.h

static UINT8 c,c1,c2,c3,co,cn, r,r1,r2,r3,ro,rn;
static INT16 ao,an;
static UINT8 new_hit_flags;
static UINT8 * my_tile_pointer;
static UINT16 old_v,new_v,push_v;

//vertical part of collision physics
static void up_coll(struct chunk * chk, INT8 vy) {
	//first, do the actual MOve!
	ro= (UINT8)(chk->top_y >>7); //old_y would be used here but is unnecessary
	chk->top_y+=(INT16)vy;
	rn= (UINT8)(chk->top_y >>7); //new_y would be used here but is unnecessary

	//first do the flat top/bottom collision
	// get columns and rows
	c1= (UINT8)((chk->left_x>>4)+ chk->bevel >>3);
	c2= (UINT8)((chk->left_x-1 >>4)+ chk->w -chk->bevel >>3);

	//push toppish
	// if we're in the same row then we can skip this part entirely
	if(rn!=ro) {
	if(ro) for(c=c1;c!=c2+1;++c) {
		//what tile is in the square we pushed into?
		my_tile_pointer= get_prop_pointer(chk->my_area,c,rn);

		//due to being on top, slopes are irrelevent. check for all contact
		if((*my_tile_pointer)&14) {
			//push out to there
			chk->top_y= ( ((UINT16)ro)<<7); //push_m
			//indicate top hit
			chk->hit_flags= HIT_TOP;
			//and done.
			break;
		}
	}
	//this is when it hits the top of the map!
	else {
		//push out to there
		chk->top_y= 0; //it's just the top.
		//indicate top hit
		chk->hit_flags= HIT_TOP;
		chk->chk_flags|=HIT_TOP;
	}
	}
	//if there's no bevel then that's gonna be it.
	if(!chk->bevel) {
		return;
	}

	//otherwise continue.

	//now push top-right!
	push_v= ( ((UINT16)ro<<3)-((UINT16)c2<<3)-chk->bevel +chk->w <<4 ) +chk->left_x-1; //right bevel hits

	// only do this if it helps push-out.
	if(!(push_v-chk->top_y &32768UL)) {//push_v > top_y; helps pushout
	r=ro;
	c= (UINT8)((chk->left_x-1 >>4)+ chk->w >>3)+1;
	++c2;
	// NOTE: c2 is our counter now, due to optimization
	while(c2!=c) {
		my_tile_pointer= get_prop_pointer(chk->my_area,c2,r);

		//once again slopes are irrelevent.
		if((*my_tile_pointer)&14) {
			if(chk->top_y==push_v) {
				chk->hit_flags|= HIT_T_RIGHT;
			} else {
				chk->top_y= push_v;
				chk->hit_flags= HIT_T_RIGHT;
			}
			//and done.
			break;
		}
		//counter part of the for loop.
		++c2;
		++r;
	}
	}

	//now push top-left!
	push_v= ( ((UINT16)ro<<3)+((UINT16)c1+1<<3)-chk->bevel <<4 ) -chk->left_x; //left bevel hits
	// skip all this if it won't help push-out
	if(push_v-chk->top_y &32768UL) {//push_v < top_y; if it's not helping pushout
		return;
	}
	r=ro;
	c= (UINT8)(chk->left_x>>7) -1;
	--c1;
	//c1 is counter due to effickency
	while(c1!=c) {
		my_tile_pointer= get_prop_pointer(chk->my_area,c1,r);

		//once again slopes are irrelevent.
		if((*my_tile_pointer)&14) {
			if(chk->top_y==push_v) {
				chk->hit_flags|= HIT_T_LEFT;
			} else {
				chk->top_y= push_v;
				chk->hit_flags= HIT_T_LEFT;
			}
			//and done.
			return; //would be break; but it's the end of the method
		}
		//counter part of the for loop.
		--c1;
		++r;
	}
	
	//done.
}

static void down_coll(struct chunk * chk, INT8 vy) {
	//ACTUAL MOVE!
	old_v= chk->top_y;
	chk->top_y+=(INT16)vy;
	new_v= chk->top_y;

	// get columns and rows
	c1= (UINT8)((chk->left_x>>4)+ chk->bevel >>3);
	c2= (UINT8)((chk->left_x-1 >>4)+ chk->w -chk->bevel >>3);
	 //IRRESPONSIBLE USE OF CO AND CN; they're slightly offset c1 and c2
	co= (UINT8)((chk->left_x-1 >>4)+ chk->bevel >>3);
	cn= (UINT8)((chk->left_x >>4)+ chk->w -chk->bevel >>3);
	//row we're on; row we were
	rn= (UINT8)((new_v-1 >>4)+chk->h >>3);
	ro= (UINT8)((old_v-1 >>4)+chk->h >>3);

	//temp flag variables.
	new_hit_flags=0;
	//moveflags is initialized later

	//push bottomish
	// if we're in the same row then we can skip this part entirely
	if(rn!=ro) {
	if(rn != chk->my_area->map_rows) for(c=c1;c!=c2+1;++c) {
		//what tile is in the square we pushed into?
		my_tile_pointer= get_prop_pointer(chk->my_area,c,rn);

		//check for a point of contact
		if((*my_tile_pointer)&14) {
			//if it's a slope on the edge of the edge, ignore because it's a corner issue.
			if(c==c1 && c1==co &&(*my_tile_pointer&8))
				continue;
			if(c==c2 && c2==cn &&(*my_tile_pointer&4))
				break;

			//push out to there
			chk->top_y= ( ((UINT16)rn<<3)-chk->h <<4  );
			//indicate bottom hit
			new_hit_flags= HIT_BOT;
			//and done.
			break;
		}
	}
	//very bottom row
	else {
		//push out to there
		chk->top_y= ( ((UINT16)rn<<3)-chk->h <<4  ); //the bottom!
		//indicate top hit
		new_hit_flags= HIT_BOT;
		chk->chk_flags|=HIT_BOT;
	}
	}

	//now push bottom-right!
	
	//all ROW variables are now for the sorta diagonal rows.
	an= (new_v+chk->left_x-1 >>4)-chk->bevel +chk->w+chk->h >>3;
	ao= (old_v+chk->left_x-1 >>4)-chk->bevel +chk->w+chk->h >>3;
	
	//skip if it's the same row
	if(an==ao)
		goto botl;
	//where to push out to
	push_v= ( ((UINT16)an<<3)+chk->bevel -chk->w-chk->h <<4 ) -chk->left_x; //right bevel hits
	// skip all this if it won't help push-out
	if(chk->top_y-push_v &32768UL) { //top_y < push_v; if it's not helping pushout
		goto botl;
	}
	//do the one corner piece regardless of bevel
	c=cn;
	r=ao-cn;
	my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
	if((*my_tile_pointer)&4) {
		//collided!
		chk->top_y= push_v;
		new_hit_flags= HIT_B_RIGHT;
		goto botl;
	}

	//the rest of the possible collision tiles
	// this is the rightmost col of the chk;
	c3= (UINT8)((chk->left_x-1 >>4)+ chk->w >>3);
	//and this is the loop.
	while(c!=c3) {
		++c;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&10) {
			chk->top_y= push_v;
			new_hit_flags= HIT_B_RIGHT;
			goto botl;
		}
		--r;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&4) {
			chk->top_y= push_v;
			new_hit_flags= HIT_B_RIGHT;
			goto botl;
		}

	}
	//special case for bevel/side split down the middle of a tile
	if(c2!=cn && rn!=ro && !(new_hit_flags&2U)) {
		my_tile_pointer= get_prop_pointer(chk->my_area,cn,rn);
		if((*my_tile_pointer)&10) {
			chk->top_y= push_v;
			new_hit_flags= HIT_B_RIGHT;
			goto botl;
		}
	}

	//NOW FOR THE FREAKING BOTTOMLEFT!!!!
	botl:
	//TODO occasionally hugs bottom left corner when leaving platform
	//all ROW variables are now for the sorta diagonal rows.
	an= ((new_v-1 -chk->left_x >>4)-chk->bevel +chk->h >>3);
	ao= ((old_v-1 -chk->left_x >>4)-chk->bevel +chk->h >>3);
	
	//skip if it's the same row
	if(an==ao) {
		chk->hit_flags |= new_hit_flags;
		return;
	}
	//where to push out to
	push_v= ( ((UINT16)an<<3)+chk->bevel -chk->h <<4 ) +chk->left_x;
	// skip all this if it won't help push-out
	if(chk->top_y-push_v &32768UL) { //top_y < push_v; if it's not helping pushout
		chk->hit_flags |= new_hit_flags;
		return;
	}
	//do the one corner piece regardless of bevel
	c=co;
	r=an+co;
	my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
	if((*my_tile_pointer)&8) {
		//collided!
		chk->top_y= push_v;
		new_hit_flags= HIT_B_LEFT;
		chk->hit_flags |= new_hit_flags;
		return;
	}

	//the rest of the possible collision tiles
	// this is the rightmost col of the chk;
	c3= (UINT8)(chk->left_x >>7);
	//and this is the loop.
	while(c!=c3) {
		--c;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&6) {
			chk->top_y= push_v;
			new_hit_flags= HIT_B_LEFT;
			chk->hit_flags |= new_hit_flags;
			return;
		}

		--r;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&8) {
			chk->top_y= push_v;
			new_hit_flags= HIT_B_LEFT;
			chk->hit_flags |= new_hit_flags;
			return;
		}

	}
	//special case for bevel/side split down the middle of a tile
	if(c1!=co && rn!=ro && !(new_hit_flags&2U)) {
		my_tile_pointer= get_prop_pointer(chk->my_area,co,rn);
		if((*my_tile_pointer)&6) {
			chk->top_y= push_v;
			new_hit_flags= HIT_B_LEFT;
			//these two lines are redundant, as per end of method
			// chk->hit_flags |= new_hit_flags;
			// return;
		}
	}

	//done.
	chk->hit_flags |= new_hit_flags;
}

static void left_coll(struct chunk * chk, INT8 vx) {
	//ACTUAL MOVE!
	old_v= chk->left_x;
	chk->left_x+=(INT16)vx;
	new_v= chk->left_x;

	// get columns and rows
	r1= (UINT8)((chk->top_y>>4)+ chk->bevel >>3);
	r2= (UINT8)((chk->top_y-1 >>4)+ chk->h -chk->bevel >>3);
	 //IRRESPONSIBLE USE OF RO AND RN; they're slightly offset r1 and r2
	//ro= (UINT8)((chk->top_y-1 >>4)+ chk->bevel >>3);
	rn= (UINT8)((chk->top_y >>4)+ chk->h -chk->bevel >>3);
	//col we're on; col we were
	cn= (UINT8)(new_v >>7);
	co= (UINT8)(old_v >>7);

	//temp flag variables.
	new_hit_flags=0;
	//moveflags is initialized later

	//push SIDE
	// if we're in the same row then we can skip this part entirely
	if(cn!=co) {
	if(co) for(r=r1;r!=r2+1;++r) {
		//what tile is in the square we pushed into?
		my_tile_pointer= get_prop_pointer(chk->my_area,cn,r);

		//check for a point of contact
		if((*my_tile_pointer)&14) {
			//if it's a slope on the edge of the edge, ignore because it's a corner issue.
			if(r==r2 && r2==rn &&(*my_tile_pointer&8))
				break;

			//push out to there
			chk->left_x= ((UINT16)co<<7 );
			//indicate bottom hit
			new_hit_flags= HIT_LEFT;
			//and done.
			break;
		}
	}
	//this is when it hits the top of the map!
	else {
		//push out to there
		chk->left_x= 0; //it's just the top.
		//indicate top hit
		new_hit_flags= HIT_LEFT;
		chk->chk_flags|=HIT_LEFT;
	}
	}

	//now push bottom-LEFT!
	
	//diagonal row variables!
	an= (chk->top_y-new_v -1 >>4);
	ao= (chk->top_y-old_v -1 >>4);
	if(an&2048U) an|= 61440U;//keeping signedness because meh
	if(ao&2048U) ao|= 61440U;
	an= an-chk->bevel +chk->h >>3;
	ao= ao-chk->bevel +chk->h >>3;
	if(an&4096U) an|= 61440U;
	if(ao&4096U) ao|= 61440U;
	
	//skip if it's the same row
	if(an==ao) {
		chk->hit_flags |= new_hit_flags;
		return;
	}
	//where to push out to
	push_v= (chk->h-chk->bevel -(an<<3) <<4 ) +chk->top_y;
	
	// skip all this if it won't help push-out
	if(push_v-chk->left_x &32768UL) { //if it's not helping pushout
		chk->hit_flags |= new_hit_flags;
		return;
	}
	//do the one corner piece regardless of bevel
	r=rn;
	c=rn-an;
	my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
	if((*my_tile_pointer)&8) {
		//collided!
		chk->left_x= push_v;
		new_hit_flags= HIT_B_LEFT;
		chk->hit_flags = new_hit_flags;
		return;
	}

	//the rest of the possible collision tiles
	// this is the rightmost col of the chk;
	r3= (UINT8)((chk->top_y-1 >>4)+ chk->h >>3);
	//and this is the loop.
	while(r!=r3) {
		++r;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&6) {
			chk->left_x= push_v;
			new_hit_flags= HIT_B_LEFT;
			chk->hit_flags |= new_hit_flags;
			return;
		}
		++c;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&8) {
			chk->left_x= push_v;
			new_hit_flags= HIT_B_LEFT;
			chk->hit_flags |= new_hit_flags;
			return;
		}

	}
	//special case for bevel/side split down the middle of a tile
	if(r2!=rn && cn!=co && !(new_hit_flags&4U)) {
		my_tile_pointer= get_prop_pointer(chk->my_area,cn,rn);
		if((*my_tile_pointer)&6) {
			chk->left_x= push_v;
			new_hit_flags= HIT_B_LEFT;
			//THE FOLLOWING LINES ARE REDUNDANT; therefore not included.
			//(they happen at the bottom anyway)
			// chk->hit_flags |= new_hit_flags;
			// return;
		}
	}
	//done.
	chk->hit_flags |= new_hit_flags;
}

//horizonal part of collision physics
static void right_coll(struct chunk * chk, INT8 vx) {
	//ACTUAL MOVE!
	old_v= chk->left_x;
	chk->left_x+=(INT16)vx;
	new_v= chk->left_x;

	// get columns and rows
	r1= (UINT8)((chk->top_y>>4)+ chk->bevel >>3);
	r2= (UINT8)((chk->top_y-1 >>4)+ chk->h -chk->bevel >>3);
	 //IRRESPONSIBLE USE OF CO AND CN; they're slightly offset c1 and c2
	//ro= (UINT8)((chk->top_y-1 >>4)+ chk->bevel >>3);
	rn= (UINT8)((chk->top_y >>4)+ chk->h -chk->bevel >>3);
	//row we're on; row we were
	cn= (UINT8)((new_v-1 >>4)+chk->w >>3);
	co= (UINT8)((old_v-1 >>4)+chk->w >>3);

	//temp flag variables.
	new_hit_flags=0;
	//moveflags is initialized later

	//push bottomish
	// if we're in the same row then we can skip this part entirely
	if(cn!=co) {
	if(cn != chk->my_area->map_cols) for(r=r1;r!=r2+1;++r) {
		//what tile is in the square we pushed into?
		my_tile_pointer= get_prop_pointer(chk->my_area,cn,r);

		//check for a point of contact
		if((*my_tile_pointer)&14) {
			//if it's a slope on the edge of the edge, ignore because it's a corner issue.
			if(r==r2 && r2==rn &&(*my_tile_pointer&4))
				break;

			//push out to there
			chk->left_x= ( ((UINT16)cn<<3)-chk->w <<4 );
			//indicate bottom hit
			new_hit_flags= HIT_RIGHT;
			//and done.
			break;
		}
	}
	//very rightmost
	else {
		//push out to there
		chk->left_x= ( ((UINT16)cn<<3)-chk->w <<4 ); //the bottom!
		//indicate top hit
		new_hit_flags= HIT_RIGHT;
		chk->chk_flags|=HIT_RIGHT;
	}
	}

	//now push bottom-right!
	
	//diagonal row variables!
	an= (new_v+chk->top_y -1 >>4)-chk->bevel +chk->h+chk->w >>3;
	ao= (old_v+chk->top_y -1 >>4)-chk->bevel +chk->h+chk->w >>3;
	
	//skip if it's the same row
	if(an==ao) {
		chk->hit_flags |= new_hit_flags;
		return;
	}
	//where to push out to
	push_v= ( ((UINT16)an<<3)+chk->bevel -chk->h-chk->w <<4 ) -chk->top_y; //right bevel hits
	// skip all this if it won't help push-out
	if(chk->left_x-push_v &32768UL) { //top_y < push_v; if it's not helping pushout
		chk->hit_flags |= new_hit_flags;
		return;
	}
	//do the one corner piece regardless of bevel
	r=rn;
	c=ao-rn;
	my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
	if((*my_tile_pointer)&4) {
		//collided!
		chk->left_x= push_v;
		new_hit_flags= HIT_B_RIGHT;
		chk->hit_flags = new_hit_flags;
		return;
	}

	//the rest of the possible collision tiles
	// this is the rightmost col of the chk;
	r3= (UINT8)((chk->top_y-1 >>4)+ chk->h >>3);
	//and this is the loop.
	while(r!=r3) {
		++r;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&10) {
			chk->left_x= push_v;
			new_hit_flags= HIT_B_RIGHT;
			chk->hit_flags |= new_hit_flags;
			return;
		}
		--c;
		my_tile_pointer= get_prop_pointer(chk->my_area,c,r);
		if((*my_tile_pointer)&4) {
			chk->left_x= push_v;
			new_hit_flags= HIT_B_RIGHT;
			chk->hit_flags |= new_hit_flags;
			return;
		}

	}
	//special case for bevel/side split down the middle of a tile
	if(r2!=rn && cn!=co && !(new_hit_flags&8U)) {
		my_tile_pointer= get_prop_pointer(chk->my_area,cn,rn);
		if((*my_tile_pointer)&10) {
			chk->left_x= push_v;
			new_hit_flags= HIT_B_RIGHT;
			//THE FOLLOWING LINES ARE REDUNDANT; therefore not included.
			//(they happen at the bottom anyway)
			// chk->hit_flags |= new_hit_flags;
			// return;
		}
	}
	//done.
	chk->hit_flags |= new_hit_flags;

}

//water check!
void water_check(struct chunk * chk) {
	r= (UINT8)((chk->top_y-1 >>3)+ chk->h >>4); //>>3 and >>4 are swapped; he must be at least half way in water.
	c= (UINT8)(chk->left_x>>7); //exact column is irrelevent.
	if(*get_prop_pointer(chk->my_area,c,r)&1) //is in water
		chk->chk_flags |= FLAG_WATER;
	else
		chk->chk_flags &= ~FLAG_WATER;
}

//moves a chunk, always ENSURES collision physics.
void scroll_chunk_coll(struct chunk * chk, INT8 vx, INT8 vy) {
	//where we'll examine the properties
	SWITCH_ROM_MBC1( chk->my_area->props_bank );

	//reset block flags.
	chk->hit_flags=0;
	chk->chk_flags&=~ HIT_SIDES;

	//don't worry about x...
	if(!vx) {
		if(!vy) return;//nothinG!
		//only y stuff
		if(vy&128U) up_coll(chk,vy);
		else down_coll(chk,vy);
	}
	//don't worry about y...
	else if(!vy) {
		//only x stuff
		if(vx&128U) left_coll(chk,vx);
		else right_coll(chk,vx);
	}
	//up then over for going up.
	else if(vy&128U) {
		//y first then x
		up_coll(chk,vy);
		//see if we can skip x at this part
		if(vx&128U) {
			if(!(chk->hit_flags& 16U)) {
				if(vx&128U) left_coll(chk,vx);
				else right_coll(chk,vx);
			}
		} else if(!(chk->hit_flags& 32U)) {
			if(vx&128U) left_coll(chk,vx);
			else right_coll(chk,vx);
		}
	}
	//over then down for going down. (ASSISTS SLOPE)
	else {
		//for this one, X first and THEN y
		if(vx&128U) left_coll(chk,vx);
		else right_coll(chk,vx);	
		//see if we skip y
		if(vx&128U) {
			if(!(chk->hit_flags& 64U))
				down_coll(chk,vy);
		} else if(!(chk->hit_flags& 128U))
			down_coll(chk,vy);
	}
	//remove chk flags which are eliminated from the hit flags
	chk->chk_flags &= ( ~HIT_SIDES +(HIT_SIDES&chk->hit_flags) );

	//now change velocities according to block flags!
	if(chk->hit_flags& 144U) {//top left + bottom right
		if(chk->vy&128U) {
			chk->vy= -chk->vy;
			chk->vy= -(chk->vy >>1);
		} else {
			chk->vy>>=1;
		}
		if(chk->vx&128U) {
			chk->vx=  -chk->vx;
			chk->vy+= chk->vx >>1;
		} else {
			chk->vy-= chk->vx>>1;
		}
		chk->vx= -chk->vy;
	}
	if(chk->hit_flags& 96U) {//top right + bottom left
		if(chk->vy&128U) {
			chk->vy= -chk->vy;
			chk->vy= -(chk->vy >>1);
		} else {
			chk->vy>>=1;
		}
		if(chk->vx&128U) {
			chk->vx=  -chk->vx;
			chk->vy-= chk->vx >>1;
		} else {
			chk->vy+= chk->vx>>1;
		}
		chk->vx= chk->vy;
	}
	
	if(chk->hit_flags& 3U) {// up/down
		chk->vy=0;
	}
	if(chk->hit_flags& 12U) {// l/r
		chk->vx=0;
	}

	//FINALLY check for water collisions
	water_check(chk);
}

//moves a chunk, always IGNORES collision physics
void scroll_chunk_nocoll(struct chunk * chk, INT8 x, INT8 y) {
	//move ze verti
	chk->top_y+=(signed int)y;
	//move ze things horiz
	chk->left_x+=(signed int)x;
}

//moves a chunk a certain horizontal and vertical distance
void scroll_chunk(struct chunk * chk, INT8 x, INT8 y) {
	if(chk->transp)
		scroll_chunk_coll(chk,x,y);
	else
		scroll_chunk_nocoll(chk,x,y);
}

//hey this is a seeeecret test thing.
//set_bkg_tiles((UINT8)(c&31U),(UINT8)(r&31U), 1,1, 0x5010);
