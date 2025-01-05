#ifndef TASKS_H
#define TASKS_H

#include "types.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <nvml.h>

using namespace std;

struct Task {

    string name; // Name of the task

    string parent_path; // Path to the script directory
    string script_name; // Name of the script


    unordered_map<int, double> exec_times; // Execution times profiled for the kernel

    Task(string const& name, string const& parent_path, string const& script_name) : name(name), parent_path(parent_path), script_name(script_name) {
        exec_times = {};
    }

    bool execute(Instance const& instance, bool background);
    void profile_times(nvmlDevice_t device);
};

vector<Task> get_tasks(const string & kernels_path);


#endif // TASKS_H