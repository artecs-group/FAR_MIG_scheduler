#include "scheduler.h"
#include "GPU_info.h"
#include "logging.h"
#include <vector>
#include <cfloat>
#include <algorithm>
#include <queue>
#include <functional>
#include <thread>

// Create tree structure for the algorithm
static shared_ptr<TreeNode> create_repartition_tree();
// Phase 1 of FAR's algorithm
static vector<Allocation> get_allocations_family(vector<Task> & tasks);
// Phase 2 of FAR's algorithm
static shared_ptr<TreeNode> repartitioning_schedule(Allocation const& allocation);

TreeNode::TreeNode(int start, int size, weak_ptr<TreeNode> parent) : start(start), size(size), parent(parent) {
    tasks = {};
    end = 0;
}

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


TreeNode FAR_schedule_tasks(vector<Task> & tasks){
    vector<Allocation> allocations = get_allocations_family(tasks);
    double min_makespan = DBL_MAX;
    shared_ptr<TreeNode> best_tree;
    for (auto const& allocation: allocations){
        shared_ptr<TreeNode> tree = repartitioning_schedule(allocation);
        (*tree).show_tree();
        double makespan = tree->get_makespan();
        cout << "Tree makespan: " << makespan << 's' << endl;
        if (makespan < min_makespan){
            min_makespan = makespan;
            best_tree = tree;
        }
    }
    return *best_tree;
}

