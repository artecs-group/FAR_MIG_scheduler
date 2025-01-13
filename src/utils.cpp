#include "utils.h"
#include "logging.h"
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <algorithm>

using namespace std;

void profile_tasks(vector<Task> & tasks, nvmlDevice_t device){
    vector<Task*> tasks_failed;
    for (auto & task: tasks){
        task.profile_times(device);
        if (task.has_error()){
            tasks_failed.push_back(&task);
        }
    }
    // Remove the tasks that failed from the list
    for (auto task: tasks_failed){
        tasks.erase(remove(tasks.begin(), tasks.end(), *task), tasks.end());
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

    LOG_INFO("Tasks loaded from " + kernels_filename  + ":");
    for (auto const& task: tasks){
        cout << task.name << endl;
    }
    cout << endl;
    return tasks;
}