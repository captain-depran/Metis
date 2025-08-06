#include "../Headers/body.h"
#include "../Headers/vector3D.h"
#include "../Headers/spacecraft.h"
#include <iostream>

manouver::manouver(vector3D dv_,std::string label_,double time_){
    dv=dv_;
    label=label_;
    time=time_;
};

vector3D spacecraft::inertial_dv(manouver mnvr, body const& parent){
    vector3D r_hat = (pos-parent.pos).unit();          // Radial direction
    vector3D t_hat = (vel-parent.vel).unit();          // Along-track
    vector3D n_hat = r_hat.cross(t_hat).unit(); // Out-of-plane

    vector3D dv_inertial =
        (r_hat * mnvr.dv.y) +
        (t_hat * mnvr.dv.x) +
        (n_hat * mnvr.dv.z);

    return dv_inertial;

}

void spacecraft::perform_manouver(manouver mnvr, body const& parent){
    vector3D dv_inertial = inertial_dv(mnvr, parent);
    vel=vel+dv_inertial;
    std::cout<<name<<": PERFORMED MANOUVER "<<mnvr.label<<std::endl;
}