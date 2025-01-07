#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <unordered_map>
#include <unordered_set>
#include "tasks.h"
using namespace std;

typedef unordered_map<unsigned int, unordered_set<Task*>> Allocation;

// Repartition tree structure
struct TreeNode{
    int start, size; // Slice of start and size of the instance
    vector<Task*> tasks;// Tasks to execute in this node  
    vector<TreeNode*> children; // Children of this node
    double end; // End time of the node execution
    TreeNode* parent; // Parent of this node

    TreeNode(int start, int size, TreeNode* parent) : start(start), size(size), children(children), parent(parent) {
        tasks = {};
        end = 0;
    }

    ~TreeNode() { // Destructor to delete the children
        for (TreeNode* child: children){
            if(child != nullptr) delete child;
        }
    }
    
};

// Phase 1 of FAR's algorithm
vector<Allocation> get_allocations_family(vector<Task> & tasks);



#endif // SCHEDULER_H