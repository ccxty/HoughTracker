cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(HoughTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++14 standard to use")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG -g)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(ROOT 6.16...6.28 CONFIG REQUIRED)
#find_package(benchmark REQUIRED)

include(${ROOT_USE_FILE})
include_directories(include)

separate_arguments(ROOT_EXE_LINKER_FLAGS)
add_executable(HoughTracker src/main3.C)
target_link_libraries(HoughTracker PUBLIC ${ROOT_LIBRARIES}
                                          ${ROOT_EXE_LINKER_FLAGS}
#                                          benchmark::benchmark
                                          )