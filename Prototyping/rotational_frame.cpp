#include <iostream>
#include <cmath>
#include "../Headers/vector3D.h"
using namespace std;
#define PI 3.14159265

vector3D rot_frame(vector3D& rotator_pos, vector3D& obj_pos){
    vector3D translated_pos;
    double theta = atan2(rotator_pos.y,rotator_pos.x);
    translated_pos.z=obj_pos.z;
    translated_pos.x=(obj_pos.x*cos(-1*theta))-(obj_pos.y*sin(-1*theta));
    translated_pos.z=(obj_pos.x*sin(-1*theta))+(obj_pos.y*cos(-1*theta));
    return translated_pos;
};
int main(){
    vector3D earth_pos(0,0,0);
    vector3D moon_pos(3.84e8,0,0);
    vector3D sat_pos(2.4e7,1.2e7,0);

    vector3D rot_pos=rot_frame(moon_pos,sat_pos);

    rot_pos.print();
    return 0;
};
