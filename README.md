# FAR scheduler for NVIDIA Multi-Instance GPU (MIG)
This repository contains a C++ implementation of the FAR task scheduler, targeted for NVIDIA GPUs that support physical partitioning via Multi-Instance GPU (MIG). This scheduler is useful for reducing the joint execution time of tasks (makespan) by cleverly co-executing them with MIG. The FAR algorithm used by the scheduler is presented and accompanied by a comprehensive evaluation in [this paper](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4958466) (preprint for the moment). 

## Supported GPUs
Currently, the software explicitly supports the following NVIDIA GPU models:  
- **NVIDIA A30**  
- **NVIDIA A100**  
- **NVIDIA H100**
  
Moreover, it probably works correctly for models with the same MIG characteristics as some of the above. In particular, it will probably work for models NVIDIA B100 and B200 that have the same MIG partitioning as A100 and H100, although it is not tested for them.

For the A100 and H100 models, the [MIG API of the NVML library](https://docs.nvidia.com/deploy/nvml-api/group__nvmlMultiInstanceGPU.html) currently has some bugs in the handling of some instances, and the scheduler may not work correctly. With a future version of the library that fixes these problems the scheduler should work perfectly for these GPUs.

## Requirements
To use this software, ensure the following prerequisites are met:

#### Hardware
- An NVIDIA GPU with MIG technology (see previous section for supported models).

#### Software
- **Operating System**: Linux recommended. NVML is available on Windows, but MIG management is limited or unsupported in some cases. Use on Windows is experimental and not guaranteed.
- **NVIDIA Driver**: Version 470 or later, with MIG support enabled.
- **CUDA Toolkit**: Version 11.0 or newer.
- **NVIDIA Management Library (NVML)**: Comes bundled with the NVIDIA driver; ensure it is correctly installed (usually in `/usr/include/` or `/usr/local/cuda/include`).  

#### Build Dependencies
- **CMake**:  
  - Version 3.10 or newer to configure and build the project.
- **C++ Compiler**:  
  - g++ version at least 5.1 to robustly support C++11.  

#### Permissions
- Ensure you have administrative privileges to configure MIG instances on the GPU.

## Installation
To install and build the project, follow these steps:
#### 1. Clone the repository
First, clone this repository to your local machine using the following command:
```bash
git clone https://github.com/Jorgitou98/FAR_MIG_scheduler.git
cd FAR_MIG_scheduler
```
##### 2. Update the CUDA root directory
Edit `/FAR_MIG_scheduler/CMakeLists.txt` changing the CUDA_ROOT path to point to your CUDA installation directory.
```
set(CUDA_ROOT "/usr/local/cuda" CACHE PATH "CUDA Toolkit root directory")
```
#### 3. Generate build files with CMake and compile the project
```bash
cd build
cmake ..
```
#### 4. Compile the project
This will create the executable file ``mig_scheduler.exe``.
```bash
make
```
## Usage

## Publications
The paper presenting this scheduler is currently under review. For the moment you can access the [preprint](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4958466).
## Acknowledgements
This work is funded by Grant PID2021-126576NB-I00 funded by MCIN/AEI/10.13039/501100011033 and by _"ERDF A way of making Europe"_. 
