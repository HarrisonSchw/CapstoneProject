cmake_minimum_required(VERSION 3.10)
project(Capstone)

find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Capstone main.cpp)
target_link_libraries(Capstone ${OpenCV_LIBS})