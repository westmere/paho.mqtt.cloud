cmake_minimum_required(VERSION 2.8)

if(NOT PAHO_MQTT_SDK)
set(PAHO_MQTT_SDK ${CMAKE_CURRENT_LIST_DIR}/paho.mqtt.embedded-c)
endif()

if(NOT PAHO_MQTT_PLATFORM_DEFINITIONS OR NOT PAHO_MQTT_PLATFORM_SOURCES)
set(PAHO_MQTT_PLATFORM_DEFINITIONS MQTT_TASK MQTT_LWIP MQTTCLIENT_PLATFORM_HEADER=${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/MQTTFreeRTOS.h)
set(PAHO_MQTT_PLATFORM_SOURCES  ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/MQTTFreeRTOS.c)
endif()

set(PAHO_MQTT_PACKET_CLIENT_SOURCE  ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTFormat.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTPacket.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTSerializePublish.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTDeserializePublish.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTConnectClient.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTSubscribeClient.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTUnsubscribeClient.c)
                                    
set(PAHO_MQTT_PACKET_SERVER_SOURCE  ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTFormat.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTPacket.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTSerializePublish.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTDeserializePublish.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTConnectServer.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTSubscribeServer.c
                                    ${PAHO_MQTT_SDK}/MQTTPacket/src/MQTTUnsubscribeServer.c)

add_library(paho-embed-mqtt3c OBJECT ${SOURCES})
# install(TARGETS paho-embed-mqtt3c DESTINATION /usr/lib)
target_compile_definitions(paho-embed-mqtt3c PRIVATE MQTT_SERVER MQTT_CLIENT PAHO_MQTT_PLATFORM_DEFINITIONS)

add_library(MQTTPacketClient OBJECT ${PAHO_MQTT_PACKET_CLIENT_SOURCE})
target_compile_definitions(MQTTPacketClient PRIVATE MQTT_CLIENT PAHO_MQTT_PLATFORM_DEFINITIONS)

add_library(MQTTPacketServer OBJECT ${PAHO_MQTT_PACKET_SERVER_SOURCE})
target_compile_definitions(MQTTPacketServer PRIVATE MQTT_SERVER PAHO_MQTT_PLATFORM_DEFINITIONS)

add_library(MQTTClient OBJECT   ${PAHO_MQTT_PACKET_CLIENT_SOURCE}
                                ${PAHO_MQTT_SDK}/MQTTClient-C/src/MQTTClient.c)
target_compile_definitions(MQTTClient PRIVATE MQTT_CLIENT PAHO_MQTT_PLATFORM_DEFINITIONS)

include_directories(${PAHO_MQTT_SDK}/MQTTPacket/src
                    ${PAHO_MQTT_SDK}/MQTTClient-C/src)
