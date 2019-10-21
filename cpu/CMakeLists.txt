cmake_minimum_required(VERSION 3.15)
project(cpu)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpu cpu.cpp ../stack/stack.h)