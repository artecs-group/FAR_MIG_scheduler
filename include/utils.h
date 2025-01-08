#ifndef UTILS_H
#define UTILS_H
#include "tasks.h"
#include <vector>
#include <string>
#include <nvml.h>
using namespace std;

vector<Task> get_tasks(const string & kernels_path);
void profile_tasks(vector<Task> & tasks, nvmlDevice_t device);

#endif // UTILS_H