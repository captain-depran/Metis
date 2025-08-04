#include "../Headers/file_tools.h"
#include "../Headers/body.h"
#include "../Headers/kep_to_cart.h"
#include "../Headers/constants.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

std::string file_string(int id){
    char file_name[6];
    sprintf(file_name,"%d",id);
    std::string str("OUTPUT/");
    str.append(file_name);
    str.append(".bin");
    return str;
};

void file_wipe(int id){
    std::string str=file_string(id);
    std::ofstream log_file(str,std::ofstream::out | std::ofstream::trunc);
    log_file.close();
}

void load_body_file(std::string file_name,std::vector<body>& output_body_list){
    body_import current;
    std::vector<body_import> imported_bodies;
    std::ifstream body_file(file_name);
    std::string line;
    double host_mass = 1.989e30;
    bool in_body_block=false;

    enum key_type {PARENT, MASS, RADIUS, SEMI_MAJ, ECC, INC, LONG_ASC_NODE, ARG_PERI, TRUE_ANOM, UNKNOWN};
    std::map<std::string, key_type> key_map{
        {"PARENT", PARENT},
        {"MASS", MASS},
        {"RADIUS", RADIUS},
        {"SEMI_MAJ", SEMI_MAJ},
        {"ECC", ECC},
        {"INC", INC},
        {"LONG_ASC_NODE", LONG_ASC_NODE},
        {"ARG_PERI", ARG_PERI},
        {"TRUE_ANOM", TRUE_ANOM},
        {"UNKNOWN", UNKNOWN}
    };

    if (!body_file.is_open()){
        std::cout<<"ERROR OPENING BODY FILE"<<std::endl;
        return;
    }
    while(getline(body_file,line)){
        if (line.empty() || line[0] == '#') continue;

        else if (line.rfind("BODY",0)==0){
            //BODY BLOCK DETECTION
            std::stringstream ss(line);
            std::string temp;
            ss >> temp >> current.id >> current.name;
            in_body_block=true;
            std::cout<<"DETECTED BODY: "<<current.name<<std::endl;            
        }
        else if(line.rfind("END",0)==0){
            //END BODY BLOCK
            in_body_block=false;
            imported_bodies.push_back(current);
            current={};
        }
        else if(in_body_block){
            std::size_t pos = line.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            switch (key_map.count(key)?key_map[key]:UNKNOWN){
                case PARENT:
                    current.parent=value;
                    break;
                case MASS:
                    current.mass=std::stod(value);
                    break;
                case RADIUS:
                    current.radius=std::stod(value);
                    break;                    
                case SEMI_MAJ:
                    current.semi_maj=std::stod(value);
                    break;                
                case ECC:
                    current.ecc=std::stod(value);
                    break;
                case INC:
                    current.inc=(std::stod(value))*(PI/180);
                    break;  
                case LONG_ASC_NODE:
                    current.long_asc_node=(std::stod(value))*(PI/180);
                    break; 
                case ARG_PERI:
                    current.arg_peri=(std::stod(value))*(PI/180);
                    break;          
                case TRUE_ANOM:
                    current.true_anom=(std::stod(value))*(PI/180);
                    break;   
                default:
                    std::cout<<"UNKNOWN PARAMETER IN FILE!!"<<std::endl;
                    break;
            }
        }   
    }
    body_file.close();
    for (body_import& obj:imported_bodies){
        if (obj.parent=="NONE"||obj.parent==""){
            host_mass = obj.mass;
            state_vector inject_state=cart_state(host_mass,obj.semi_maj,obj.ecc,obj.inc,obj.long_asc_node,obj.arg_peri,obj.true_anom);
            obj.r=inject_state.r;
            obj.v=inject_state.v;
        }
        else{
            for (body_import& other_obj:imported_bodies){
                if (other_obj.name==obj.parent){
                    host_mass=other_obj.mass;
                    state_vector inject_state=cart_state(host_mass,obj.semi_maj,obj.ecc,obj.inc,obj.long_asc_node,obj.arg_peri,obj.true_anom);
                    obj.r=inject_state.r+other_obj.r;
                    obj.v=inject_state.v+other_obj.v;
                }
            }
        }

        output_body_list.push_back(body(obj.id,obj.mass,obj.r,obj.v));
        output_body_list.back().pos.print();
    }
    std::cout<<"BODY LOADING COMPLETED!"<<std::endl;
    
}

