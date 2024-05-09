cmake_minimum_required(VERSION 3.10)
project(RoboCatServer)

set(CMAKE_CXX_STANDARD 14)

#载入RoboCat文件夹下的头文件
include_directories("RoboCat/inc")
include_directories("RoboServer/inc")

file(GLOB_RECURSE SRC_FILES RoboCat/src/*.cpp) #获取RoboCat文件夹下的所有.cpp文件
file(GLOB_RECURSE SRC_FILES_SERVER RoboServer/src/*.cpp) #获取RoboCatServer文件夹下的所有.cpp文件

#设置-wall参数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")#-Wall参数表示显示所有警告

#与RoboCatServer文件下的ServerMain.cpp文件链接
add_executable(RoboCatServer ${SRC_FILES_SERVER} ${SRC_FILES})