static vector<Allocation> get_allocations_family(vector<Task> & tasks){
    vector<Allocation> allocations;
    
    // Get empty first allocation
    Allocation first_allocation;
    for (int size: global_GPU_info->valid_instance_sizes){
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
        if (instance_size == global_GPU_info->num_slices) break;

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


static shared_ptr<TreeNode> repartitioning_schedule(Allocation const& allocation){
    // Create a map with the tasks decreasingly ordered by time
    unordered_map<unsigned int, vector<Task*>> tasks_by_size;
    for (auto const& [size, tasks]: allocation){
        vector<Task*> tasks_vector(tasks.begin(), tasks.end());
        sort(tasks_vector.begin(), tasks_vector.end(), [size](Task* a, Task* b){
            return a->exec_times.at(size) < b->exec_times.at(size);
        });
        tasks_by_size[size] = move(tasks_vector);
    }
    // Get the root of the repartitioning tree
    shared_ptr<TreeNode> root = create_repartition_tree();

    // Min-heap of TreeNode pointers opened
    auto compare_nodes = [](shared_ptr<TreeNode> a, shared_ptr<TreeNode> b){
        return a->end > b->end;
    };
    priority_queue<shared_ptr<TreeNode>, vector<shared_ptr<TreeNode>>, decltype(compare_nodes)> heap(compare_nodes);

    // Put the root in the heap
    heap.push(root);

    // For sequential reconfiguration
    double reconfig_end = 0;

    // While there are nodes opened in the heap
    while(!heap.empty()){
        // Get the node with the smallest end time
        auto node = heap.top();
        heap.pop();

        // If there are unscheduled task assigned to the node instance size
        if(tasks_by_size.count(node->size) && !tasks_by_size[node->size].empty()){

            // Get the task with the longest execution time
            Task* task = tasks_by_size[node->size].back();
            tasks_by_size[node->size].pop_back();
            if(tasks_by_size[node->size].empty()){
                tasks_by_size.erase(node->size);
            }

            // If it's the first task, create the instance
            if(node->tasks.empty()){
                reconfig_end = max(reconfig_end, node->end);
                reconfig_end += global_GPU_info->times_create[node->size];
                node->end = reconfig_end;
            }

            // Assign the task to the node
            node->tasks.push_back(task);
            node->end += task->exec_times.at(node->size);

            // Return the node to the heap
            heap.push(node);
        }
        // If there are unscheduled tasks
        else if (!tasks_by_size.empty()){
            // Give time to destroy
            if (!node->tasks.empty()){
                reconfig_end = max(reconfig_end, node->end);
                reconfig_end += global_GPU_info->times_destroy[node->size];
            }
            // Create the children instances
            for (auto child: node->children){
                child->end = node->end;
                heap.push(child);
            }
        }
    }

    return root;
}



// Definition of the repartition tree for the corresponding GPU
static shared_ptr<TreeNode> create_repartition_tree(){
    if (global_GPU_info->name == "A30"){

        auto root = make_shared<TreeNode>(0, 4);

        auto node_0_2 = make_shared<TreeNode>(0, 2, root);
        auto node_2_2 = make_shared<TreeNode>(2, 2, root);

        auto node_0_1 = make_shared<TreeNode>(0, 1, node_0_2);
        auto node_1_1 = make_shared<TreeNode>(1, 1, node_0_2);

        auto node_2_1 = make_shared<TreeNode>(2, 1, node_2_2);
        auto node_3_1 = make_shared<TreeNode>(3, 1, node_2_2);

        root->children = {node_0_2, node_2_2};

        node_0_2->children = {node_0_1, node_1_1};
        node_2_2->children = {node_2_1, node_3_1};

        node_0_1->children = {};
        node_1_1->children = {};
        node_2_1->children = {};
        node_3_1->children = {};
        
        return root;
    } else if (global_GPU_info->name == "A100/H100"){
        auto root = make_shared<TreeNode>(0, 7);

        auto node_0_4 = make_shared<TreeNode>(0, 4, root);
        auto node_4_3 = make_shared<TreeNode>(4, 3, root);

        auto node_0_3 = make_shared<TreeNode>(0, 3, node_0_4);

        auto node_0_2 = make_shared<TreeNode>(0, 2, node_0_3);
        auto node_2_2 = make_shared<TreeNode>(2, 2, node_0_3);

        auto node_0_1 = make_shared<TreeNode>(0, 1, node_0_2);
        auto node_1_1 = make_shared<TreeNode>(1, 1, node_0_2);

        auto node_2_1 = make_shared<TreeNode>(2, 1, node_2_2);
        auto node_3_1 = make_shared<TreeNode>(3, 1, node_2_2);

        auto node_4_2 = make_shared<TreeNode>(4, 2, node_4_3);
        auto node_6_1 = make_shared<TreeNode>(6, 1, node_4_3);

        auto node_4_1 = make_shared<TreeNode>(4, 1, node_4_2);
        auto node_5_1 = make_shared<TreeNode>(5, 1, node_4_2);

        root->children = {node_0_4, node_4_3};

        node_0_4->children = {node_0_3};
        node_4_3->children = {node_4_2, node_6_1};

        node_0_3->children = {node_0_2, node_2_2};
        node_0_2->children = {node_0_1, node_1_1};
        node_2_2->children = {node_2_1, node_3_1};

        node_4_2->children = {node_4_1, node_5_1};
        return root;
    }
    else{
        LOG_ERROR("GPU model unknown");
        exit(1);
    }
}

void TreeNode::show_tree() const{
    cout << "======================================" << endl;
    cout << "Repartitioning tree:" << endl;

    function<void(const TreeNode &, int)> show_node = [&](const TreeNode & node, int level){
        for (int i = 0; i < level; i++){
            cout << "--";
        }
        cout << "Node(start=" << node.start << ", size=" << node.size << ", end=" << node.end << ", tasks=[";
        for (auto const& task: node.tasks){
            cout << task->name << " ";
        }
        cout << "])" << endl;
        for (auto const& child: node.children){
            show_node(*child, level + 1);
        }
    };

    show_node(*this, 0);
}

double TreeNode::get_makespan() const{
    double makespan = this->end;
    for (auto child: this->children){
        makespan = max(makespan, child->get_makespan());
    }
    return makespan;
}

void TreeNode::execute_tasks(nvmlDevice_t device) const{
    // Create the instance, execute the tasks and destroy it if there are tasks in this node
    if (!this->tasks.empty()){
        Instance instance = create_instance(device, this->start, this->size);
        for (auto const& task: this->tasks){
            task->execute(instance);
        }
        destroy_instance(instance);
    }
    // Parallel execution of tasks in the children nodes
    vector<thread> children_threads;
    for (auto const& child: this->children){
        children_threads.emplace_back([&child, device]{
            child->execute_tasks(device);
        });
    }
    // Wait for the children threads to finish
    for (auto & thread: children_threads){
        thread.join();
    }
}