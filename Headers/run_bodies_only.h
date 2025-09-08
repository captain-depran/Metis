#ifndef RUN_BODIES_ONLY
#define RUN_BODIES_ONLY

#include <vector>
#include <string>


void pre_run_bodies(std::string body_file,std::string sat_file,double timespace, double stepsize, int block_size, bool log_flag,int log_freq,std::vector<int> &body_ids);


#endif