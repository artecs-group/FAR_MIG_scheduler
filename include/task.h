#ifndef TASK_H
#define TASK_H
#include <string>

using namespace std;

struct Task {

    string execScriptPath; // Path to the script that executes the kernel

    

    Task(const string& path) : execScriptPath(path) {}
};


#endif