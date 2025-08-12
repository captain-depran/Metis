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
#include <fstream>
#include <string>
#include <thread>
#include <sstream>
#include <iomanip> 


using namespace std;
using namespace std::chrono;

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

void run_loop(spacecraft& craft,vector<body> &system, int loop_size, double stepsize, double& current_t){
    if (system[0].pos_log.size()!=loop_size){
        craft.pos_log.resize(loop_size);
        for(body &obj:system){
            obj.pos_log.resize(loop_size);
        };
    };
    
    for (int step = 0; step < (loop_size); step++){
            current_t+=stepsize;
            for (manouver &mnvr:craft.all_manouvers){
                if (current_t >= mnvr.time && mnvr.executed==false){
                    craft.perform_manouver(mnvr,current_t);
                }
            }
            craft.grav_result.zero();
            for (body &obj:system){
                obj.grav_result.zero();
            }

            for(body& obj:system){
                craft.sum_grav(obj);
                for (const body &other_body:system){
                    obj.sum_grav(other_body);
                }
            }

            for (body &obj:system){
                obj.vel_update(stepsize);
            }
            craft.vel_update(stepsize);
            for (body &obj:system){
                obj.pos_update(stepsize,step);
            }
            craft.pos_update(stepsize,step);
            craft.situation_update(system,false);
            
        }
};

void block_save(spacecraft craft,vector<body> system,int block_size){
    for (body &obj:system){
        chunk_dump(obj,block_size);
    }
    chunk_dump(craft,block_size);
};


void run_sim(string body_file,string sat_file,double timespace, double stepsize, int block_size, bool log_flag,int log_freq,vector<int> &body_ids){
    thread block_writer;
    vector <body> system;
    double current_t=0;

    int step_count = timespace/stepsize;

    int num_blocks=step_count/block_size;
    int remainder = step_count%block_size;
    

    load_body_file("INPUT/bodies (sun_earth_moon).cfg",system);
    spacecraft craft=load_craft_file("INPUT/test_sat.cfg",system);
    int sums_done=0;

    auto total_start= high_resolution_clock::now();

    //intial block
    for (body &obj:system){
        file_wipe(obj.id);
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
    craft.situation_update(system,true);
    run_loop(craft,system,block_size,stepsize,current_t);

    // Rest of Blocks
    for (int block = 1; block<num_blocks;block++){
        auto start = high_resolution_clock::now();

        if (log_flag){
            block_writer=thread(block_save,craft,system,block_size);
        };

        run_loop(craft,system,block_size,stepsize,current_t);
        
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

        run_loop(craft,system,remainder,stepsize,current_t);

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
        if (mnvr.executed==true){
            cout << "MANOUVER "<<mnvr.label<<" EXECUTED AT "<<formatTime(mnvr.executed_time)<<endl;
        }
    }
};


int main(){
    vector<int> body_ids;
    bool log;
    sim_settings settings=load_settings_file("INPUT/SIM_CONFIG.cfg");
    double timespace=settings.timespan;
    double stepsize=settings.step_size;
    int step_count = timespace/stepsize;
    vector <body> system;

    int block_size=settings.buffer_size;

    if (settings.log_freq=0){
        log=false;
        settings.log_freq=1;
    }
    else{
        log=true;
    };

    cout << "---------------------------------"<<endl;
    cout <<"Time Limit: "<<formatTime(timespace)<<endl;
    cout <<"Step Size: "<<stepsize<<endl;
    cout <<"Steps: "<<(timespace/stepsize)<<endl;
    cout <<"Buffer Size: "<<block_size<<endl;
    cout <<"Spacecraft File: "<<settings.sat_file_name<<endl;
    cout <<"Body File: "<<settings.body_file_name<<endl;
    cout << "---------------------------------"<<endl;
    


    run_sim(settings.body_file_name,
        settings.sat_file_name,
        timespace,
        stepsize,
        block_size,
        log,
        settings.log_freq,
        body_ids);

    cout << "---------------------------------"<<endl;
    cout << "Frame Centering in progress..." <<endl;
    for (int &id:body_ids){
        frame_center(settings.core_body_id,id,step_count);
    };
    cout << "---------------------------------"<<endl;


    
    /*
    cout << "---------------------------------"<<endl;
    cout<< "Frame rotation in progress..."<<endl;
    frame_swap(7,7,9,step_count);
    frame_swap(7,8,10,step_count);
    cout << "---------------------------------"<<endl;
    */
    
    cout <<"DONE! Press ENTER to exit."<<endl;
    cin.ignore(); // Wait for user input before exiting
    return 0;
}