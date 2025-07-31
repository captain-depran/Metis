#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include "../Headers/vector3D.h"
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
            grav_result.x,grav_result.y,grav_result.z=0;
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

};

void file_wipe(body obj){
    char file_name[6];
    sprintf(file_name,"%d",obj.id);
    string str(file_name);
    str.append(".bin");
    ofstream log_file(str,ofstream::out | ofstream::trunc);
    log_file.close();
}

string file_string(int id){
    char file_name[6];
    sprintf(file_name,"%d",id);
    string str(file_name);
    str.append(".bin");
    return str;
};

void chunk_dump(body &obj,int block_size){
    string str=file_string(obj.id);
    ofstream log_file(str, ios::out | ios::binary |ios::app);
    log_file.write(reinterpret_cast<const char*>(obj.pos_log.data()), block_size * sizeof(vector3D));
    /*for (int i=0;i<block_size;i++){
        log_file<<fixed<<obj.pos_log[i].x<<","<<fixed<<obj.pos_log[i].y<<","<<fixed<<obj.pos_log[i].z<<endl;
    }*/
    log_file.close();
};

vector3D rot_frame(vector3D& rotator_pos, vector3D& obj_pos){
    vector3D translated_pos;
    double theta = atan2(rotator_pos.y,rotator_pos.x);
    translated_pos.z=obj_pos.z;
    translated_pos.x=(obj_pos.x*cos(-1*theta))-(obj_pos.y*sin(-1*theta));
    translated_pos.y=(obj_pos.x*sin(-1*theta))+(obj_pos.y*cos(-1*theta));
    return translated_pos;
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
        thread earth_writer(chunk_dump,system[0],block_size);
        thread moon_writer(chunk_dump,system[1],block_size);
        thread sat_writer(chunk_dump,system[2],block_size);
        
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

void frame_center(int reference_id, int target_id, int out_id){
    string ref_str=file_string(reference_id);
    string tgt_str=file_string(target_id);
    string out_str=file_string(out_id);
    ifstream ref_file;
    ifstream tgt_file;
    ofstream out_file;

    ref_file.open(ref_str);
    tgt_file.open(tgt_str);
    out_file.open(out_str);
    if (ref_file.fail()||tgt_file.fail()||out_file.fail()){
        cerr <<"An object file failed to open"<<endl;
        ref_file.close();
        tgt_file.close();
        out_file.close();
        exit(1);
    }

    string ref_line,tgt_line;
    while (getline(tgt_file,tgt_line)&&getline(ref_file,ref_line)){
        stringstream tgt_stream(tgt_line),ref_stream(ref_line);
        string token;
        double tgt[3],ref[3];
        int i=0;

        while (getline(tgt_stream, token, ',') && i < 3) 
            tgt[i++] = std::stod(token);

        i=0; 

        while (getline(ref_stream, token, ',') && i < 3) 
            ref[i++] = std::stod(token);

        vector3D ref_pos(ref[0],ref[1],ref[2]);
        vector3D tgt_pos(tgt[0],tgt[1],tgt[2]);      
        vector3D new_pos=tgt_pos-ref_pos;

        out_file <<fixed<<new_pos.x<<","<<fixed<<new_pos.y<<","<<fixed<<new_pos.z<<"\n";

    }
    ref_file.close();
    tgt_file.close();
    out_file.close();
};

void frame_swap(int reference_id, int target_id, int out_id){
    string ref_str=file_string(reference_id);
    string tgt_str=file_string(target_id);
    string out_str=file_string(out_id);
    ifstream ref_file;
    ifstream tgt_file;
    ofstream out_file;

    ref_file.open(ref_str);
    tgt_file.open(tgt_str);
    out_file.open(out_str);
    if (ref_file.fail()||tgt_file.fail()||out_file.fail()){
        cerr <<"An object file failed to open"<<endl;
        ref_file.close();
        tgt_file.close();
        out_file.close();
        exit(1);
    }

    string ref_line,tgt_line;
    while (getline(tgt_file,tgt_line)&&getline(ref_file,ref_line)){
        stringstream tgt_stream(tgt_line),ref_stream(ref_line);
        string token;
        double tgt[3],ref[3];
        int i=0;

        while (getline(tgt_stream, token, ',') && i < 3) 
            tgt[i++] = std::stod(token);

        i=0; 

        while (getline(ref_stream, token, ',') && i < 3) 
            ref[i++] = std::stod(token);

        vector3D ref_pos(ref[0],ref[1],ref[2]);
        vector3D tgt_pos(tgt[0],tgt[1],tgt[2]);      
        vector3D new_pos=rot_frame(ref_pos,tgt_pos);

        out_file <<fixed<<new_pos.x<<","<<fixed<<new_pos.y<<","<<fixed<<new_pos.z<<"\n";

    }
    ref_file.close();
    tgt_file.close();
    out_file.close();
};

int main(){
    double timespace=1.419e6;
    double stepsize=10;
    int step_count = timespace/stepsize;

    block_size=50000;

   
    body sat(03,1000,3.195e8,0,10e6,0,1027.5,0);
    auto total_start= high_resolution_clock::now();

    //Top is no logging, bottom is with logging
    //run_sim(timespace,stepsize,block_size,sat);
    run_sim(timespace,stepsize,block_size,sat,true);

    auto total_stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(total_stop - total_start);
    cout << "Execution Time: "<< duration.count() << " milliseconds" <<endl;
    //cout << "Frame translation in progress..." <<endl;


    //thread a(frame_center,1,1,4);
    //thread b(frame_center,1,2,5);
    //thread c(frame_center,1,3,6);

    //.join();
    //b.join();
    //c.join();
    
    //cout<< "Frame rotation..."<<endl;
    //thread d(frame_swap,5,5,7);
    //thread e(frame_swap,5,6,8);
    //d.join();
    //e.join();

    return 0;
}