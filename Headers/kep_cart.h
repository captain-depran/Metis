#ifndef KEP_CART_H
#define KEP_CART_H

#include "../Headers/vector3D.h"
#include "../Headers/body.h"

struct state_vector {
    vector3D r;
    vector3D v;
};

struct kep_state {
    double semi_maj;
    double ecc;
    double inc;
    double long_asc_node;
    double arg_peri;
    double true_anom;
};

state_vector cart_state(double host_mass, double a, double e, double i, double Omega, double omega, double nu);
kep_state cart_to_kep(body const& parent, vector3D& r, vector3D& v);
#endif