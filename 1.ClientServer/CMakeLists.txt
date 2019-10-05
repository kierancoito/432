cmake_minimum_required(VERSION 3.13)
project(1_ClientServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(1_ClientServer
        Server.cpp Client.cpp)