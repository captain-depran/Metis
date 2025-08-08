#include "../Headers/body.h"
#include "../Headers/vector3D.h"
#include "../Headers/constants.h"
#include <vector>


body::body(int id_,double mass_,vector3D r, vector3D v){
    pos_log.resize(1);
    id=id_;
    mass=mass_;
    pos=r;
    vel=v;
    grav_result.x=grav_result.y=grav_result.z=0;
    bodies_felt=0;
    pos_log[0]=pos;
}

        
body::body(int id_,double mass_,double x, double y, double z, double vx, double vy, double vz){
    pos_log.resize(1);
    id=id_;
    mass=mass_;
    pos.x=x;
    pos.y=y;
    pos.z=z;
    vel.x=vx;
    vel.y=vy;
    vel.z=vz;
    grav_result.x=grav_result.y=grav_result.z=0;
    bodies_felt=0;
    pos_log[0]=pos;
}

void body::set_name(std::string name_){
    name=name_;
}

vector3D body::grav_accel(body const& attrac){
    auto diff=pos-attrac.pos;
    return diff.unit()*((Gconst*(attrac.mass))/(diff.mag()*diff.mag()))*(-1);
}

void body::sum_grav(body const& attrac){
    bool same_body=(attrac.id!=id);
    if(same_body){
        grav_result = grav_result + grav_accel(attrac);
        bodies_felt++;
    };
}

void body::vel_update(double dt){
    new_vel=vel+(grav_result*dt);
    vel=new_vel;
}

void body::pos_update(double dt,int step){
    pos=(new_vel*dt)+pos;
    pos_log[step]=(pos);
}

void body::init_vel(double dt){
    vel=vel+(grav_result*0.5*dt);
}

void body::final_vel(double dt){
    vel=vel-(grav_result*0.5*dt);
}


