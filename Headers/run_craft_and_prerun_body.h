#ifndef RUN_CRAFT_AND_PRERUN_BODY
#define RUN_CRAFT_AND_PRERUN_BODY

#include <vector>
#include <string>


void run_sat(std::string body_file,std::string sat_file,double timespace, double stepsize, int block_size, bool log_flag,int log_freq,std::vector<int> &body_ids);


#endif