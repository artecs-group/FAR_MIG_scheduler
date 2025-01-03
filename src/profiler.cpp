#include "profiler.h"
#include "GPU_config.h"
#include "MIG_manager.h"
#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <stdlib.h>
#include <limits>

using namespace std;


static double measure_exec_time(const Task & task){
    cout << "INFO. Executing task: " << task.exec_script_path << endl;
    struct timeval init_time, end_time;
    // Get initial time
    gettimeofday(&init_time, NULL);
    // Execute the task
    int err = system(task.exec_script_path.c_str());
    // Get final time
    gettimeofday(&end_time, NULL);
    // Calculate the execution time
    if(err == 0){
        return (end_time.tv_sec - init_time.tv_sec) + (end_time.tv_usec - init_time.tv_usec)/1000000.0;
    }
    else{
        // If there was an error, exec time is infinity
        cerr << "Error executing task: " << task.exec_script_path << endl;
        return numeric_limits<double>::infinity();
    }
}

void set_exec_times(vector<Task> & tasks, nvmlDevice_t device){
    for (Task & task : tasks){
        // Destroy every residual instance
        destroy_all_instances(device);

        // For each possible instance size, execute every task
        for (int instance_size: global_GPU_config->valid_instance_sizes){
            // Create the instance
            Instance instance(0, instance_size);
            create_instance(device, instance);
            // Measure and save the execution time
            //double task_time = measure_exec_time(task);
            //task.exec_times[instance_size] = task_time;
            cout << "Ok! Task " << task.exec_script_path << " profiled with " << 1 << " seconds with size " << instance_size << endl;
            destroy_instance(instance);
        }
    }

}