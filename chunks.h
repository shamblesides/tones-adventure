/*
 * CHUNKS.H
 *
 * Offers a struct which stores info about
 * solid dynamic objects, called "chunks."
 * Provides methods for handling collisions
 * and movement.
 *
 */

#ifndef CHUNKS_H
#define CHUNKS_H

#include <gb/gb.h>
#include "areas.h"

//a chunk is a solid object with volume and some sprites
struct chunk {
	UINT16 left_x, top_y;	// by subpixels (/16)
	UINT8 w,h,bevel;	// by pixels	(1)
	INT8 vx, vy;		// by subpixels (/16)
	UINT8 transp; 		// whether it goes through walls!
	UINT8 first_sprite;	// which sprites this chunk will use
	UINT8 num_sprites;
	UINT8 tileset_loc;	// tileset offset in vram
	INT8 x_off, y_off;	// by pixels (1) sprite drawing offset
	struct area * my_area;
	UINT8 hit_flags;	// where the object is touching something
	UINT8 chk_flags;	// misc flags;
				// |1
				// |2
				// |4
				// |8	= hit edge of the lvl
				// |16	= facing_right
				// |32	= in_water
				// ...  reserved
	//19 bytes
	void (*act)(struct chunk * chk); // act function for the chunk
	//21
	UINT8 data[3];		// misc data
	//24
	
};

//hit flag definitions
#define HIT_TOP 1U
#define HIT_BOT 2U
#define HIT_LEFT 4U
#define HIT_RIGHT 8U
#define HIT_T_LEFT 16U
#define HIT_T_RIGHT 32U
#define HIT_B_LEFT 64U
#define HIT_B_RIGHT 128U
#define HIT_SIDES \
	(HIT_TOP+HIT_BOT+HIT_LEFT+HIT_RIGHT)

//misc flags
#define FACING_RIGHT 16U
#define FLAG_WATER 32U

//moves a chunk based on its current velocities
#define GO_CHUNK(w) \
  scroll_chunk((w),(w)->vx,(w)->vy)

//moves a chunk with x and y manually specified
void scroll_chunk(struct chunk * chk, INT8 x, INT8 y);

//methods for moving a chunk manually, overriding the transp flag
void scroll_chunk_nocoll(struct chunk * chk, INT8 x, INT8 y);
void scroll_chunk_coll(struct chunk * chk, INT8 vx, INT8 vy);


#endif
