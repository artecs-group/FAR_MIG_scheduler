#include "profiler.h"
#include "GPU_config.h"
#include "MIG_manager.h"

using namespace std;

void set_exec_times(const vector<Task> & tasks, nvmlDevice_t device){
    for (const Task & task : tasks){
        destroy_all_instances(device);
        for (int instance_size: global_GPU_config->valid_instance_sizes){
            
        }
    }


}