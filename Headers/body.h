#ifndef BODY_H
#define BODY_H

#include "../Headers/vector3D.h"
#include "../Headers/constants.h"
#include <vector>
#include <string>

struct body_import{
    std::string name;
    std::string parent;
    int id;
    double radius;
    double mass;
    double semi_maj;
    double ecc;
    double inc;
    double long_asc_node;
    double arg_peri;
    double true_anom;
    vector3D r;
    vector3D v;
};

class body{
    public:
        double mass;
        std::string name;
        int parent_id=1;
        int id;
        vector3D pos;
        vector3D grav_result;
        std::vector<vector3D> pos_log;
        int bodies_felt;
        vector3D vel;
        vector3D new_vel;
        body(int id_,double mass_,vector3D r, vector3D v);
        body(int id_,double mass_,double x, double y, double z, double vx, double vy, double vz);
        void set_name(std::string name_);
        vector3D grav_accel(body const& attrac);
        virtual void sum_grav(body const& attrac);
        void vel_update(double dt);
        void pos_update(double dt,int step);
        void init_vel(double dt);
        void final_vel(double dt);
};



#endif
