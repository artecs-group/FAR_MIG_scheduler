
#include "GPU_config.h"
#include <nvml.h>
#include <iostream>

using namespace std;

// Configuration for A30
const GPUConfig A30_config = {
    "A30",
    {1, 2, 4},   // Valid sizes for the instances

    {{1, NVML_GPU_INSTANCE_PROFILE_1_SLICE},
     {2, NVML_GPU_INSTANCE_PROFILE_2_SLICE},
     {4, NVML_GPU_INSTANCE_PROFILE_4_SLICE}}, // Valid profiles for the GPU instances

    {{1, NVML_COMPUTE_INSTANCE_PROFILE_1_SLICE},
     {2, NVML_COMPUTE_INSTANCE_PROFILE_2_SLICE},
     {4, NVML_COMPUTE_INSTANCE_PROFILE_4_SLICE}}, // Valid profiles for the compute instances

    4            // Total number of slices
};

// Configuration for A100/H100
const GPUConfig A100_H100_config = {
    "A100/H100",
    {1, 2, 3, 4, 7},  // Valid sizes for the instances

    {{1, NVML_GPU_INSTANCE_PROFILE_1_SLICE},
     {2, NVML_GPU_INSTANCE_PROFILE_2_SLICE},
     {3, NVML_GPU_INSTANCE_PROFILE_3_SLICE},
     {4, NVML_GPU_INSTANCE_PROFILE_4_SLICE},
     {7, NVML_GPU_INSTANCE_PROFILE_7_SLICE},}, // Valid profiles for the GPU instances

    {{1, NVML_COMPUTE_INSTANCE_PROFILE_1_SLICE},
     {2, NVML_COMPUTE_INSTANCE_PROFILE_2_SLICE},
     {3, NVML_COMPUTE_INSTANCE_PROFILE_3_SLICE},
     {4, NVML_COMPUTE_INSTANCE_PROFILE_4_SLICE},
     {7, NVML_COMPUTE_INSTANCE_PROFILE_7_SLICE}}, // Valid profiles for the compute instances

    7                 // Total number of slices
};

// Global GPU configuration
const GPUConfig* global_GPU_config = nullptr;

// Procedure to initialize the GPU configuration
void initialize_GPU_config(const string& gpu_name){
    if (gpu_name == "NVIDIA A30") {
        global_GPU_config = &A30_config;
    } else if (gpu_name == "NVIDIA A100" || gpu_name == "NVIDIA H100") {
        global_GPU_config = &A100_H100_config;
    } else {
        cerr << "GPU model unknown: " + gpu_name << endl;
        exit(1);
    }
}