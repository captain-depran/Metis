#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include "../Headers/vector3D.h"
#include <fstream>
#include <thread>
using namespace std;
using namespace std::chrono;

const double Gconst = 6.6743e-11;
double block_size;

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
            pos_log.resize(block_size);
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
            pos_log[0]=pos;
            cout<<"Body "<<id<<" Intialised\n";
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

        void pos_update(double& dt,int& step){
            pos=(new_vel*dt)+pos;
            pos_log[step]=(pos);
        }

};

void file_wipe(body obj){
    char file_name[6];
    sprintf(file_name,"%d",obj.id);
    string str(file_name);
    str.append(".txt");
    ofstream log_file(str,ofstream::out | ofstream::trunc);
    log_file.close();
}

void chunk_dump(body obj,int block_size){
    char file_name[6];
    sprintf(file_name,"%d",obj.id);
    string str(file_name);
    str.append(".txt");
    ofstream log_file(str,ofstream::out | ofstream::app);
    for (int i=0;i<block_size;i++){
        log_file<<fixed<<obj.pos_log[i].x<<","<<fixed<<obj.pos_log[i].y<<","<<fixed<<obj.pos_log[i].z<<endl;
    }
    log_file.close();
};

int main(){
    //vector3D pos(14,19,-22);
    //vector3D pos2(12,-9,2);

    //(pos+pos2).print();
    //(pos/4).print();


    double timespace=1.21e6;
    //double timespace=864000;
    double stepsize=5;
    int step_count = timespace/stepsize;

    block_size=2000;
    int num_blocks=step_count/block_size;

    body earth(01,5.972e24,0,0,0,0,0,0);
    body moon(02,7.348e22,3.84e8,0,0,0,1082.0,0);
    body sat(03,1000,6671e3,0,0,0,10849,0);
        
    body system[3]={earth,moon,sat};

    

    int sums_done=0;
    //intial block
    auto total_start= high_resolution_clock::now();
    file_wipe(system[0]);
    file_wipe(system[1]);
    file_wipe(system[2]);
    for (int step = 0; step < (block_size); step++){
            for (body obj:system){
                obj.grav_result.zero();
                for (body other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body obj:system){
                obj.vel_update(stepsize);
                obj.pos_update(stepsize,step);
                system[obj.id-1]=obj;
            }
        }
    cout<<"Intial Block done";
    
    for (int block = 1; block<num_blocks;block++){
        auto start = high_resolution_clock::now();
        thread earth_writer(chunk_dump,system[0],block_size);
        thread moon_writer(chunk_dump,system[1],block_size);
        thread sat_writer(chunk_dump,system[2],block_size);
        
        for (int step = 0; step < (block_size); step++){
            for (body obj:system){
                obj.grav_result.zero();
                for (body other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body obj:system){
                
                obj.vel_update(stepsize);
                obj.pos_update(stepsize,step);
                system[obj.id-1]=obj;
            }
        }
        earth_writer.join();
        moon_writer.join();
        sat_writer.join();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: "<< ((100*block)/num_blocks) << "% | Block Time: "<< duration.count() << " milliseconds        ";

    }
    thread earth_writer(chunk_dump,system[0],block_size);
    thread moon_writer(chunk_dump,system[1],block_size);
    thread sat_writer(chunk_dump,system[2],block_size);
    earth_writer.join();
    moon_writer.join();
    sat_writer.join();



    /*
    vector3D total_grav=sat.grav_accel(earth)+sat.grav_accel(moon);
    vector3D egrav=earth.grav_accel(sat)+earth.grav_accel(moon);
    vector3D mgrav=moon.grav_accel(sat)+moon.grav_accel(earth);
    */
    

    cout<<"\n"<<"Final Position: ";
    system[2].pos.print();
    cout <<"Steps: "<<(timespace/stepsize)<<endl;
    cout <<"Sums done on satellite: "<<system[2].bodies_felt<<endl;
    auto total_stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(total_stop - total_start);
    cout << "Execution Time: "<< duration.count() << " milliseconds" <<endl;
    return 0;
}