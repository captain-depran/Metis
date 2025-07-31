#include "../Headers/vector3D.h"
#include "../Headers/file_tools.h"
#include "../Headers/frame_tools.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

vector3D rot_frame(vector3D& rotator_pos, vector3D& obj_pos){
    vector3D translated_pos;
    double theta = atan2(rotator_pos.y,rotator_pos.x);
    translated_pos.z=obj_pos.z;
    translated_pos.x=(obj_pos.x*cos(-1*theta))-(obj_pos.y*sin(-1*theta));
    translated_pos.y=(obj_pos.x*sin(-1*theta))+(obj_pos.y*cos(-1*theta));
    return translated_pos;
};


void frame_center(int reference_id, int target_id, int out_id,int entries){
    string ref_str=file_string(reference_id);
    string tgt_str=file_string(target_id);
    string out_str=file_string(out_id);
    ifstream ref_file(ref_str, ios::binary);
    ifstream tgt_file(tgt_str, ios::binary);
    ofstream out_file(out_str,ios::binary);

    if (ref_file.fail()||tgt_file.fail()||out_file.fail()){
        cerr <<"An object file failed to open"<<endl;
        ref_file.close();
        tgt_file.close();
        out_file.close();
        exit(1);
    }

    vector3D ref_pos, tgt_pos, new_pos;
    for (int i=0; i<entries; i++) {
        ref_file.read(reinterpret_cast<char*>(&ref_pos), sizeof(vector3D));
        tgt_file.read(reinterpret_cast<char*>(&tgt_pos), sizeof(vector3D));

        new_pos = tgt_pos - ref_pos;

        out_file.write(reinterpret_cast<const char*>(&new_pos), sizeof(vector3D));
    }

    ref_file.close();
    tgt_file.close();
    out_file.close();
};

void frame_swap(int reference_id, int target_id, int out_id,int entries){
    string ref_str=file_string(reference_id);
    string tgt_str=file_string(target_id);
    string out_str=file_string(out_id);
    ifstream ref_file(ref_str, ios::binary);
    ifstream tgt_file(tgt_str, ios::binary);
    ofstream out_file(out_str,ios::binary);

    if (ref_file.fail()||tgt_file.fail()||out_file.fail()){
        cerr <<"An object file failed to open"<<endl;
        ref_file.close();
        tgt_file.close();
        out_file.close();
        exit(1);
    }

    vector3D ref_pos, tgt_pos, new_pos;
    for (int i=0; i<entries; i++) {
        ref_file.read(reinterpret_cast<char*>(&ref_pos), sizeof(vector3D));
        tgt_file.read(reinterpret_cast<char*>(&tgt_pos), sizeof(vector3D));

        new_pos = rot_frame(ref_pos,tgt_pos);

        out_file.write(reinterpret_cast<const char*>(&new_pos), sizeof(vector3D));
    }

    
    ref_file.close();
    tgt_file.close();
    out_file.close();
};