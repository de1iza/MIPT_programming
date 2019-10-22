cmake_minimum_required(VERSION 3.15)
project(cpu)

set(CMAKE_CXX_STANDARD 14)

set(Sources asm.cpp cpu.cpp disasm.cpp)
set(Headers enum.h commands.h ../stack/stack.h)

#add_executable(${Sources} ${Headers})
add_executable(cpu disasm.cpp)