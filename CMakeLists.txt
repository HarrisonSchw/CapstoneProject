cmake_minimum_required(VERSION 3.10)
project(MyProject)

find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(MyProject main.cpp)
target_link_libraries(MyProject ${OpenCV_LIBS})