#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include "../Headers/vector3D.h"
using namespace std;
using namespace std::chrono;

const double Gconst = 6.6743e-11;
double step_count;

class body{
    public:
        double mass;
        int id;
        vector3D pos;
        vector3D grav_result;
        vector<vector3D> pos_log;
        int bodies_felt;
        vector3D vel;
        vector3D new_vel;

        
        body(int id_,double mass_,double x, double y, double z, double vx, double vy, double vz){
            //pos_log.reserve(step_count);
            id=id_;
            mass=mass_;
            pos.x=x;
            pos.y=y;
            pos.z=z;
            vel.x=vx;
            vel.y=vy;
            vel.z=vz;
            grav_result.x,grav_result.y,grav_result.z=0;
            bodies_felt=0;
            pos_log.push_back(pos);
        }


        vector3D grav_accel(body const& attrac){
            auto diff=pos-attrac.pos;
            return diff.unit()*((Gconst*(attrac.mass))/(diff.mag()*diff.mag()))*(-1);
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

        void vel_update(double& dt){
            new_vel=vel+(grav_result*dt);
            vel=new_vel;
        }

        void pos_update(double& dt){
            pos=(new_vel*dt)+pos;
            pos_log.push_back(pos);
        }

};



int main(){
    //vector3D pos(14,19,-22);
    //vector3D pos2(12,-9,2);

    //(pos+pos2).print();
    //(pos/4).print();


    //double timespace=3.154e7;
    double timespace=250000;
    double stepsize=10;
    step_count = timespace/stepsize;

    body earth(1,5.972e24,0,0,0,0,0,0);
    body moon(2,7.348e22,3.84e8,0,0,0,1082.0,0);
    body sat(3,1000,3.0e8,0,0,0,800,0);
        
    body system[3]={earth,moon,sat};

    

    int sums_done=0;
    

    auto start = high_resolution_clock::now();
    for (double step = 0; step < (step_count); step++){
        for (body obj:system){
            obj.grav_result.zero();
            for (body other_body:system){
                obj.sum_grav(other_body);
            }
            system[obj.id-1]=obj;   
        }
        for (body obj:system){
            obj.vel_update(stepsize);
            obj.pos_update(stepsize);
            system[obj.id-1]=obj;
        }


        //cout<<system[2].vel.mag()<<endl;
        //cout<<(system[2].pos-system[0].pos).mag()<<endl;

    }




    /*
    vector3D total_grav=sat.grav_accel(earth)+sat.grav_accel(moon);
    vector3D egrav=earth.grav_accel(sat)+earth.grav_accel(moon);
    vector3D mgrav=moon.grav_accel(sat)+moon.grav_accel(earth);
    */
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout<<"Final Position: ";
    system[2].pos.print();
    cout <<"Steps: "<<(timespace/stepsize)<<endl;
    cout <<"Sums done on satellite: "<<system[2].bodies_felt<<endl;
    cout << "Execution Time: "<< duration.count() << " milliseconds" <<endl;
    return 0;
}