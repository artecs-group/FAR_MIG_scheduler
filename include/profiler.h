#ifndef PROFILER_H
#define PROFILER_H

#include "task.h"
#include <vector>
#include <nvml.h>
using namespace std;

void set_exec_times(const vector<Task> & tasks, nvmlDevice_t device);

#endif // PROFILER_H