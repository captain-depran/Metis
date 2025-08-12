#include "../Headers/body.h"
#include "../Headers/vector3D.h"
#include "../Headers/spacecraft.h"
#include "../Headers/kep_cart.h"
#include <iostream>


manouver::manouver(vector3D dv_,std::string label_,double time_){
    dv=dv_;
    label=label_;
    time=time_;
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
    mnvr.executed_time=current_t;
    mnvr.executed=true; // Mark the manouver as executed
    complete_manouvers.push_back(mnvr); // Store the executed manouver
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

void spacecraft::situation_update(std::vector<body> &system,bool kep_check){
    current_max_grav = 0; // Reset the maximum gravitational effect
    if(kep_check){
        orbit_params = cart_to_kep(system[dominant_body_index], pos, vel); // Update the orbital parameters based on the dominant body
        };
    }