#ifndef VECTOR3D_H
#define VECTOR3D_H

class vector3D{
    public:
        double x,y,z;
        vector3D();
        vector3D(double i,double j,double k);
        void print();
        double mag();
        void zero();
        vector3D unit();
        vector3D cross(vector3D const& obj);
        double vector3D::dot(vector3D const& obj);
        vector3D operator+(vector3D const& obj);
        vector3D operator-(vector3D const& obj);
        vector3D operator*(double scalar);
        vector3D operator/(double scalar);
};

#endif