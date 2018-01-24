/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(MQTT_CLOUD_H)
#define MQTT_CLOUD_H

#ifndef MQTT_CLOUD_SENDBUF_SIZE
#define MQTT_CLOUD_SENDBUF_SIZE (128)
#endif
#ifndef MQTT_CLOUD_READBUF_SIZE
#define MQTT_CLOUD_READBUF_SIZE (128)
#endif
#ifndef MQTT_CLOUD_HOSTNAME_SIZE
#define MQTT_CLOUD_HOSTNAME_SIZE (64)
#endif

struct MQTTCLoudSubscribe
{
	const char* topicFilter;
	enum QoS qos;
	messageHandler messageHandler;
};

struct MQTTCLoudConfig
{
    const struct MQTTCLoudSubscribe *topics;
    const MQTTPacket_connectData *options;
};

struct MQTTCLoud
{
    const struct MQTTCLoudConfig *config;
    MQTTClient *client;
    Network *network;
    unsigned char sendbuf[128];
    unsigned char readbuf[128];
    char host[MQTT_CLOUD_HOSTNAME_SIZE];
    unsigned int port;
    unsigned int command_timeout_ms;
};

int MQTTCLoudClientInit(struct MQTTCLoud *c, MQTTClient* client, Network* network,
                        const char *host, unsigned int port,
                        unsigned int command_timeout_ms,
                        const struct MQTTCLoudConfig *config);
int MQTTCLoudClientPoll(struct MQTTCLoud *c);

#endif
