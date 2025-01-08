#ifndef TASKS_H
#define TASKS_H

#include "MIG_manager.h"
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

    Task(string const& name, string const& parent_path, string const& script_name);

    bool execute(Instance const& instance, bool background) const;
    void profile_times(nvmlDevice_t device);

};

#endif // TASKS_H