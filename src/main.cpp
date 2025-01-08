#include "GPU_info.h"
#include "MIG_manager.h"
#include "utils.h"
#include "logging.h"
#include "scheduler.h"
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


     initialize_GPU_info(gpu_name);

     // Validate the scripts for scheduling
     vector<Task> tasks = get_tasks(kernels_filename);
     if (tasks.empty()){
          LOG_ERROR("No valid tasks for scheduling. Problem parsing the file " + kernels_filename);
          return 1;
     }

     // Enable MIG
     MIG_enable(device, gpu_number);

     // Destroy all instances to start from scratch
     destroy_all_instances(device);
     
     // Profile instance creation and destruction times
     profile_reconfig_times(device);

     // Profile tasks to get their execution times for each instance size
     // profile_tasks(tasks, device);
     for(auto & task: tasks){
          task.exec_times = {
               {1, 1},
               {2, 2},
               {4, 4}
          };
     }

     // Get the allocations family
     vector<Allocation> alloc_family = get_allocations_family(tasks);
     cout << "Allocations family: " << endl;
     for (auto const& alloc: alloc_family){
          for (auto const& [size, tasks]: alloc){
               cout << "Size: " << size << " Tasks: ";
               for (auto const& task: tasks){
                    cout << task->name << " ";
               }
               cout << endl;
          }
     }
     
     for (auto const& alloc: alloc_family){
          shared_ptr<TreeNode> tree = repartitioning_schedule(alloc);
          cout << "============================" << endl;
          (*tree).show_tree();
          cout << "============================" << endl;
     }

     //Disable MIG
     MIG_disable(device, gpu_number);

     return 0;
}