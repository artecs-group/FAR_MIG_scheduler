#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include <unordered_map>
#include "scheduler.h"

using namespace std;


//Struct for the GPU scheduling configuration
struct GPUConfig {
    string name;
    vector<int> valid_instance_sizes; // Valid sizes for the instances
    unordered_map<int, unsigned int> valid_gi_profiles;   // Valid profiles for the GPU instances
    unordered_map<int, unsigned int> valid_ci_profiles;   // Valid profiles for the compute instances
    int num_slices;                  // Total number of slices
    TreeNode root;                  // Root of the GPU repartition tree
};

// Declaration of the global GPU configuration
extern const GPUConfig* global_GPU_config;

// Procedure to initialize the GPU configuration
void initialize_GPU_config(const string& gpu_name);

#endif // CONFIG_H
