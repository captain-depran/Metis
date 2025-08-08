#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <string>
#include <vector>
#include <memory>
#include "../Headers/spacecraft.h"
#include "../Headers/body.h"

struct sim_settings{
    std::string body_file_name = "INPUT/";
    std::string sat_file_name = "INPUT/";
    double timespan;
    double step_size;
    int log_freq;
    int buffer_size;
};

std::string file_string(int id);
void file_wipe(int id);
void load_body_file(std::string file_name,std::vector<body>& output_body_list);
spacecraft load_craft_file(std::string file_name,std::vector<body>&mass_bodies);
void load_mnvrs(std::ifstream& sat_file, spacecraft& craft);
sim_settings load_settings_file(std::string file_name);


#endif