cmake_minimum_required(VERSION 3.0)

project(main)

option(SIMD_ENABLED "Whether to enable SIMD optimizations" OFF)
option(OPENMP_ENABLED "Whether to enable OpenMP parallelization" ON)
option(IPO_ENABLED "Whether to enable interprocedural optimization" ON)
option(CUDA_ENABLED "Whether to enable CUDA, if available" ON)
option(OPENGL_ENABLED "Whether to enable OpenGL, if available" ON)
option(TESTS_ENABLED "Whether to build test binaries" OFF)
option(PROFILING_ENABLED "Whether to enable google-perftools linker flags" OFF)
option(CGAL_ENABLED "Whether to enable the CGAL library" ON)
option(BOOST_STATIC "Whether to enable static boost library linker flags" ON)
set(CUDA_ARCHS "Auto" CACHE STRING "List of CUDA architectures for which to \
generate code, e.g., Auto, All, Maxwell, Pascal, ...")

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeHelper.cmake NO_POLICY_SCOPE)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Qt5 5.4 REQUIRED COMPONENTS Core OpenGL Widgets)
find_package(Eigen3)
set(CUDA_MIN_VERSION "7.0")
find_package(CUDA ${CUDA_MIN_VERSION} QUIET)
if(CUDA_FOUND)
    message("CUDA FOUND")
    add_definitions("-DCUDA_ENABLED")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SelectCudaComputeArch.cmake)

    set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS}" "${CUDA_ARCH_FLAGS}")

    set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -D_FORCE_INLINES")

    set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -Wno-deprecated-gpu-targets")

    message(STATUS "Enabling CUDA support (version: ${CUDA_VERSION_STRING},"
    " archs: ${CUDA_ARCH_FLAGS_readable})")
endif()
if(BOOST_STATIC)
    set(Boost_USE_STATIC_LIBS ON)
else()
    add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

if(Qt5_POSITION_INDEPENDENT_CODE)
   # message("Qt5_POSITION_INDEPENDENT_CODE")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    if(CUDA_ENABLED AND NOT IS_MSVC)
        #message("IS_MSVC============================")
        set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} --compiler-options -fPIC")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif()
endif()

if(OPENMP_ENABLED AND OPENMP_FOUND)
    message(STATUS "Enabling OpenMP support")
    add_definitions("-DOPENMP_ENABLED")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(STATUS "Disabling OpenMP support")
endif()


find_package(OpenGL REQUIRED)
find_package(Glew REQUIRED)
find_package(Git)
find_package(Glog REQUIRED)
find_package(FreeImage REQUIRED)

find_package(CGAL QUIET)
find_package(Ceres REQUIRED)

find_package(Boost REQUIRED COMPONENTS
             program_options
             filesystem
             graph
             system
             unit_test_framework)

#find_package(G2O REQUIRED)

