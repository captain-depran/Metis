#ifndef SPACECRAFT_H
#define SPACECRAFT_H

#include "../Headers/vector3D.h"
#include "../Headers/body.h"
#include <string>
#include <vector>


class manouver{
    public:
        vector3D dv;
        std::string label;
        double time;
        bool executed=false;
        manouver(vector3D dv_,std::string label_,double time_);
};
class spacecraft : public body{
    public:
        using body::body;
        std::vector<manouver> all_manouvers;
        vector3D spacecraft::inertial_dv(manouver mnvr, body const& parent);
        void spacecraft::perform_manouver(manouver mnvr, body const& parent);


};
#endif