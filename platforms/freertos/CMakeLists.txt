cmake_minimum_required(VERSION 2.8)

add_library(paho-mqtt-freertos-object OBJECT Thread.c Timer.c lwip/Network.c)
target_include_directories(paho-mqtt-freertos-object PRIVATE ${CMAKE_CURRENT_LIST_DIR})
