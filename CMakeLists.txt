# a cmake list file
cmake_minimum_required(VERSION 3.5)
# project name
# set(CMAKE_C_COMPILER "C:/Files/w64devkit/bin/gcc.exe")
# set(CMAKE_CXX_COMPILER "C:/Files/w64devkit/bin/g++.exe")
project(JSONCPP)

# 定义变量
SET(TEST test.cpp)

SET(MAIN main.cpp)

# 指定cpp版本
SET(CMAKE_CXX_STANDARD 17)
# 指定存储路径
SET(Home "D:/study/jsoncpp")
SET(EXECUTABLE_OUTPUT_PATH ${Home})
# 包含头文件
# include_directories("${PROJECT_SOURCE_DIR}/jsoncpp.h")
# 搜索当前目录的src目录下所有的源文件，并存储到变量中
# file(GLOB MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# file(GLOB MAIN_HEAD "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# 搜索路径另一种方式
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)
include_directories(${PROJECT_SOURCE_DIR}/include)

# add_executable(main ${MAIN} ${SRC_LIST})

add_executable(test ${TEST} ${SRC_LIST})
