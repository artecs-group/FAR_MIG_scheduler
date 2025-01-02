#include <iostream>
#include "utils.h"
#include "MIG_manager.h"
#include "task.h"
using namespace std;

int main(int argc, char* argv[]){

   if (argc != 3){
        cerr << "Usage: " << argv[0] << " <gpu_name> <path to kernels filelist>" << endl;
        return 1;
   }
   string gpu_name = argv[1];
   string kernels_filename = argv[2];

   int gpu_number;
   nvmlDevice_t device;

   // If the GPU is available, we init the compiler and bind with the device.
   if ((gpu_number = get_gpu_number(gpu_name)) != -1){
      cout << "GPU " << gpu_name << " has been found in number " << gpu_number << endl;
      init_nvml();
      device = bind_device(gpu_number);
   // Otherwise, we show the available GPUs.
   } else {
      cerr << "GPU " << gpu_name << " is not available" << endl;
      cout << "Available GPU names:" << endl;
      show_available_gpus();
      return 1;
   }

   // Validate the scripts for scheduling
   vector<Task> tasks = validate_scripts(kernels_filename);
   if (tasks.empty()){
        cerr << "Error: no valid tasks for scheduling" << endl;
        return 1;
   }





   return 0;
}