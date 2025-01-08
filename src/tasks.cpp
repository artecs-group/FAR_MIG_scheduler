#include "tasks.h"
#include "GPU_info.h"
#include "logging.h"
#include <sys/time.h>
#include <stdexcept>
#include <cfloat>
#include <unistd.h>

using namespace std;

Task::Task(string const& name, string const& parent_path, string const& script_name) : name(name), parent_path(parent_path), script_name(script_name) {
    exec_times = {};
}

static string exec_command(Task const& task, Instance const& instance, bool background){
    // Move to task directory
    string command = "cd " + task.parent_path;
    // Concatenate the command to execute the script
    command += " && CUDA_VISIBLE_DEVICES=" + instance.uuid + " sh " + task.script_name;

    // Get current directory
    char currentDirectory[1024];
    if (getcwd(currentDirectory, sizeof(currentDirectory)) == nullptr) {
        LOG_ERROR("Get current directory");
        exit(1);
    }

    // TO DO: Redirect the output to a log file for the task
    // command += " &>> " + string(currentDirectory) + "/logs/" + task.name + ".log";

    // Return to the original directory
    command += " && cd " + string(currentDirectory);

    // If the task is to be executed in the background, add to the command
    if (background){
        command = "(" + command + ") &";
    }
    return command;
} 


bool Task::execute(Instance const& instance, bool background) const{
    // Execute the task in the given instance
    string command = exec_command(*this, instance, background);
    int status = system(command.c_str());
    if (status != 0){
        cout << "ERROR: Task " << this->name << " failed with " << instance << endl;
        // If there was an error executing the task, throw an exception to set infinite time for it
        throw runtime_error("Task execution failed");
    }
    return true;
}

void Task::profile_times(nvmlDevice_t device){
    destroy_all_instances(device);
    struct timeval init_time, end_time;
    // For each possible instance size, execute every task
    for (int instance_size: global_GPU_info->valid_instance_sizes){
        // Create the instance
        Instance instance = create_instance(device, 0, instance_size);
        try{
            // Measure and save the execution time
            gettimeofday(&init_time, NULL);
            this->execute(instance, false);
            gettimeofday(&end_time, NULL);
        } catch (runtime_error const& e){
            // If the task failed, set infinite time for it
            this->exec_times[instance_size] = DBL_MAX;
            LOG_ERROR("Task " + this->name + " failed with size " + to_string(instance_size) + " and has infinite time");
            destroy_instance(instance);
            continue;
        }
        double task_time = (end_time.tv_sec - init_time.tv_sec) + (end_time.tv_usec - init_time.tv_usec) / 1000000.0;
        this->exec_times[instance_size] = task_time;
        LOG_INFO("Task " + this->name + " profiled with " + to_string(task_time) + " seconds with size " + to_string(instance_size));
        destroy_instance(instance);
    }
}