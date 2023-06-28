cmake_minimum_required(VERSION 3.25)
project(String)

set(CMAKE_CXX_STANDARD 17)

add_executable(String main.cpp String.h BufferManager.h Testable.h TestString.h)
