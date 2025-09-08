#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include "../Headers/vector3D.h"
#include "../Headers/file_tools.h"
#include "../Headers/frame_tools.h"
#include "../Headers/kep_cart.h"
#include "../Headers/body.h"
#include "../Headers/spacecraft.h"
#include "../Headers/run_craft_and_prerun_body.h"
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <iomanip> 


using namespace std;
using namespace std::chrono;
namespace{
// Function to format time in days, hours, minutes, and seconds, courtesy of the internet
string formatTime(double totalSeconds) {
    // Separate integer and fractional parts
    long long intSeconds = static_cast<long long>(std::floor(totalSeconds));
    double fractional = totalSeconds - intSeconds;

    int days    = intSeconds / 86400;
    int hours   = (intSeconds % 86400) / 3600;
    int minutes = (intSeconds % 3600) / 60;
    int seconds = intSeconds % 60;

    std::ostringstream oss;
    oss << days << "d "
        << hours << "h "
        << minutes << "m ";

    // Include fractional seconds with up to 3 decimal places (milliseconds)
    oss << std::fixed << std::setprecision(3) << (seconds + fractional) << "s";

    return oss.str();
}

void chunk_dump(body &obj,int dump_size){
    string str=file_string(obj.id);
    ofstream log_file(str, ios::out | ios::binary |ios::app);
    log_file.write(reinterpret_cast<const char*>(obj.pos_log.data()), dump_size * sizeof(vector3D));
    log_file.close();
};

void run_loop(spacecraft& craft,vector<body> &system, int loop_size, double stepsize, double& current_t, int& run_step){
    if (system[0].pos_log.size()!=loop_size){
        craft.pos_log.resize(loop_size);
    };
    
    for (int step = 0; step < (loop_size); step++){
            craft.situation_update(system,current_t);
            current_t+=stepsize;
            craft.grav_result.zero();

            for(body& obj:system){
                craft.sum_grav(obj);
                obj.vel_from_pos_log(stepsize,run_step);
            }
            //craft.grav_result.print();
            craft.vel_update(stepsize);
            for (body &obj:system){
                obj.pos_from_log(run_step);
            }
            craft.pos_update(stepsize,step);
            run_step++;
            
        }
};

void block_save(spacecraft craft,vector<body> system,int block_size){
    chunk_dump(craft,block_size);
};
}

void run_sat(string body_file,string sat_file,double timespace, double stepsize, int block_size, bool log_flag,int log_freq,vector<int> &body_ids){
    thread block_writer;
    vector <body> system;
    std::map<std::string,int> sys_index_map;

    double current_t=0;
    int run_step=0;

    int step_count = timespace/stepsize;

    int num_blocks=step_count/block_size;
    int remainder = step_count%block_size;
    

    load_body_file("INPUT/bodies (sun_earth_moon).cfg",system,sys_index_map);
    spacecraft craft=load_craft_file("INPUT/test_sat.cfg",system);
    craft.assign_sys_map(sys_index_map);
    int sums_done=0;

    auto total_start= high_resolution_clock::now();

    //intial block
    for (body &obj:system){
        obj.pos_log=import_positions(file_string(obj.id));
        body_ids.push_back(obj.id);
    }
    file_wipe(craft.id);
    body_ids.push_back(craft.id);
    craft.grav_result.zero();
    // Leapfrog velocity offset init
    for (body &obj:system){
        obj.grav_result.zero();
        craft.sum_grav(obj);
        for (const body &other_body:system){
                obj.sum_grav(other_body);
        }
        obj.init_vel(stepsize); 
    }
    craft.init_vel(stepsize);

    // Block 1 run
    craft.situation_update(system,current_t);
    run_loop(craft,system,block_size,stepsize,current_t,run_step);

    // Rest of Blocks
    for (int block = 1; block<num_blocks;block++){
        auto start = high_resolution_clock::now();

        if (log_flag){
            block_writer=thread(block_save,craft,system,block_size);
        };

        run_loop(craft,system,block_size,stepsize,current_t,run_step);
        
        if (block_writer.joinable()){block_writer.join();};


        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: "<< ((100*(block+1))/num_blocks) << "% | Block Time: "<< duration.count() << " milliseconds        ";

    }

    if(log_flag){
        thread block_writer(block_save,craft,system,block_size);
        block_writer.join();
    };

    // A partial block run to sweep up any remainders (if runtime not perfectly divisible)
    if (remainder > 0){
        auto start = high_resolution_clock::now();

        run_loop(craft,system,remainder,stepsize,current_t,run_step);

        if(log_flag){
            thread block_writer(block_save,craft,system,remainder);
            block_writer.join();  
        };  

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout <<"\r"<< "Progress: FINISHING STRAGGLERS | Block Time: "<< duration.count() << " milliseconds        ";
    }

    for (body &obj:system){
        obj.final_vel(stepsize);   
    }
    craft.final_vel(stepsize);

    auto total_stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(total_stop - total_start);
    cout << "\nExecution Time: "<< duration.count() << " milliseconds" <<endl;
    cout << "---------------------------------"<<endl;
    for (manouver &mnvr:craft.complete_manouvers){
        if (mnvr.executed==true && !mnvr.is_coast){
            cout << "MANOUVER "<<mnvr.label<<" EXECUTED AT "<<formatTime(mnvr.executed_time)<<endl;
        }
        else if (mnvr.executed==true && mnvr.is_coast){
            cout << "COAST EVENT "<<mnvr.label<<" ENDED AT "<<formatTime(mnvr.executed_time)<<endl;
        };
    };
    cout << "---------------------------------"<<endl;
    craft.print_orbit(system);
};