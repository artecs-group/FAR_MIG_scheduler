#include "scheduler.h"
#include "GPU_config.h"
#include <vector>
#include <cfloat>

static int min_area_size(Task const& task, int min_instance_size = -1){
    int min_size = -1;
    double min_area = DBL_MAX;
    for (auto const& [size, time]: task.exec_times){
        if (size <= min_instance_size) continue;
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

static void search_longest_task(Allocation const& allocation, Task* & longest_task, int & instance_size){
    double longest_time = 0;
    for (auto const& [size, tasks]: allocation){
        for (auto const& task: tasks){
            if (task->exec_times.at(size) > longest_time){
                longest_time = task->exec_times.at(size);
                longest_task = task;
                instance_size = size;
            }
        }
    }
} 

vector<Allocation> get_allocations_family(vector<Task> & tasks){
    vector<Allocation> allocations;
    
    // Get empty first allocation
    Allocation first_allocation;
    for (int size: global_GPU_config->valid_instance_sizes){
        first_allocation[size] = {};
    }
    // Put each task with its best size in the first allocation
    for (auto & task: tasks){
        int best_size = min_area_size(task);
        first_allocation[best_size].insert(&task);
    }
    allocations.push_back(first_allocation);

    while(true){
        // Copy the last allocation and search the longest task
        Allocation next_allocation = copy(allocations.back());
        Task* longest_task;
        int instance_size;
        search_longest_task(next_allocation, longest_task, instance_size);

        // If instance size for the longest task is all the GPU, the family is complete (break)
        if (instance_size == global_GPU_config->num_slices) break;

        // Remove the longest task from the allocation
        next_allocation[instance_size].erase(longest_task);

        // Put the longest task in the next best size
        int next_best_size = min_area_size(*longest_task, instance_size);
        next_allocation[next_best_size].insert(longest_task);

        // Add the new allocation to the family
        allocations.push_back(next_allocation);
    }

    return allocations;
}

