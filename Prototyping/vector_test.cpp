#include <iostream>
#include <cmath>
#include <chrono>
using namespace std;
using namespace std::chrono;

const float Gconst = 6.6743e-11;
class vector3D{
    public:
        double x,y,z;

        vector3D(){
            x=0;
            y=0;
            z=0;
        }

        vector3D(double i,double j,double k){
            x=i;
            y=j;
            z=k;
        }

        void print(){
            cout<<"( "<<x<<" "<<y<<" "<<z<<" )\n";
        }

        double mag(){
            return (sqrt((x*x)+(y*y)+(z*z)));
        }

        vector3D unit(){
            vector3D unit_vec(x/mag(),y/mag(),z/mag());
            return unit_vec;
        }

        vector3D operator+(vector3D const& obj){
            vector3D result(
                x+obj.x,
                y+obj.y,
                z+obj.z);
            return result;
        }

        vector3D operator-(vector3D const& obj){
            vector3D result(
                x-obj.x,
                y-obj.y,
                z-obj.z);
            return result;
        }

        vector3D operator*(double scalar){
            vector3D result(
                x*scalar,
                y*scalar,
                z*scalar);
            return result;
        }

        vector3D operator/(double scalar){
            vector3D result(
                x/scalar,
                y/scalar,
                z/scalar);
            return result;
        }

    
};

class body{
    public:
        double mass;
        vector3D pos;
        body(double x, double y, double z, double mass_){
            mass=mass_;
            pos.x=x;
            pos.y=y;
            pos.z=z;
        }


        vector3D grav_accel(body const& attrac){
            auto diff=pos-attrac.pos;
            return diff.unit()*((Gconst*(attrac.mass))/(diff.mag()*diff.mag()));
        }
};

int main(){
    //vector3D pos(14,19,-22);
    //vector3D pos2(12,-9,2);

    //(pos+pos2).print();
    //(pos/4).print();



    body earth(0,0,0,5.972e24);
    body moon(3.84e8,0,0,7.348e22);
    body sat(3.26e8,0,0,1000);

    auto start = high_resolution_clock::now();

    vector3D total_grav=sat.grav_accel(earth)+sat.grav_accel(moon);
    vector3D egrav=earth.grav_accel(sat)+earth.grav_accel(moon);
    vector3D mgrav=moon.grav_accel(sat)+moon.grav_accel(earth);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout<<"Gravitational acceleration Vector: ";
    total_grav.print();
    cout << "Execution Time: "<< duration.count() << " Microseconds" <<endl;

    return 0;
}