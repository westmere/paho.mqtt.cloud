cmake_minimum_required(VERSION 2.8)

add_definitions(MQTTCLIENT_PLATFORM_HEADER=${CMAKE_CURRENT_LIST_DIR}/MQTTFreeRTOS.h)
add_library(paho-mqtt-freertos OBJECT   Mutex.c
                                        Timer.c
                                        lwip/Network.c)
