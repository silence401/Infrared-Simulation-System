cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(testm)
set(CMAKE_CXX_STANDARD 14)
set(Torch_DIR "/home/simon/下载/software/libtorchcuda101/share/cmake/Torch")
find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)

message(STATUS "Pytorch status:")
message(STATUS "    libraries: ${TORCH_LIBRARIES}")


message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

add_executable(testm test.cc dataset.cc)
target_link_libraries(testm ${TORCH_LIBRARIES} ${OpenCV_LIBS})
