#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <unordered_map>
#include <vector>
#include "tasks.h"
using namespace std;

typedef unordered_map<unsigned int, vector<Task*>> Allocation;

// Phase 1 of FAR's algorithm
vector<Allocation> get_allocations_family(vector<Task> const& tasks);

#endif // SCHEDULER_H