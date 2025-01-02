#ifndef TASK_H
#define TASK_H
#include <string>
#include <unordered_map>

using namespace std;

struct Task {

    string execScriptPath; // Path to the script that executes the kernel

    unordered_map<int, double> execTimes; // Execution times profiled for the kernel

    Task(const string& path) : execScriptPath(path) {}
};


#endif