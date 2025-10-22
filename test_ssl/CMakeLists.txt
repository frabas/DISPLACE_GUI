cmake_minimum_required(VERSION 3.16)
project(mytest LANGUAGES CXX)

set(Qt6_DIR "C:/Qt6/6.8.3/msvc2022_64/lib/cmake/Qt6")

find_package(Qt6 6.8.3 REQUIRED COMPONENTS Core Network Widgets)

add_executable(mytest main.cpp)
target_link_libraries(mytest PRIVATE Qt6::Core Qt6::Network Qt6::Widgets)

