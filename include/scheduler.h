#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "tasks.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;

typedef unordered_map<unsigned int, unordered_set<Task*>> Allocation;

// Repartition tree structure
struct TreeNode{
    int start, size; // Slice of start and size of the instance
    vector<Task*> tasks;// Tasks to execute in this node  
    vector<shared_ptr<TreeNode>> children; // Children of this node
    double end; // End time of the node execution
    weak_ptr<TreeNode> parent; // Parent of this node

    TreeNode(int start, int size, weak_ptr<TreeNode> parent = weak_ptr<TreeNode>());

    void show_tree(); // Show the complete current tree in detail
};

// Phase 1 of FAR's algorithm
vector<Allocation> get_allocations_family(vector<Task> & tasks);

// Phase 2 of FAR's algorithm
shared_ptr<TreeNode> repartitioning_schedule(Allocation const& allocation);

#endif // SCHEDULER_H