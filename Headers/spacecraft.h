#ifndef SPACECRAFT_H
#define SPACECRAFT_H

#include "../Headers/vector3D.h"
#include "../Headers/body.h"
#include <string>
#include <vector>
#include <map>



class manouver{
    public:
        vector3D dv;
        std::string label;
        double time;
        double executed_time;
        bool executed=false;
        manouver(vector3D dv_,std::string label_,double time_);
};
class spacecraft : public body{
    public:
        using body::body;
        double current_max_grav=0;
        int dominant_body_id=0;
        vector3D parent_pos;
        vector3D parent_vel;
        void sum_grav(body const& attrac) override;
        std::vector<manouver> all_manouvers;
        std::vector<manouver> complete_manouvers;
        vector3D inertial_dv(manouver mnvr, vector3D parent_pos, vector3D parent_vel);
        void perform_manouver(manouver& mnvr,double& current_t);
        void situation_update(std::vector<body> &system);
};
#endif