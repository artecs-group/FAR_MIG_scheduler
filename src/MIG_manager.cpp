#include "MIG_manager.h"
#include "GPU_config.h"
#include <iostream>
#include <cuda_runtime.h>
#include <nvml.h>
#include <stdlib.h>

using namespace std;

string get_gpu_name(nvmlDevice_t device){
   // Obtener el nombre de la GPU
   char name[NVML_DEVICE_NAME_BUFFER_SIZE];
   nvmlReturn_t result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
    if (result == NVML_SUCCESS) {
        cout << "Ok! GPU model: " << name << " detected" << endl;
    } else {
        cerr << "Error obteniendo el nombre de la GPU: " << nvmlErrorString(result) << endl;
    }
    return string(name);
}

void init_nvml(){
    nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS) {
        cerr << "Failed to initialize NVML: " << nvmlErrorString(result) << endl;
        exit(1);
    }
    cout << "Ok! NVML has been initialized" << endl;
}

nvmlDevice_t bind_device(int gpu_number){
    nvmlDevice_t device;
    nvmlReturn_t result = nvmlDeviceGetHandleByIndex(gpu_number, &device);
    if (result != NVML_SUCCESS) {
        cerr << "Failed to bind device: " << nvmlErrorString(result) << endl;
        exit(1);
    }
    cout << "Ok! Device has been binded" << endl;
    return device;
}

void MIG_enable(nvmlDevice_t device, int gpu_number){
    nvmlReturn_t status;
    nvmlReturn_t result = nvmlDeviceSetMigMode(device, NVML_DEVICE_MIG_ENABLE, &status);
    if (result != NVML_SUCCESS) {
        cerr << "Failed to activate MIG: " << nvmlErrorString(result) << endl;
    }
    cout << "Ok! MIG has been activated" << endl;
}

void MIG_disable(nvmlDevice_t device, int gpu_number){
    nvmlReturn_t status;
    nvmlReturn_t result = nvmlDeviceSetMigMode(device, NVML_DEVICE_MIG_DISABLE, &status);
    if (result != NVML_SUCCESS) {
        cerr << "Failed to deactivate MIG: " << nvmlErrorString(result) << endl;
    }
    cout << "Ok! MIG has been deactivated" << endl;
}


void destroy_all_instances(nvmlDevice_t device){
    int destroyed_count = 0;
    nvmlGpuInstance_t * gpu_instances = (nvmlGpuInstance_t *) malloc(sizeof(nvmlGpuInstance_t) * global_GPU_config->num_slices);
    
    for (auto & gpu_instance_profile: global_GPU_config->valid_gi_profiles){
        int instance_size = gpu_instance_profile.first;
        unsigned int profile = gpu_instance_profile.second;
        nvmlGpuInstanceProfileInfo_t info;
        nvmlReturn_t result = nvmlDeviceGetGpuInstanceProfileInfo(device, profile, &info);
        if(result != NVML_SUCCESS){
            cerr << "Failed to get GPU instance profile info: " << nvmlErrorString(result) << endl;
            exit(1);
        }
        unsigned int count;
        result = nvmlDeviceGetGpuInstances(device, info.id, gpu_instances, &count);
        if(result != NVML_SUCCESS){
            cerr << "Failed to get GPU instances: " << nvmlErrorString(result) << endl;
            exit(1);
        }
        for (int i = 0; i < count; i++){
            result = nvmlGpuInstanceDestroy(gpu_instances[i]);
            if(result != NVML_SUCCESS){
                cerr << "Failed to destroy GPU instance: " << nvmlErrorString(result) << endl;
                exit(1);
            }
        }
        destroyed_count += count;
    }

    cout << "All GPU instances have been destroyed: " << destroyed_count << " instances" << endl;

    free(gpu_instances);

}

void create_instance(nvmlDevice_t device, Instance & instance){
    nvmlReturn_t result;
    
    // Create GPU instance
    nvmlGpuInstance_t gpuInstance;
    nvmlGpuInstancePlacement_t placement;
    placement.start = instance.start;
    placement.size = instance.size;

    // Get GPU instance profile
    nvmlGpuInstanceProfileInfo_t gi_info;
    result = nvmlDeviceGetGpuInstanceProfileInfo (device, global_GPU_config->valid_gi_profiles.at(placement.size), &gi_info);
    if(NVML_SUCCESS != result){
            cout << "Failed getting gpu instance ID: " << nvmlErrorString(result) << endl;
            exit(1);
    }

    // Create GPU instance
    result = nvmlDeviceCreateGpuInstanceWithPlacement(device, gi_info.id, &placement, &gpuInstance );
    if(NVML_SUCCESS != result){
        cout << "Failed creating gpu instance: " << nvmlErrorString(result) << endl;
        exit(1);
    }
    instance.gpuInstance = gpuInstance;


    // Get compute instance profile
    nvmlComputeInstanceProfileInfo_t ci_info;
    
    result = nvmlGpuInstanceGetComputeInstanceProfileInfo (gpuInstance, global_GPU_config->valid_ci_profiles.at(placement.size), NVML_COMPUTE_INSTANCE_ENGINE_PROFILE_SHARED, &ci_info );
    if(NVML_SUCCESS != result){
            cout << "Failed getting compute instance ID: " << nvmlErrorString(result) << endl;
            exit(1);
    }
    // Create compute instance
    nvmlComputeInstance_t computeInstance;
    result =  nvmlGpuInstanceCreateComputeInstance (gpuInstance, ci_info.id, &computeInstance );
    if(NVML_SUCCESS != result){
            cout << "Failed creating compute instance: " << nvmlErrorString(result) << endl;
            exit(1);
    }
    instance.computeInstance = computeInstance;
}