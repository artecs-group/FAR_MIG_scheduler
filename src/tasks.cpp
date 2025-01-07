#include "tasks.h"
#include "GPU_info.h"
#include "logging.h"
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <cfloat>

using namespace std;


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


bool Task::execute(Instance const& instance, bool background){
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

void profile_tasks(vector<Task> & tasks, nvmlDevice_t device){
    for (auto & task: tasks){
        task.profile_times(device);
    }
    // Show the profiled times
    cout << "======================================" << endl;
    cout << "Profiled times for the tasks:" << endl;
    for (auto const& task: tasks){
        cout << "Task " << task.name << ":" << endl;
        for (auto const& exec_time: task.exec_times){
            cout << "Size " << exec_time.first << ": " << exec_time.second << " seconds" << endl;
        }
    }
    cout << "======================================" << endl;
}

static bool valid_exec_path(const string & path){
    struct stat fileInfo;
    // Verify if the path is valid and executable
    bool isValid = (stat(path.c_str(), &fileInfo) == 0) && (fileInfo.st_mode & S_IXUSR);
    if (!isValid){
        LOG_ERROR("Path " + path + " doesn't exist or is not executable");
    }
    return isValid;
}

vector<Task> get_tasks(const string & kernels_filename){    
    vector<Task> tasks;

    ifstream file(kernels_filename);
    if (!file.is_open()){
        LOG_ERROR("Could not open file " + kernels_filename);
        exit(1);
    }
    string name, parent_path, script_name; // Task data
    while (file >> name >> parent_path >> script_name){
        // Validate path as executable file
        string path = parent_path + "/" + script_name;
        bool isValid = valid_exec_path(path);
        if (isValid) tasks.push_back(Task(name, parent_path, script_name));
    }
    file.close();
    return tasks;
}