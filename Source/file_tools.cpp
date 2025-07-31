#include "../Headers/file_tools.h"
#include <iostream>
#include <fstream>

std::string file_string(int id){
    char file_name[6];
    sprintf(file_name,"%d",id);
    std::string str(file_name);
    str.append(".bin");
    return str;
};