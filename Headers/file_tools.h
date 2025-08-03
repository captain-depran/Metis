#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <string>
#include <vector>
#include "../Headers/body.h"

std::string file_string(int id);
void file_wipe(int id);
void load_body_file(std::string file_name,std::vector<body>& output_body_list);

#endif