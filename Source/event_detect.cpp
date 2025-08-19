#include "../Headers/body.h"
#include "../Headers/event_detect.h"
#include "../Headers/kep_cart.h"
#include "../Headers/vector3D.h"
#include "../Headers/spacecraft.h"
#include "../Headers/constants.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>


/*
TRIGGER TYPES
1 - Time Trigger
2 - Close Approach Trigger (Vehicle must already be approaching the body)
3 - Turnback Trigger (Vehicle is moving towards the body)
4 - True Anomaly Trigger
5 - Dominant Attractive Body change Trigger
6 - Relative phase angle trigger
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

bool check_bodyswap(int dom_body_index,int last_index){
    if (dom_body_index!=last_index){
        return true;
    }
    else{
        return false;
    };
};

bool check_phase_angle(double tgt_angle, vector3D pos, vector3D other_body_pos, vector3D parent_pos){
    double error_buffer = 2; //Half a degree tolerance for phase angle

    vector3D self_rel=pos-parent_pos;
    vector3D other_rel=other_body_pos-parent_pos;


    double self_angle=atan2(self_rel.y,self_rel.x);
    double other_angle=atan2(other_rel.y,other_rel.x);

    double phase=other_angle-self_angle;
    //std::cout<<phase/PI<<std::endl;
    if (phase > PI) {
        phase -= 2*PI;
    }
    else if (phase < -PI) {
        phase += 2*PI;
    };

    phase=phase*(180/PI);
    if (fabs(phase-tgt_angle)<= error_buffer){
        return true;
    }
    else{
        return false;
    };
}