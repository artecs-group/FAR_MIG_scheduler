#ifndef MIG_MANAGER_H
#define MIG_MANAGER_H

#include <string>
#include <nvml.h>

using namespace std;

int get_gpu_number(const string & gpu_name);
void show_available_gpus();
void init_nvml();
nvmlDevice_t bind_device(int gpu_number);

#endif