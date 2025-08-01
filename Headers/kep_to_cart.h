#ifndef KEP_TO_CART_H
#define KEP_TO_CART_H

#include "../Headers/vector3D.h"

struct state_vector {
    vector3D r;
    vector3D v;
};

state_vector cart_state(double host_mass, double a, double e, double i, double Omega, double omega, double nu);

#endif