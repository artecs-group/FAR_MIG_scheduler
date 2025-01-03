#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <nvml.h>
#include "task.h"

using namespace std;

// Macro for logging errors
#define LOG_ERROR(msg) (cerr << "ERROR: " << msg << " (File: " << __FILE__ << ", Line: " << __LINE__ << ")\n")

struct Instance{
    size_t start, size;
    nvmlGpuInstance_t gpuInstance = 0;
    nvmlComputeInstance_t computeInstance = 0;

    // Constructor para inicialización con lista
    Instance(size_t start, size_t size) : start(start), size(size) {}
    bool operator==(const Instance& other) const {
        return start == other.start && size == other.size;
    }

    // Printable instances
    friend ostream& operator<<(ostream& os, const Instance& instance) {
        os << "Instance(start=" << instance.start << ", size=" << instance.size << ")";
        return os;
    }
};

vector<Task> validate_scripts(const string & kernels_filename);

#endif