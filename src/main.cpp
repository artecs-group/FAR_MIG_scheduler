#include <iostream>
#include "utils.h"
#include "GPU_config.h"
#include "MIG_manager.h"
#include "task.h"
using namespace std;

int main(int argc, char* argv[]){

   if (argc != 3){
        cerr << "Usage: " << argv[0] << " <gpu_number> <path to kernels filelist>" << endl;
        return 1;
   }
   int gpu_number = atoi(argv[1]);
   string kernels_filename = argv[2];

   // Init the compiler, bind with the device and init GPU config. for the scheduler.
   init_nvml();
   nvmlDevice_t device = bind_device(gpu_number);
   string gpu_name = get_gpu_name(device);


   initialize_GPU_config(gpu_name);

   // Validate the scripts for scheduling
   vector<Task> tasks = validate_scripts(kernels_filename);
   if (tasks.empty()){
        cerr << "Error: no valid tasks for scheduling" << endl;
        return 1;
   }

   // Enable MIG
   MIG_enable(device, gpu_number);

   Instance instance(0, 1);
   create_instance(device, instance);

   destroy_all_instances(device);

   //Disable MIG
   MIG_disable(device, gpu_number);





   return 0;
}