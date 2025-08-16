#ifndef SPACECRAFT_H
#define SPACECRAFT_H

#include "../Headers/vector3D.h"
#include "../Headers/body.h"
#include "../Headers/kep_cart.h"
#include "../Headers/event_detect.h"
#include <string>
#include <vector>
#include <map>



class manouver{
    public:
        vector3D dv;
        std::string label;
        int trigger_type;
        trigger_params trig_conditions;
        bool is_coast;
        double executed_time;
        bool executed=false;
        manouver(vector3D dv_,std::string label_,int trigger_type_, trigger_params trig_conditions_);
        manouver(std::string label_,int trigger_type_, trigger_params trig_conditions_);
};


class spacecraft : public body{
    public:
        using body::body;
        double current_max_grav=0;
        int dominant_body_id=0;
        int dominant_body_index=0;
        double last_event_time=0;

        kep_state orbit_params;
        vector3D parent_pos;
        vector3D parent_vel; 
        void sum_grav(body const& attrac) override;
        
        //Manouver execution functions
        int max_mnvr_index=-1;
        int next_mnvr_index=0;
        std::vector<manouver> all_manouvers;
        std::vector<manouver> complete_manouvers;
        vector3D inertial_dv(manouver mnvr, vector3D parent_pos, vector3D parent_vel);
        void perform_manouver(manouver& mnvr,double& current_t);

        //House Keeping functions
        void spacecraft::situation_update(std::vector<body> &system, double& current_t);
};
#endif