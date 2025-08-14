#include "../Headers/body.h"
#include "../Headers/event_detect.h"
#include "../Headers/kep_cart.h"
#include "../Headers/vector3D.h"
#include "../Headers/spacecraft.h"

#include <iostream>
#include <vector>
#include <string>


/*
TRIGGER TYPES
1 - Time Trigger
2 - Close Approach Trigger (Vehicle must already be approaching the body)
3 - Turnback Trigger (Vehicle is moving towards the body)
4 - True Anomaly Trigger
*/


bool check_time(double current_t, double trigger_time){
    if (current_t>=trigger_time){
        return true;
    }
    else{
        return false;
    };
};

bool check_close_approach(vector3D craft_pos, vector3D craft_vel,int dom_body_index, std::vector<body>& system, double threshold){
    vector3D rel_r= craft_pos - system[dom_body_index].pos;
    vector3D rel_v= craft_vel - system[dom_body_index].vel;
    if (rel_r.mag() < threshold){
        double dot_check = rel_r.dot(rel_v);
        std::cout<<dot_check<<std::endl;
        if (dot_check >= 0){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    };
};

bool check_turnback(vector3D craft_pos, vector3D craft_vel,int dom_body_index, std::vector<body>& system){
    vector3D rel_r= craft_pos - system[dom_body_index].pos;
    vector3D rel_v= craft_vel - system[dom_body_index].vel;
    double dot_check = rel_r.dot(rel_v);
    if (dot_check <= 0){
        return true;
    }
    else{
        return false;
    }
};

bool check_anomaly(double tgt_anom, vector3D craft_pos, vector3D craft_vel,int dom_body_index, std::vector<body>& system){
    vector3D rel_r= craft_pos - system[dom_body_index].pos;
    vector3D rel_v= craft_vel - system[dom_body_index].vel;
    double dot_check = rel_r.dot(rel_v);
    double anom_buffer=1;
    bool feasible=false;

    if (tgt_anom>=180 && dot_check<=0){
        feasible = true;
    }
    else if (tgt_anom<180 && dot_check>0){
        feasible = true;
    }
    else{return false;};

    if (feasible){
        kep_state orb_elements=cart_to_kep(system[dom_body_index],craft_pos, craft_vel);
        if (orb_elements.true_anom-tgt_anom>0 && orb_elements.true_anom-tgt_anom<=anom_buffer){
            return true;
        }
        else{return false;};
    }
    else{return false;};

};