/*
 * TILE_MEM.H
 *
 * Manages storage of tilesets in VRAM.
 * Implements a stack structure.
 *
 * TODO: Things can be removed
 * from the middle of the stack though, arbitrarily;
 * But, this can be slow as all the things on top of it
 * are copied down. It's not recommended.
 *
 */

UINT8 push_BG_tiledata(char * set, UINT8 length);

UINT8 push_OBJ_tiledata(char * set, UINT8 length);

void reset_BG_stack();

void reset_OBJ_stack();