#message(${EIGEN3_INCLUDE_DIRS})
include_directories(
    ${Qt5OpenGL_INCLUDE_DIRS} 
    ${Qt5Widgets_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${CUDA_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${FREEIMAGE_INCLUDE_DIRS}
    ${CERES_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${HEADER_FILES}
    src
    lib
)
link_directories(${Boost_LIBRARY_DIRS})
# link_libraries( 
#     ${Qt5OpenGL_LIBRARIES} 
#     ${Qt5Widgets_LIBRARIES}
# )


# FILE(GLOB SOURCES_FILES 
# "./src/*.cc"
# "./src/ui/*.cc"
# "src/util/option_manager.cc"
# "src/util/misc.cc"
# "src/util/*.cc"
# "src/mvs/*.cc"
# "src/feature/*.cc"
# "src/sfm/*.cc"
# "src/base/*.cc"
# "src/controllers/*.cc"
# "src/estimators/*.cc"
# "src/optim/*.cc"
# "src/retrieval/*.cc")
FILE(GLOB SOURCES_FILES "./src/*.cc")
# FILE(GLOB HEADER_FILES "./src*.h"
#                         "./src/ui/*.h"
#                         "src/util/option_manager.h"
#                         "src/util/misc.h"
#                         "src/util/*.h"
#                         "src/mvs/*.h"
#                         "src/feature/*.h"
                        
#                         "src/sfm/*.h"
#                         "src/base/*.h"
#                         "src/controllers/*.h"
#                         "src/estimators/*.h"
#                         "src/optim/*.h"
#                         "src/retrieval/*.h")

# FILE(GLOB LIB_FLANN_SRCS "lib/FLANN/*.h"
#                          "lib/FLANN/*.cpp"
#                          "lib/FLANN/*.hpp"
#                          "lib/FLANN/*.cu")

# FILE(GLOB LIB_GRACLUS_SRCS "lib/Graclus/*.h"
#                            "lib/Graclus/*.c")

# FILE(GLOB LIB_LSD_SRCS "lib/LSD/*.h"
#                         "lib/LSD/*.c")

# FILE(GLOB LIB_PBA_SRCS "lib/PBA/*.h"
#                         "lib/PBA/*.cpp"
#                         "lib/PBA/*.cu")

# FILE(GLOB LIB_POISSON_RECON_SRCS "lib/PoissonRecon/*.h"
#                                  "lib/PoissonRecon/*.cpp"
#                                  "lib/PoissonRecon/*.inl")

# FILE(GLOB LIB_SIFT_GPU_SRCS "lib/SiftGPU/*.h"
#                             "lib/SiftGPU/*.cpp"
#                             "lib/SiftGPU/*.cu")

# FILE(GLOB LIB_SQLITE_SRCS "lib/SQLite/*.h"
#                           "lib/SQLite/*.c")

# FILE(GLOB LIB_VLFEAT_SRCS "lib/VLFeat/*.h"
#                           "lib/VLFeat/*.c"
#                           "lib/VLFeat/*.tc")
# message(${LIB_FLANN_SRCS})

# COLMAP_ADD_SOURCE_DIR(lib/FLANN LIB_FLANN_SRCS *.h *.cpp *.hpp *.cu)
# COLMAP_ADD_SOURCE_DIR(lib/Graclus LIB_GRACLUS_SRCS *.h *.c)
# COLMAP_ADD_SOURCE_DIR(lib/LSD LIB_LSD_SRCS *.h *.c)
# COLMAP_ADD_SOURCE_DIR(lib/PBA LIB_PBA_SRCS *.h *.cpp *.cu)
# COLMAP_ADD_SOURCE_DIR(lib/PoissonRecon LIB_POISSON_RECON_SRCS *.h *.cpp *.inl)
# COLMAP_ADD_SOURCE_DIR(lib/SiftGPU LIB_SIFT_GPU_SRCS *.h *.cpp *.cu)
# COLMAP_ADD_SOURCE_DIR(lib/SQLite LIB_SQLITE_SRCS *.h *.c)
# COLMAP_ADD_SOURCE_DIR(lib/VLFeat LIB_VLFEAT_SRCS *.h *.c *.tc)

COLMAP_ADD_SOURCE_DIR(src/base BASE_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/controllers CONTROLLERS_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/estimators ESTIMATORS_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/exe EXE_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/feature FEATURE_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/mvs MVS_SRCS *.h *.cc *.cu)
COLMAP_ADD_SOURCE_DIR(src/optim OPTIM_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/retrieval RETRIEVAL_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/sfm SFM_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/tools TOOLS_SRCS *.h *.cc)
COLMAP_ADD_SOURCE_DIR(src/ui UI_SRCS *.h *.cc *.qrc)
COLMAP_ADD_SOURCE_DIR(src/util UTIL_SRCS *.h *.cc)



# add_library(
#             INFRARED_INTERNAL_LIBRARIES
#             ${LIB_FLANN_SRCS}
#             ${LIB_GRACLUS_SRCS}
#             ${LIB_LSD_SRCS}
#             ${LIB_PBA_SRCS}
#             ${LIB_POISSON_RECON_SRCS}
#             ${LIB_SIFT_GPU_SRCS}
#             ${LIB_SQLITE_SRCS}
#             ${LIB_VLFEAT_SRCS})

#aux_source_directory(./src/ui CPP_LIST)
#message(${CPP_LIST})
#message(${SOURCES_FILES})
add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)
add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED)
# aux_source_directory(./src/util/ CPP_LIST2)
# aux_source_directory(./src CPP_LIST1)
message("*************************")
set(COLMAP_EXTERNAL_LIBRARIES
    pthread
    ${GLOG_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES} 
    ${Qt5Widgets_LIBRARIES} 
    ${OPENGL_LIBRARIES}
    ${FREEIMAGE_LIBRARIES}
    ${CERES_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CGAL_LIBRARY}
    ${GMP_LIBRARIES}
    ${OpenMP_libomp_LIBRARY}
    )

set(COLMAP_INTERNAL_LIBRARIES
    flann
    graclus
    lsd
    pba
    poisson_recon
    sqlite3
    sift_gpu
    vlfeat
)
add_subdirectory(lib)
add_subdirectory(src)
#qt资源文件
qt5_add_resources(QRC_FILE ./src/resources.qrc)
add_executable(
                main
                ./src/main.cc
                ${QRC_FILE}
                #${SOURCES_FILES}
               # ${HEADER_FILES}
                )  
#target_link_libraries(main ${INFRARED_EXTERNAL_LIBRARIES} ${INFRARED_INTERNAL_LIBRARIES})   
target_compile_options(main PRIVATE -fPIC)
target_link_libraries(main colmap)      
