#include <gb/gb.h>

static UINT8 bg_stack=0;
static UINT8 obj_stack=0;

//NOTE: be sure to switch to the proper bank!
UINT8 push_BG_tiledata(char * set, UINT8 length) {
	set_bkg_data(bg_stack,length,set);
	bg_stack+= length;
	return bg_stack-length;
}

UINT8 push_OBJ_tiledata(char * set, UINT8 length) {
	set_sprite_data(obj_stack,length,set);
	obj_stack+= length;
	return obj_stack-length;
}

void reset_BG_stack() {
	bg_stack=0;
}

void reset_OBJ_stack() {
	obj_stack=0;
}
