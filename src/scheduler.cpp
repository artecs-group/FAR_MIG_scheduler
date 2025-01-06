#include "scheduler.h"
#include "GPU_config.h"
#include <cfloat>

static Allocation get_empty_allocation(){
    Allocation allocation = unordered_map<unsigned int, vector<Task*>>();
    for (int size: global_GPU_config->valid_instance_sizes){
        allocation[size] = {};
    }
    return allocation;
}

static int min_area_size(Task const& task){
    int min_size = -1;
    double min_area = DBL_MAX;
    for (auto const& [size, time]: task.exec_times){
        if (size * time < min_area){
            min_area = size * time;
            min_size = size;
        }
    }
    return min_size;
}

static Allocation copy(Allocation const& alloc){
    Allocation alloc_copy;
    for (auto const& [size, tasks]: alloc){
        alloc_copy[size] = tasks;
    }
    return alloc_copy;
}

vector<Allocation> get_allocations_family(vector<Task> & tasks){
    vector<Allocation> allocations;
    Allocation first_allocation = get_empty_allocation();
    for (auto & task: tasks){
        int best_size = min_area_size(task);
        first_allocation[best_size].push_back(&task);
    }
    allocations.push_back(first_allocation);
    Allocation next_allocation = copy(first_allocation);
    // TO DO: Implement the rest of the code
    return allocations;
}