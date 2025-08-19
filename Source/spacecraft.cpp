#include "../Headers/body.h"
#include "../Headers/vector3D.h"
#include "../Headers/spacecraft.h"
#include "../Headers/kep_cart.h"
#include "../Headers/event_detect.h"
#include <iostream>


manouver::manouver(vector3D dv_,std::string label_,int trigger_type_, trigger_params trig_conditions_){
    dv=dv_;
    label=label_;
    trigger_type=trigger_type_;
    trig_conditions=trig_conditions_;
    is_coast=false;
};

manouver::manouver(std::string label_,int trigger_type_, trigger_params trig_conditions_){
    label=label_;
    trigger_type=trigger_type_;
    trig_conditions=trig_conditions_;
    is_coast=true;
};

void spacecraft::assign_sys_map(std::map<std::string,int> passed_map){
    sys_index_map=passed_map;
};

vector3D spacecraft::inertial_dv(manouver mnvr, vector3D parent_pos, vector3D parent_vel){
    vector3D r_hat = (pos-parent_pos).unit();          // Radial direction
    vector3D t_hat = (vel-parent_vel).unit();          // Along-track
    vector3D n_hat = r_hat.cross(t_hat).unit(); // Out-of-plane

    vector3D dv_inertial =
        (r_hat * mnvr.dv.y) +
        (t_hat * mnvr.dv.x) +
        (n_hat * mnvr.dv.z);

    return dv_inertial;
}

void spacecraft::perform_manouver(manouver& mnvr,double& current_t){
    vector3D dv_inertial = inertial_dv(mnvr, parent_pos, parent_vel);
    vel=vel+dv_inertial;
    mnvr.executed_time=last_event_time=current_t;
    mnvr.executed=true; // Mark the manouver as executed
    complete_manouvers.push_back(mnvr); // Store the executed manouver
    next_mnvr_index++; // Move to the next manouver
}
void spacecraft::sum_grav(body const& attrac){
    bool same_body=(attrac.id!=id);
    if(same_body){
        vector3D grav=grav_accel(attrac);
        grav_result = grav_result + grav;
        if (grav.mag() > current_max_grav){
            dominant_body_id = attrac.id; // Update the dominant body if this one has a stronger gravitational effect
            dominant_body_index = attrac.system_index; // Update the index of the dominant body
            parent_pos = attrac.pos; // Update the position of the dominant body
            parent_vel = attrac.vel; // Update the velocity of the dominant body
            current_max_grav = grav.mag();
        }
        bodies_felt++;
    };
}

void spacecraft::print_orbit(std::vector<body> &system){
    kep_state orbital_params=cart_to_kep(system[dominant_body_index],pos,vel);
    std::cout<<"SEMI-MAJOR AXIS: "<<orbital_params.semi_maj<<std::endl;
    std::cout<<"ECCENTRICITY: "<<orbital_params.ecc<<std::endl;
    std::cout<<"INCLINATION: "<<orbital_params.inc<<std::endl;
    std::cout<<"PARENT BODY: "<<system[dominant_body_index].name<<std::endl;
};

//Browses the spacecraft's events list and detects if an event is due to be triggered. Also, housekeeping
void spacecraft::situation_update(std::vector<body> &system, double& current_t){
    current_max_grav = 0; // Reset the maximum gravitational effect
    int tgt_index;
    int parent_index;
    if(next_mnvr_index<=max_mnvr_index){
        manouver& mnvr= all_manouvers[next_mnvr_index];
        if (!mnvr.executed){
            switch (mnvr.trigger_type){
                case 1: // Time since last event
                    if (check_time(current_t-last_event_time, mnvr.trig_conditions.trigger_time)){
                        perform_manouver(mnvr,current_t);
                    };
                    break;
                case 2: // Close Approach
                    if (check_close_approach(pos,vel,dominant_body_index,system,mnvr.trig_conditions.CLA_threshold)){
                        perform_manouver(mnvr,current_t);
                    };
                    break;
                case 3: // Turnback towards body
                    if (check_turnback(pos,vel,dominant_body_index,system)){
                        perform_manouver(mnvr,current_t);
                    };
                    break;
                case 4: //True Anomaly
                    if (check_anomaly(mnvr.trig_conditions.tgt_anom,pos,vel,dominant_body_index,system)){
                        perform_manouver(mnvr,current_t);
                    };
                    break;
                case 5: //Grav Dominant body Swap
                    if (mnvr.trig_conditions.last_dom_body_index!=-1 //-1 indicates starting conditions
                        && check_bodyswap(dominant_body_index,mnvr.trig_conditions.last_dom_body_index)){ 
                        perform_manouver(mnvr,current_t);
                    }
                    mnvr.trig_conditions.last_dom_body_index = dominant_body_index;
                    break;
                case 6: //Relative Phase angle
                    // NEED TO STREAMLINE EXTRACTION/REFERENCE TO CENTRAL ORBITAL BODY FOR SELF AND TARGET
                    tgt_index=sys_index_map[mnvr.trig_conditions.tgt_name];
                    parent_index=sys_index_map[mnvr.trig_conditions.parent_name];
                    if (check_phase_angle(mnvr.trig_conditions.tgt_phase_angle,pos,system[tgt_index].pos,system[parent_index].pos)){
                        perform_manouver(mnvr,current_t);
                    };
                    break;
                
                default:
                    std::cout<<"ERROR: Unknown Trigger"<<std::endl;
                    break;

            }
        }
    }   
}


    //orbit_params = cart_to_kep(system[dominant_body_index], pos, vel); // Update the orbital parameters based on the dominant body