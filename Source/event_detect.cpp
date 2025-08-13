#include "../Headers/body.h"
#include "../Headers/event_detect.h"
#include "../Headers/kep_cart.h"
#include "../Headers/vector3D.h"
#include "../Headers/spacecraft.h"

#include <vector>
#include <string>
#include <unordered_map>


enum trigger_type{MNV,WAIT,UNKNOWN};
std::unordered_map<std::string, trigger_type> event_map{
    {"TIME", MNV},
    {"WAIT", WAIT},
    {"UNKNOWN", UNKNOWN}
};


bool check_time(spacecraft& craft, std::vector<body>& system, double current_t, double trigger_time){
    if (current_t>=trigger_time){
        return true;
    }
    else{
        return false;
    };
};

bool check_close_approach(spacecraft& craft, std::vector<body>& system, double threshold){
    vector3D rel_r= craft.pos - system[craft.dominant_body_index].pos;
    vector3D rel_v= craft.vel - system[craft.dominant_body_index].vel;
    if (rel_r.mag() < threshold){
        double dot_check = rel_r.dot(rel_v);
        if (dot_check < 0){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    };
}


