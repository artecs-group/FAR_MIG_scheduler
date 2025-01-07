
#include "GPU_config.h"
#include <nvml.h>
#include <iostream>

using namespace std;

static TreeNode create_repartition_tree(string gpu_name);

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

    4,            // Total number of slices
    create_repartition_tree("A30") // Root of the GPU repartition tree
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

    7,                 // Total number of slices
    create_repartition_tree("A100/H100") // Root of the GPU repartition tree
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

static TreeNode create_repartition_tree(string gpu_name){
    if (gpu_name == "A30"){
        TreeNode root(0, 4, nullptr);
        TreeNode* node_0_2 = new TreeNode(0, 2, &root);
        TreeNode* node_2_2 = new TreeNode(2, 2, &root);
        TreeNode* node_0_1 = new TreeNode(0, 1, node_0_2);
        TreeNode* node_1_1 = new TreeNode(1, 1, node_0_2);
        TreeNode* node_2_1 = new TreeNode(2, 1, node_2_2);
        TreeNode* node_3_1 = new TreeNode(3, 1, node_2_2);
        root.children = {node_0_2, node_2_2};
        node_0_2->children = {node_0_1, node_1_1};
        node_2_2->children = {node_2_1, node_3_1};
        node_0_1->children = {};
        node_1_1->children = {};
        node_2_1->children = {};
        node_3_1->children = {};
        return root;
    } else if (gpu_name == "A100/H100"){
        TreeNode root(0, 7, nullptr);
        return root;
    }
    else {
        cerr << "GPU model unknown: " + gpu_name << endl;
        exit(1);
    }
}