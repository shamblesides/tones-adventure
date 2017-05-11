#include <gb/gb.h>

#include "chunks.h"
#include "areas.h"

extern UINT16 cam_x, cam_y;

void set_cam(struct area * new_area, struct chunk * new_focus);
void set_cam_focus(struct chunk * new_focus);
void cam_act();
