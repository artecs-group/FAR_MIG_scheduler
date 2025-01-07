#ifndef MIG_MANAGER_H
#define MIG_MANAGER_H

#include <iostream>
#include <string>
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

void init_nvml();
nvmlDevice_t bind_device(int gpu_number);
string get_gpu_name(nvmlDevice_t device);
void MIG_enable(nvmlDevice_t device, int gpu_number);
void MIG_disable(nvmlDevice_t device, int gpu_number);

void destroy_all_instances(nvmlDevice_t device);
Instance create_instance(nvmlDevice_t device, size_t start, size_t size);
void destroy_instance(Instance const& instance);

#endif