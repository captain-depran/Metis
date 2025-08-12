#include "../Headers/file_tools.h"
#include "../Headers/body.h"
#include "../Headers/kep_cart.h"
#include "../Headers/spacecraft.h"
#include "../Headers/constants.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

std::string file_string(int id){
    std::string str("OUTPUT/");
    str=str+std::to_string(id);
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
    double host_mass;
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
    int current_index=0;
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
        body new_body=body(obj.id,obj.mass,obj.r,obj.v);
        new_body.set_name(obj.name);
        new_body.system_index=current_index;
        output_body_list.push_back(new_body);
        current_index++;
    }
    std::cout<<"BODY LOADING COMPLETED!"<<std::endl;
    
}

spacecraft load_craft_file(std::string file_name,std::vector<body>&mass_bodies){
    body_import sat_params;
    std::ifstream sat_file(file_name);
    std::string line;
    double host_mass;
    bool injection=true;
    bool reached_params=false;
    bool valid_sat=false;

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

    if (!sat_file.is_open()){
        std::cout<<"ERROR OPENING SPACECRAFT FILE"<<std::endl;
        return spacecraft(0,0,0,0,0,0,0,0);
    }
    while((injection==true)&&getline(sat_file,line)){
        if (line.empty() || line[0] == '#') continue;

        else if (line.rfind("BODY",0)==0){
            //BODY BLOCK DETECTION
            std::stringstream ss(line);
            std::string temp;
            ss >> temp >> sat_params.id >> sat_params.name;
            std::cout<<"DETECTED SPACECRAFT: "<<sat_params.name<<std::endl;
            reached_params=true;            
        }
        else if(line.rfind("END",0)==0){
            //END BODY BLOCK
            reached_params=false;
            injection=false;
        }
        else if(reached_params){
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
                    sat_params.parent=value;
                    break;
                case MASS:
                    sat_params.mass=std::stod(value);
                    break;
                case RADIUS:
                    sat_params.radius=std::stod(value);
                    break;                    
                case SEMI_MAJ:
                    sat_params.semi_maj=std::stod(value);
                    break;                
                case ECC:
                    sat_params.ecc=std::stod(value);
                    break;
                case INC:
                    sat_params.inc=(std::stod(value))*(PI/180);
                    break;  
                case LONG_ASC_NODE:
                    sat_params.long_asc_node=(std::stod(value))*(PI/180);
                    break; 
                case ARG_PERI:
                    sat_params.arg_peri=(std::stod(value))*(PI/180);
                    break;          
                case TRUE_ANOM:
                    sat_params.true_anom=(std::stod(value))*(PI/180);
                    break;   
                default:
                    std::cout<<"UNKNOWN PARAMETER IN FILE!!"<<std::endl;
                    break;
            }
        }   
    }
    
    for (body& obj:mass_bodies){
        if (obj.name==sat_params.parent){
            host_mass=obj.mass;
            state_vector inject_state=cart_state(host_mass,sat_params.semi_maj,sat_params.ecc,sat_params.inc,sat_params.long_asc_node,sat_params.arg_peri,sat_params.true_anom);
            sat_params.r=inject_state.r+obj.pos;
            sat_params.v=inject_state.v+obj.vel;
            valid_sat=true;
            break;
        };
    }
    if (valid_sat){
        spacecraft craft=spacecraft(sat_params.id,sat_params.mass,sat_params.r,sat_params.v);
        craft.set_name(sat_params.name);
        load_mnvrs(sat_file, craft);
        sat_file.close();
        std::cout<<"SPACECRAFT LOADING COMPLETED!"<<std::endl;
        return craft;
    }
    else{
        sat_file.close();
        std::cout<<"SPACECRAFT: INVALID PARENT"<<std::endl;
        return spacecraft(0,0,0,0,0,0,0,0);
    }
}

void load_mnvrs(std::ifstream& sat_file, spacecraft& craft){
    enum trigger_type {TIME,PE,AP,NODE,ANOM,UNKNOWN};

    std::map<std::string, trigger_type> trig_map{
        {"TIME", TIME},
        {"PE", PE},
        {"AP", AP},
        {"NODE", NODE},
        {"ANOM", ANOM},
        {"UNKNOWN", UNKNOWN}
    };

    std::string line;
    while(getline(sat_file,line)){
        if (line.empty() || line[0] == '#') continue;

        else if(line.rfind("MNV",0)==0){
            std::stringstream ss(line);
            std::string item;
            std::vector<std::string> parts;
            while (std::getline(ss,item,':')){
                parts.push_back(item);
            }
            vector3D dv=vector3D(std::stod(parts[2]),std::stod(parts[3]),std::stod(parts[4]));
            switch (trig_map.count(parts[5])?trig_map[parts[5]]:UNKNOWN){
                case TIME:
                    craft.all_manouvers.push_back(manouver(dv, parts[1], std::stod(parts[6])));
                    break;
                case PE:
                    break;
                case AP:
                    break;
                case NODE:
                    break;
                case ANOM:
                    break;
                default:
                    std::cout<<"UNKNOWN PARAMETER IN FILE!!"<<std::endl;
                    break;
            }
        };
    }

}


sim_settings load_settings_file(std::string file_name){

    sim_settings settings;
    std::ifstream config_file(file_name);
    std::string line;

    enum key_type {BODY_FILE, SAT_FILE, TIMESPAN, STEP_SIZE, LOG_FREQ, BUFFER_SIZE, CORE_BODY, UNKNOWN};
    std::map<std::string, key_type> key_map{
        {"BODY_FILE", BODY_FILE},
        {"SAT_FILE", SAT_FILE},
        {"TIMESPAN", TIMESPAN},
        {"STEP_SIZE", STEP_SIZE},
        {"LOG_FREQ", LOG_FREQ},
        {"BUFFER_SIZE", BUFFER_SIZE},
        {"CORE_BODY", CORE_BODY},
        {"UNKNOWN", UNKNOWN}
    };
    if (!config_file.is_open()){
        std::cout<<"ERROR OPENING CONFIG FILE"<<std::endl;
    };
    while(getline(config_file,line)){
        if (line.empty() || line[0] == '#') continue;
        
        else{
            std::size_t pos = line.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            switch (key_map.count(key)?key_map[key]:UNKNOWN){
                case BODY_FILE:
                    settings.body_file_name.append(value);
                    break;
                case SAT_FILE:
                    settings.sat_file_name.append(value);
                    break;
                case TIMESPAN:
                    settings.timespan=std::stod(value);
                    break;                    
                case STEP_SIZE:
                    settings.step_size=std::stod(value);
                    break;                
                case LOG_FREQ:
                    settings.log_freq=std::stod(value);
                    break;
                case BUFFER_SIZE:
                    settings.buffer_size=std::stod(value);
                    break;
                case CORE_BODY:
                    settings.core_body_id=std::stoi(value);
                    break;    
                default:
                    std::cout<<"UNKNOWN PARAMETER IN FILE!!"<<std::endl;
                    break;
            }
        } 

        };
    config_file.close();
    return settings;
}