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
};
class spacecraft : public body{
    public:
        using body::body;
        std::vector<manouver> all_manouvers;


};
#endif