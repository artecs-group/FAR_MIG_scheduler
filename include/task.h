#ifndef TASK_H
#define TASK_H
#include <string>
#include <unordered_map>

using namespace std;

struct Task {

    string exec_script_path; // Path to the script that executes the kernel

    unordered_map<int, double> exec_times; // Execution times profiled for the kernel

    Task(const string& path) : exec_script_path(path) {
        exec_times = {};
    }
};


#endif