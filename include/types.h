#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <nvml.h>

using namespace std;

struct Instance {
    size_t start, size;
    nvmlGpuInstance_t gpuInstance;
    nvmlComputeInstance_t computeInstance;
    string uuid;

    Instance(size_t start, size_t size, nvmlGpuInstance_t gpuInstance, nvmlComputeInstance_t computeInstance, string uuid) : start(start), size(size), gpuInstance(gpuInstance), computeInstance(computeInstance), uuid(uuid) {}

    bool operator==(const Instance& other) const {
        return start == other.start && size == other.size;
    }

    // Printable instances
    friend ostream& operator<<(ostream& os, const Instance& instance) {
        os << "Instance(start=" << instance.start << ", size=" << instance.size << ")";
        return os;
    }
};

#endif // TYPES_H