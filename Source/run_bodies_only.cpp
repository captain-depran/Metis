#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include "../Headers/vector3D.h"
#include "../Headers/file_tools.h"
#include "../Headers/frame_tools.h"
#include "../Headers/kep_cart.h"
#include "../Headers/body.h"
#include "../Headers/run_bodies_only.h"
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <iomanip> 


using namespace std;
using namespace std::chrono;


//This is a version of a simulation run that does not consider a spacecraft, only the gravitationally significantly bodies
//It was intially a modified version of V0.4.0-dev 2 main.cpp

namespace{
void chunk_dump(body &obj,int dump_size){
    string str=file_string(obj.id);
    ofstream log_file(str, ios::out | ios::binary |ios::app);
    log_file.write(reinterpret_cast<const char*>(obj.pos_log.data()), dump_size * sizeof(vector3D));
    log_file.close();
};

void run_loop(vector<body> &system, int loop_size, double stepsize, double& current_t){
    if (system[0].pos_log.size()!=loop_size){
        for(body &obj:system){
            obj.pos_log.resize(loop_size);
        };
    };
    
    for (int step = 0; step < (loop_size); step++){
            current_t+=stepsize;
            for (body &obj:system){
                obj.grav_result.zero();
            }

            for(body& obj:system){
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
            }

            for (body &obj:system){
                obj.vel_update(stepsize);
            }
            for (body &obj:system){
                obj.pos_update(stepsize,step);
            }
            
        }
};

void block_save(vector<body> system,int block_size){
    for (body &obj:system){
        chunk_dump(obj,block_size);
    }
};
}

void pre_run_bodies(string body_file,string sat_file,double timespace, double stepsize, int block_size, bool log_flag,int log_freq,vector<int> &body_ids){
    thread block_writer;
    vector <body> system;
    std::map<std::string,int> sys_index_map;

    double current_t=0;

    int step_count = timespace/stepsize;

    int num_blocks=step_count/block_size;
    int remainder = step_count%block_size;
    

    load_body_file("INPUT/bodies (sun_earth_moon).cfg",system,sys_index_map);
    int sums_done=0;

    auto total_start= high_resolution_clock::now();

    //intial block
    for (body &obj:system){
        file_wipe(obj.id);
        body_ids.push_back(obj.id);
    }
    // Leapfrog velocity offset init
    for (body &obj:system){
        obj.grav_result.zero();
        for (const body &other_body:system){
                obj.sum_grav(other_body);
        }
        obj.init_vel(stepsize); 
    }

    // Block 1 run
    run_loop(system,block_size,stepsize,current_t);

    // Rest of Blocks
    for (int block = 1; block<num_blocks;block++){
        auto start = high_resolution_clock::now();

        if (log_flag){
            block_writer=thread(block_save,system,block_size);
        };

        run_loop(system,block_size,stepsize,current_t);
        
        if (block_writer.joinable()){block_writer.join();};


        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: "<< ((100*(block+1))/num_blocks) << "% | Block Time: "<< duration.count() << " milliseconds        ";

    }

    if(log_flag){
        thread block_writer(block_save,system,block_size);
        block_writer.join();
    };

    // A partial block run to sweep up any remainders (if runtime not perfectly divisible)
    if (remainder > 0){
        auto start = high_resolution_clock::now();

        run_loop(system,remainder,stepsize,current_t);

        if(log_flag){
            thread block_writer(block_save,system,remainder);
            block_writer.join();  
        };  

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: FINISHING STRAGGLERS | Block Time: "<< duration.count() << " milliseconds        ";
    }

    for (body &obj:system){
        obj.final_vel(stepsize);   
    }

    auto total_stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(total_stop - total_start);
    cout << "\nExecution Time: "<< duration.count() << " milliseconds" <<endl;
    cout << "---------------------------------"<<endl;
};