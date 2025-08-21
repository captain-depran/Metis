#include "../Headers/vector3D.h"
#include "../Headers/kep_cart.h"
#include "../Headers/constants.h"
#include "../Headers/body.h"
#include <cmath>
#include <iostream>

state_vector cart_state(double host_mass, double a, double e, double i, double Omega, double omega, double nu){
    //const double Gconst = 6.6743e-11;
    double MU = host_mass*Gconst;
    vector3D v_pqw;

    double p = (a * (1-e*e));
    double r_mag = p/(1+e*cos(nu));
    double h = sqrt(MU * p);

    //Perifocal Frame
    vector3D r_pqw(r_mag * cos(nu), r_mag * sin(nu), 0.0);
    if(h!=0){
        v_pqw.x=-(MU/h)*sin(nu);
        v_pqw.y= (MU/h)*(e+cos(nu));
        v_pqw.z=0.0;
        
    } else{
        vector3D v_pqw(0,0,0);
    };

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

kep_state cart_to_kep(body const& parent, vector3D& r, vector3D& v){
    vector3D rel_r= r - parent.pos;
    vector3D rel_v= v - parent.vel;


    double r_mag = rel_r.mag();
    double v_mag = rel_v.mag();

    double mu = parent.mass * Gconst;

    // Calculate the angular momentum vector
    vector3D h= rel_r.cross(rel_v);
    double h_mag = h.mag(); 
    // Eccentricity vector
    vector3D e_vec = ( (rel_v.cross(h)) / mu ) - (rel_r.unit());
    double e = e_vec.mag();

    //inclination 
    double i = acos(h.z / h_mag); 

    //Node vector and longitude of the ascending node
    vector3D k_hat(0, 0, 1);
    vector3D n = k_hat.cross(h);
    double n_mag = n.mag();

    double long_asc = atan2(n.y, n.x);
    if (long_asc < 0) long_asc += 2*PI;  

    //Argument of periapsis
    double arg_peri = atan2(
        k_hat.dot(n.cross(e_vec)) / (n_mag * e),  // sin component
        n.dot(e_vec) / (n_mag * e)                 // cos component
    );
    if (arg_peri < 0) arg_peri += 2 * PI;

    // True anomaly
    double nu = atan2(
        h.dot(e_vec.cross(rel_r)) / (h_mag * e * r_mag), // sin component
        rel_r.dot(e_vec) / (r_mag * e)                   // cos component
    );
    //std::cout << "nu: " << nu << std::endl;
    if (nu < 0) nu += 2 * PI;

    // Semi-major axis
    double energy = (v_mag * v_mag) / 2.0 - mu / r_mag;
    double a = -mu / (2.0 * energy);

    return {
        a,          // semi-major axis PASSED
        e,          // eccentricity PASSED
        i * 180/PI,          // inclination PASSED
        long_asc * 180/PI,   // longitude of the ascending node PASSED
        arg_peri * 180/PI,   // argument of periapsis 
        nu * 180/PI         // true anomaly  
    };
}