#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <nvml.h>
#include "task.h"

struct Instance{
    size_t start, size;
    nvmlGpuInstance_t gpuInstance = 0;
    nvmlComputeInstance_t computeInstance = 0;

    // Constructor para inicialización con lista
    Instance(size_t start, size_t size) : start(start), size(size) {}
    bool operator==(const Instance& other) const {
        return start == other.start && size == other.size;
    }
};

vector<Task> validate_scripts(const string & kernels_filename);

#endif