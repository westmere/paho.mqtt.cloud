cmake_minimum_required(VERSION 2.8)

option(MQTT_CLOUD_DEBUG "Enable MQTT cloud debug." OFF)
option(USE_OBJECT_MQTT_CLOUD_LIBRARY "Build MQTT cloud object library." OFF)
option(USE_OBJECT_MQTT_SERVER_LIBRARY "Build MQTT server object library." OFF)

if(NOT PAHO_MQTT_SDK)
    set(PAHO_MQTT_SDK ${CMAKE_CURRENT_LIST_DIR}/paho.mqtt.embedded-c)
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

# file(GLOB SOURCES "${PAHO_MQTT_SDK}/MQTTPacket/src/*.c")

# add_library(paho-embed-mqtt3c-object OBJECT ${SOURCES})
# target_compile_definitions(paho-embed-mqtt3c-object PRIVATE MQTT_SERVER MQTT_CLIENT)

if(MQTT_CLOUD_DEBUG)
    add_definitions(-DMQTT_CLOUD_DEBUG)
endif(MQTT_CLOUD_DEBUG)

if(USE_OBJECT_MQTT_SERVER_LIBRARY)
    add_library(MQTTPacketServerObject OBJECT ${PAHO_MQTT_PACKET_SERVER_SOURCE})
    target_compile_definitions(MQTTPacketServerObject PRIVATE MQTT_SERVER)
endif(USE_OBJECT_MQTT_SERVER_LIBRARY)

add_library(MQTTPacketClientObject OBJECT ${PAHO_MQTT_PACKET_CLIENT_SOURCE})
target_compile_definitions(MQTTPacketClientObject PRIVATE MQTT_CLIENT)

add_library(MQTTClientObject OBJECT   ${PAHO_MQTT_PACKET_CLIENT_SOURCE} ${PAHO_MQTT_SDK}/MQTTClient-C/src/MQTTClient.c)
target_compile_definitions(MQTTClientObject PRIVATE MQTT_CLIENT MQTTCLIENT_QOS2=1)

if(USE_OBJECT_MQTT_CLOUD_LIBRARY)
    add_library(MQTTCloudObject OBJECT   MQTTCloud/MQTTCloud.c)
endif(USE_OBJECT_MQTT_CLOUD_LIBRARY)

include_directories(${PAHO_MQTT_SDK}/MQTTPacket/src)
