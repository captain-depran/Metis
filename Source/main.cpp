#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include "../Headers/vector3D.h"
#include "../Headers/file_tools.h"
#include "../Headers/frame_tools.h"
#include <fstream>
#include <string>
#include <thread>
#include <sstream> 


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
            grav_result.x=grav_result.y=grav_result.z=0;
            bodies_felt=0;
            pos_log[0]=pos;
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

        void init_vel(double& dt){
            vel=vel+(grav_result*0.5*dt);
        }
        void final_vel(double& dt){
            vel=vel-(grav_result*0.5*dt);
        }

};




void file_wipe(body obj){
    string str=file_string(obj.id);
    ofstream log_file(str,ofstream::out | ofstream::trunc);
    log_file.close();
}


void chunk_dump(body &obj,int dump_size){
    string str=file_string(obj.id);
    ofstream log_file(str, ios::out | ios::binary |ios::app);
    log_file.write(reinterpret_cast<const char*>(obj.pos_log.data()), dump_size * sizeof(vector3D));
    log_file.close();
};

void run_sim(double timespace, double stepsize, int block_size, body sat, bool log){
    int step_count = timespace/stepsize;

    int num_blocks=step_count/block_size;
    int remainder = step_count%block_size;

    body earth(01,5.972e24,0,0,0,0,0,0);
    body moon(02,7.348e22,3.84e8,0,0,0,1018.0,0);
    body system[3]={earth,moon,sat};
    int sums_done=0;
    //intial block
    file_wipe(system[0]);
    file_wipe(system[1]);
    file_wipe(system[2]);

    // Leapfrog velocity offset init

    for (body &obj:system){
        obj.grav_result.zero();
        for (const body &other_body:system){
                obj.sum_grav(other_body);
        }
        obj.init_vel(stepsize);
        system[obj.id-1]=obj;   
    }

    for (int step = 0; step < (block_size); step++){
            for (body &obj:system){
                obj.grav_result.zero();
                obj.pos_update(stepsize,step);
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body &obj:system){
                obj.vel_update(stepsize);
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
            for (body &obj:system){
                obj.grav_result.zero();
                obj.pos_update(stepsize,step);
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body &obj:system){
                obj.vel_update(stepsize);
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

    if (remainder > 0){
        auto start = high_resolution_clock::now();
        for (int step = 0; step < (remainder); step++){
            for (body &obj:system){
                obj.grav_result.zero();
                obj.pos_update(stepsize,step);
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body &obj:system){
                obj.vel_update(stepsize);

                system[obj.id-1]=obj;
            }
        }
        thread earth_writer(chunk_dump,system[0],remainder);
        thread moon_writer(chunk_dump,system[1],remainder);
        thread sat_writer(chunk_dump,system[2],remainder);
        earth_writer.join();
        moon_writer.join();
        sat_writer.join();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: FINISHING STRAGGLERS | Block Time: "<< duration.count() << " milliseconds        ";
    }

    for (body &obj:system){
        obj.final_vel(stepsize);
        system[obj.id-1]=obj;   
    }

    cout<<"\n"<<"Final Position: ";
    system[2].pos.print();
    cout <<"Steps: "<<(timespace/stepsize)<<endl;
    cout <<"Sums done on satellite: "<<system[2].bodies_felt<<endl;
};

void run_sim(double timespace, double stepsize, int block_size, body sat){
    int step_count = timespace/stepsize;
    int num_blocks=step_count/block_size;
    int remainder = step_count%block_size;
    body earth(01,5.972e24,0,0,0,0,0,0);
    body moon(02,7.348e22,3.84e8,0,0,0,1018.0,0);
    body system[3]={earth,moon,sat};
    int sums_done=0;
    //intial block
    for (int step = 0; step < (block_size); step++){
            for (body &obj:system){
                obj.grav_result.zero();
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body &obj:system){
                obj.vel_update(stepsize);
                obj.pos_update(stepsize,step);
                system[obj.id-1]=obj;
            }
        }
    cout<<"Intial Block done";

    for (int block = 1; block<num_blocks;block++){
        auto start = high_resolution_clock::now();
        for (int step = 0; step < (block_size); step++){
            for (body &obj:system){
                obj.grav_result.zero();
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body &obj:system){
                
                obj.vel_update(stepsize);
                obj.pos_update(stepsize,step);
                system[obj.id-1]=obj;
            }
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: "<< ((100*block)/num_blocks) << "% | Block Time: "<< duration.count() << " milliseconds        ";
    }

    if (remainder > 0){
        auto start = high_resolution_clock::now();
        for (int step = 0; step < (remainder); step++){
            for (body &obj:system){
                obj.grav_result.zero();
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
                system[obj.id-1]=obj;   
            }
            for (body &obj:system){
                
                obj.vel_update(stepsize);
                obj.pos_update(stepsize,step);
                system[obj.id-1]=obj;
            }
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: FINISHING STRAGGLERS  | Block Time: "<< duration.count() << " milliseconds        ";
    }

    cout<<"\n"<<"Final Position: ";
    system[2].pos.print();
    cout <<"Steps: "<<(timespace/stepsize)<<endl;
    cout <<"Sums done on satellite: "<<system[2].bodies_felt<<endl;
};

int main(){
    double timespace=5.419e6;
    double stepsize=10;
    int step_count = timespace/stepsize;

    block_size=50000;

   
    body sat(03,1000,3.195e8,0,10e6,0,1027.34,0);
    auto total_start= high_resolution_clock::now();

    //Top is no logging, bottom is with logging
    //run_sim(timespace,stepsize,block_size,sat);
    run_sim(timespace,stepsize,block_size,sat,true);

    auto total_stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(total_stop - total_start);
    cout << "Execution Time: "<< duration.count() << " milliseconds" <<endl;
    cout << "Frame translation in progress..." <<endl;


    frame_center(1,1,4,step_count);
    frame_center(1,2,5,step_count);
    frame_center(1,3,6,step_count);


    cout<< "Frame rotation..."<<endl;
    frame_swap(5,5,7,step_count);
    frame_swap(5,6,8,step_count);


    return 0;
}