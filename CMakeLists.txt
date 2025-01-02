cmake_minimum_required(VERSION 3.10)
project(MIGScheduler)

find_package(CUDA REQUIRED)

set(CMAKE_CXX_STANDARD 17)

# Add the header files
include_directories(include)

set(CUDA_INCLUDE_PATH "/usr/local/cuda-12.3/include")
include_directories(${CUDA_INCLUDE_PATH})


set(NVML_LIB_PATH "/usr/local/cuda-12.3/targets/x86_64-linux/lib/stubs")
link_directories(${NVML_LIB_PATH})

# Add the source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(mig_scheduler.exe ${SOURCES})

target_link_libraries(mig_scheduler.exe ${CUDA_LIBRARIES} nvidia-ml)

