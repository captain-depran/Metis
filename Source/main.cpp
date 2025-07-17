#include <iostream>
#include <cmath>
#include <chrono>
#include "../Headers/vector3D.h"
using namespace std;
using namespace std::chrono;

const float Gconst = 6.6743e-11;
class body{
    public:
        double mass;
        int id;
        vector3D pos;
        vector3D grav_result;
        int bodies_felt;
        
        body(int id_,double x, double y, double z, double mass_){
            id=id_;
            mass=mass_;
            pos.x=x;
            pos.y=y;
            pos.z=z;
            grav_result.x,grav_result.y,grav_result.z=0;
            bodies_felt=0;
            
        }


        vector3D grav_accel(body const& attrac){
            auto diff=pos-attrac.pos;
            return diff.unit()*((Gconst*(attrac.mass))/(diff.mag()*diff.mag()));
        }

        void sum_grav(body const& attrac){
            bool same_body=(attrac.id==id);
            switch(same_body){
                case false:
                    grav_result = grav_result + grav_accel(attrac);
                    bodies_felt++;
                    break;
                case true:
                    break;
            }
        }
};


int main(){
    //vector3D pos(14,19,-22);
    //vector3D pos2(12,-9,2);

    //(pos+pos2).print();
    //(pos/4).print();

    double timespace=3.154e+7;
    int stepsize=1;

    body earth(1,0,0,0,5.972e24);
    body moon(2,3.84e8,0,0,7.348e22);
    body sat(3,3.26e8,0,0,1000);

    body system[3]={earth,moon,sat};

    int sums_done=0;
    

    auto start = high_resolution_clock::now();
    for (double step = 0; step < (timespace/stepsize); step++){
        for (body obj:system){
            obj.grav_result.zero();
            for (body other_body:system){
                obj.sum_grav(other_body);
            }
            system[obj.id-1]=obj;
        }

    }




    /*
    vector3D total_grav=sat.grav_accel(earth)+sat.grav_accel(moon);
    vector3D egrav=earth.grav_accel(sat)+earth.grav_accel(moon);
    vector3D mgrav=moon.grav_accel(sat)+moon.grav_accel(earth);
    */
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout<<"Final Gravitational acceleration Vector: ";
    system[2].grav_result.print();
    cout <<"Steps: "<<(timespace/stepsize)<<endl;
    cout <<"Sums done on satellite: "<<system[2].bodies_felt<<endl;
    cout << "Execution Time: "<< duration.count() << " milliseconds" <<endl;
    cout << "DISCLAIMER: NO POSITIONAL UPDATES ARE CALCULATED, THIS SIMPLY DOES RESULTANT GRAV CALCULATIONS "<<(timespace/stepsize)<<" AMOUNT OF TIMES!"<<endl;
    return 0;
}