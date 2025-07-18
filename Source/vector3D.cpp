#include "../Headers/vector3D.h"
#include <iostream>
using namespace std;

vector3D::vector3D(){
    x=0;
    y=0;
    z=0;
}

vector3D::vector3D(double i,double j,double k){
    x=i;
    y=j;
    z=k;
}

void vector3D::print(){
    cout<<"( "<<x<<" "<<y<<" "<<z<<" )\n";
}

double vector3D::mag(){
    return (sqrt((x*x)+(y*y)+(z*z)));
}

void vector3D::zero(){
    x=0;
    y=0;
    z=0;
}

vector3D vector3D::unit(){
    vector3D unit_vec(x/mag(),y/mag(),z/mag());
    return unit_vec;
}

vector3D vector3D::cross(vector3D const& obj){
    vector3D result;
    result.x=(y*obj.z)-(z*obj.y);
    result.y=(z*obj.x)-(x*obj.z);
    result.z=(x*obj.y)-(y*obj.x);
    return result;
}

vector3D vector3D::operator+(vector3D const& obj){
    vector3D result(
        x+obj.x,
        y+obj.y,
        z+obj.z);
    return result;
}

vector3D vector3D::operator-(vector3D const& obj){
    vector3D result(
        x-obj.x,
        y-obj.y,
        z-obj.z);
    return result;
}

vector3D vector3D::operator*(double scalar){
    vector3D result(
        x*scalar,
        y*scalar,
        z*scalar);
    return result;
}

vector3D vector3D::operator/(double scalar){
    vector3D result(
        x/scalar,
        y/scalar,
        z/scalar);
    return result;
}

    
