cmake_minimum_required(VERSION 3.22.1)
project(rclmodoki)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

