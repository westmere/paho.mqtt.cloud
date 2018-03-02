#include "MQTTClient.h"
#include "MQTTCloud.h"

#ifdef MQTT_CLOUD_DEBUG
#define MQTT_PRINTF printf
#else
#define MQTT_PRINTF(fmt,args...)
#endif

int MQTTCLoudClientInit(struct MQTTCLoud *c, MQTTClient *client, Network *network,
                        const char *host, unsigned int port,
                        unsigned int command_timeout_ms,
                        const struct MQTTCLoudConfig *config)
{
	int n, rc=0;

	n=strlen(host);
	if(n > (MQTT_CLOUD_HOSTNAME_SIZE-1))
		return -1;
	memcpy(c->host, host, n);
	c->host[n]=0;
	c->port= port;

	c->command_timeout_ms = command_timeout_ms;

	c->config = config;
	MQTTClientInit(client, network, command_timeout_ms, c->sendbuf, MQTT_CLOUD_SENDBUF_SIZE, c->readbuf, MQTT_CLOUD_READBUF_SIZE);

	c->client = client;
	c->network = network;
#ifdef MQTT_TASK
    rc = MQTTStartTask(c->client)
    MQTT_PRINTF("Return code from start tasks is %d\n", rc);
#endif
    return rc;
}

static int MQTTCLoudClientConnect(struct MQTTCLoud *c)
{
	int rc;

	if(!c->network->isconnected(c->network))
	{
		rc = NetworkConnect(c->network, c->host, c->port);
		MQTT_PRINTF("Return code from network connect: %s:%d is %d\n", c->host, c->port, rc);
		if(rc)
		{
//			c->network->disconnect(c->network);
			return rc;
		}
	}

	return MQTTConnect(c->client, (MQTTPacket_connectData*)c->config->options);
}

static int MQTTCLoudClientSubscribe(struct MQTTCLoud *c)
{
	int i,rc;

	for(i=0;c->config->topics && c->config->topics[i].topicFilter;i++)
	{
		MQTT_PRINTF("MQTT subscribing to topic \"%s\"\n", c->config->topics[i].topicFilter);
		rc=MQTTSubscribe(c->client, c->config->topics[i].topicFilter, c->config->topics[i].qos, c->config->topics[i].messageHandler);
		if(rc)
		{
			MQTT_PRINTF("Return code from MQTT subscribe is %d\n", rc);
			return rc;
		}
		else
		{
			MQTT_PRINTF("MQTT subscribe to topic \"%s\"\n", c->config->topics[i].topicFilter);
		}
	}

	return 0;
}

int MQTTCLoudClientPoll(struct MQTTCLoud *c)
{
	int rc;

	if(!MQTTIsConnected(c->client))
	{
		MQTT_PRINTF("MQTT is disconnected, reconnecting...\n");
		rc = MQTTCLoudClientConnect(c);
		if(rc)
		{
			MQTT_PRINTF("Return code from MQTT connect is %d\n", rc);
			if(c->network->isconnected(c->network))
			{
				c->network->disconnect(c->network);
			}
			return -1;
		}
		else
		{
			MQTT_PRINTF("MQTT Connected\n");
		}

		rc=MQTTCLoudClientSubscribe(c);
		if(rc)
		{
			return MQTTDisconnect(c->client);
		}
	}

	if(!c->network->isconnected(c->network) && MQTTIsConnected(c->client))
	{
		return MQTTDisconnect(c->client);
	}
#ifndef MQTT_TASK
	return MQTTYield(c->client, c->command_timeout_ms);
#else
	return 0;
#endif
}
