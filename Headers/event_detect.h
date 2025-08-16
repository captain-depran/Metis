#ifndef EVENT_DETECT_H
#define EVENT_DETECT_H

#include <vector>
#include "../Headers/body.h"

struct trigger_params{
    double trigger_time; //Used for triggering events x seconds after last event
    double CLA_threshold; //Distance within which a 'close approach' to a body can occur
    double tgt_dom_body_id; //target body to detect transfer to being gravitationally significant
    double tgt_anom; //Target true anomaly 
    double last_dom_body_index; //Stores the dominant body from last step (to detect change)
};


bool check_time(double current_t, double trigger_time);
bool check_close_approach(vector3D craft_pos, vector3D craft_vel,int dom_body_index, std::vector<body>& system, double threshold);
bool check_turnback(vector3D craft_pos, vector3D craft_vel,int dom_body_index, std::vector<body>& system);
bool check_anomaly(double tgt_anom, vector3D craft_pos, vector3D craft_vel,int dom_body_index, std::vector<body>& system);
bool check_bodyswap(int dom_body_index,int last_index);

#endif