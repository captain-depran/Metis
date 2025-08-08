#ifndef FRAME_TOOLS_H
#define FRAME_TOOLS_H

#include "../Headers/vector3D.h"

vector3D rot_frame(vector3D& rotator_pos, vector3D& obj_pos);
void frame_center(int reference_id, int target_id,int entries);
void frame_swap(int reference_id, int target_id, int out_id,int entries);

#endif