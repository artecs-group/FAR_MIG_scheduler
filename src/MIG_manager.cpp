#include "MIG_manager.h"
#include <iostream>
#include <cuda_runtime.h>
#include <nvml.h>

using namespace std;

int get_gpu_number(const string & gpu_name){
    int device_count = 0;

    // Get number of GPUs available
    cudaGetDeviceCount(&device_count);
    if (device_count == 0) {
        return -1;
    }

    // Verify if some GPU has the desired name
    for (int i = 0; i < device_count; ++i) {
        cudaDeviceProp device_prop;
        cudaGetDeviceProperties(&device_prop, i);
        if (gpu_name == device_prop.name) {
            return i;
        }
    }
    return -1;
}

void show_available_gpus(){
    int device_count = 0;
    cudaGetDeviceCount(&device_count);
    if (device_count == 0) {
        cerr << "There are no NVIDIA GPUs in the system." << endl;
    }
    for (int i = 0; i < device_count; ++i) {
        cudaDeviceProp device_prop;
        cudaGetDeviceProperties(&device_prop, i);
        cout << "\t- " << device_prop.name << endl;
    }
}

void init_nvml(){
    nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS) {
        cerr << "Failed to initialize NVML: " << nvmlErrorString(result) << endl;
        exit(1);
    }
    cout << "NVML has been initialized" << endl;
}

nvmlDevice_t bind_device(int gpu_number){
    nvmlDevice_t device;
    nvmlReturn_t result = nvmlDeviceGetHandleByIndex(gpu_number, &device);
    if (result != NVML_SUCCESS) {
        cerr << "Failed to bind device: " << nvmlErrorString(result) << endl;
        exit(1);
    }
    cout << "Device has been binded" << endl;
    return device;
}