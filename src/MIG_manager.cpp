#include "MIG_manager.h"
#include "GPU_config.h"
#include "utils.h"
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
        LOG_INFO("GPU model: " + string(name) + " detected");
    } else {
        LOG_ERROR("Error obteniendo el nombre de la GPU: " + string(nvmlErrorString(result)));
    }
    return string(name);
}

void init_nvml(){
    nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS) {
        LOG_ERROR("Failed to initialize NVML: " + string(nvmlErrorString(result)));
        exit(1);
    }
    LOG_INFO("NVML has been initialized");
}

nvmlDevice_t bind_device(int gpu_number){
    nvmlDevice_t device;
    nvmlReturn_t result = nvmlDeviceGetHandleByIndex(gpu_number, &device);
    if (result != NVML_SUCCESS) {
        LOG_ERROR("Failed to bind device: " + string(nvmlErrorString(result)));
        exit(1);
    }
    LOG_INFO("Device has been binded");
    return device;
}

void MIG_enable(nvmlDevice_t device, int gpu_number){
    nvmlReturn_t status;
    nvmlReturn_t result = nvmlDeviceSetMigMode(device, NVML_DEVICE_MIG_ENABLE, &status);
    if (result != NVML_SUCCESS) {
        LOG_ERROR("Failed to activate MIG: " + string(nvmlErrorString(result)));
    }
    LOG_INFO("MIG has been activated");
}

void MIG_disable(nvmlDevice_t device, int gpu_number){
    nvmlReturn_t status;
    nvmlReturn_t result = nvmlDeviceSetMigMode(device, NVML_DEVICE_MIG_DISABLE, &status);
    if (result != NVML_SUCCESS) {
        LOG_ERROR("Failed to deactivate MIG: " + string(nvmlErrorString(result)));
    }
    LOG_INFO("Ok! MIG has been deactivated");
}

static void destroy_all_compute_instances(nvmlGpuInstance_t gpu_instance, unsigned int ci_profile){
    nvmlComputeInstanceProfileInfo_t ci_info;
    nvmlReturn_t result = nvmlGpuInstanceGetComputeInstanceProfileInfo (gpu_instance, ci_profile, NVML_COMPUTE_INSTANCE_ENGINE_PROFILE_SHARED, &ci_info);
    if(result != NVML_SUCCESS){
        LOG_ERROR("Failed to get compute instance profile info: " + string(nvmlErrorString(result)));
        exit(1);
    }

    nvmlComputeInstance_t * compute_instances = (nvmlComputeInstance_t *) malloc(sizeof(nvmlComputeInstance_t) * global_GPU_config->num_slices);
    unsigned int compute_count;
    result = nvmlGpuInstanceGetComputeInstances(gpu_instance, ci_info.id, compute_instances, &compute_count);
    if(result != NVML_SUCCESS){
        LOG_ERROR("Failed to get compute instances: " + string(nvmlErrorString(result)));
        exit(1);
    }
    for (int j = 0; j < compute_count; j++){
        result = nvmlComputeInstanceDestroy(compute_instances[j]);
        if(result != NVML_SUCCESS){
            LOG_ERROR("Failed to destroy compute instance: " + string(nvmlErrorString(result)));
            exit(1);
        }
    }

    free(compute_instances);
}

void destroy_all_instances(nvmlDevice_t device){
    int destroyed_count = 0;
    nvmlGpuInstance_t * gpu_instances = (nvmlGpuInstance_t *) malloc(sizeof(nvmlGpuInstance_t) * global_GPU_config->num_slices);
    
    for (auto const& gpu_instance_profile: global_GPU_config->valid_gi_profiles){
        int instance_size = gpu_instance_profile.first;
        unsigned int profile = gpu_instance_profile.second;
        nvmlGpuInstanceProfileInfo_t info;
        nvmlReturn_t result = nvmlDeviceGetGpuInstanceProfileInfo(device, profile, &info);
        if(result != NVML_SUCCESS){
            LOG_ERROR("Failed to get GPU instance profile info: " + string(nvmlErrorString(result)));
            exit(1);
        }
        unsigned int count;
        result = nvmlDeviceGetGpuInstances(device, info.id, gpu_instances, &count);
        if(result != NVML_SUCCESS){
            LOG_ERROR("Failed to get GPU instances: " + string(nvmlErrorString(result)));
            exit(1);
        }
        for (int i = 0; i < count; i++){
            unsigned int ci_profile = global_GPU_config->valid_ci_profiles.at(instance_size);
            destroy_all_compute_instances(gpu_instances[i], ci_profile);

            result = nvmlGpuInstanceDestroy(gpu_instances[i]);
            if(result != NVML_SUCCESS){
                LOG_ERROR("Failed to destroy GPU instance: " + string(nvmlErrorString(result)));
                exit(1);
            }
        }
        destroyed_count += count;
    }

    LOG_INFO("All GPU instances have been destroyed: " + to_string(destroyed_count) << " instances");

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
            LOG_ERROR("Failed getting gpu instance ID: " + string(nvmlErrorString(result)));
            exit(1);
    }

    // Create GPU instance
    result = nvmlDeviceCreateGpuInstanceWithPlacement(device, gi_info.id, &placement, &gpuInstance );
    if(NVML_SUCCESS != result){
        LOG_ERROR("Failed creating gpu instance: " + string(nvmlErrorString(result)));
        exit(1);
    }
    instance.gpuInstance = gpuInstance;


    // Get compute instance profile
    nvmlComputeInstanceProfileInfo_t ci_info;
    
    result = nvmlGpuInstanceGetComputeInstanceProfileInfo (gpuInstance, global_GPU_config->valid_ci_profiles.at(placement.size), NVML_COMPUTE_INSTANCE_ENGINE_PROFILE_SHARED, &ci_info );
    if(NVML_SUCCESS != result){
            LOG_ERROR("Failed getting compute instance ID: " + string(nvmlErrorString(result)));
            exit(1);
    }
    // Create compute instance
    nvmlComputeInstance_t computeInstance;
    result =  nvmlGpuInstanceCreateComputeInstance (gpuInstance, ci_info.id, &computeInstance );
    if(NVML_SUCCESS != result){
            LOG_ERROR("Failed creating compute instance: " + string(nvmlErrorString(result)));
            exit(1);
    }
    instance.computeInstance = computeInstance;
    cout << "INFO: " << instance << " has been created\n";
}


void destroy_instance(Instance const& instance){
    nvmlReturn_t result = nvmlComputeInstanceDestroy(instance.computeInstance);
    if(result != NVML_SUCCESS){
        LOG_ERROR("Failed to destroy compute instance: " + string(nvmlErrorString(result)));
        exit(1);
    }
    result = nvmlGpuInstanceDestroy(instance.gpuInstance);
    if(result != NVML_SUCCESS){
        LOG_ERROR("Failed to destroy GPU instance: " + string(nvmlErrorString(result)));
        exit(1);
    }
}