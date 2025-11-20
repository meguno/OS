cmake_minimum_required(VERSION 3.10)
project(lab1_OS)

set(CMAKE_CXX_STANDARD 98)

add_executable(lab1_OS lab1_OS.cpp employee.h)
add_executable(Creator Creator.cpp employee.h)
add_executable(Reporter Reporter.cpp employee.h)
