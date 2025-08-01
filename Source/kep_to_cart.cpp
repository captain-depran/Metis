#include "../Headers/vector3D.h"
#include "../Headers/kep_to_cart.h"
#include <cmath>
#include <iostream>

state_vector cart_state(double host_mass, double a, double e, double i, double Omega, double omega, double nu){
    const double Gconst = 6.6743e-11;
    double MU = host_mass*Gconst;

    double p = (a * (1-e*e));
    double r_mag = p/(1+e*cos(nu));
    double h = sqrt(MU * p);

    //Perifocal Frame
    vector3D r_pqw(r_mag * cos(nu), r_mag * sin(nu), 0.0);
    vector3D v_pqw(-(MU/h)*sin(nu), (MU/h)*(e+cos(nu)), 0.0);

    //Rotation Matrix
    double cO = cos(Omega), sO = sin(Omega);
    double co = cos(omega), so = sin(omega);
    double ci = cos(i), si = sin(i);

    double Q[3][3] = {
        {cO*co - sO*so*ci, -cO*so - sO*co*ci, sO*si},
        {sO*co + cO*so*ci, -sO*so + cO*co*ci, -cO*si},
        {so*si,             co*si,            ci}
    };

    vector3D r(
        Q[0][0]*r_pqw.x + Q[0][1]*r_pqw.y,
        Q[1][0]*r_pqw.x + Q[1][1]*r_pqw.y,
        Q[2][0]*r_pqw.x + Q[2][1]*r_pqw.y);

    vector3D v(
        Q[0][0]*v_pqw.x + Q[0][1]*v_pqw.y,
        Q[1][0]*v_pqw.x + Q[1][1]*v_pqw.y,
        Q[2][0]*v_pqw.x + Q[2][1]*v_pqw.y);

    return {r, v};

};

