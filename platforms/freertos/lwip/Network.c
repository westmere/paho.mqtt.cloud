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
 *    Ian Craggs - convert to FreeRTOS
 *******************************************************************************/

#include "Network.h"
#include "Timer.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"

int host2addr(const char *hostname , struct in_addr *in)
{
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    rv = getaddrinfo(hostname, 0 , &hints , &servinfo);
    if (rv != 0)
    {
        return rv;
    }

    // loop through all the results and get the first resolve
    for (p = servinfo; p != 0; p = p->ai_next)
    {
        h = (struct sockaddr_in *)p->ai_addr;
        in->s_addr = h->sin_addr.s_addr;
    }
    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}


int NetworkConnect(Network* n, char* host, int port)
{
    struct sockaddr_in addr;
    int ret;

    if (host2addr(host, &(addr.sin_addr)) != 0)
    {
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    n->my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( n->my_socket < 0 )
    {
        // error
        return -1;
    }
    ret = connect(n->my_socket, ( struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if( ret < 0 )
    {
        // error
        close(n->my_socket);
        n->my_socket = -1;
        return ret;
    }

    return ret;
}

#if 0
int NetworkConnectTLS(Network *n, char* addr, int port, SlSockSecureFiles_t* certificates, unsigned char sec_method, unsigned int cipher, char server_verify)
{
    SlSockAddrIn_t sAddr;
    int addrSize;
    int retVal;
    unsigned long ipAddress;

    retVal = sl_NetAppDnsGetHostByName(addr, strlen(addr), &ipAddress, AF_INET);
    if (retVal < 0) {
        return -1;
    }

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = sl_Htons((unsigned short)port);
    sAddr.sin_addr.s_addr = sl_Htonl(ipAddress);

    addrSize = sizeof(SlSockAddrIn_t);

    n->my_socket = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_SEC_SOCKET);
    if (n->my_socket < 0) {
        return -1;
    }

    SlSockSecureMethod method;
    method.secureMethod = sec_method;
    retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECMETHOD, &method, sizeof(method));
    if (retVal < 0) {
        return retVal;
    }

    SlSockSecureMask mask;
    mask.secureMask = cipher;
    retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_MASK, &mask, sizeof(mask));
    if (retVal < 0) {
        return retVal;
    }

    if (certificates != NULL) {
        retVal = sl_SetSockOpt(n->my_socket, SL_SOL_SOCKET, SL_SO_SECURE_FILES, certificates->secureFiles, sizeof(SlSockSecureFiles_t));
        if (retVal < 0)
        {
            return retVal;
        }
    }

    retVal = sl_Connect(n->my_socket, (SlSockAddr_t *)&sAddr, addrSize);
    if (retVal < 0) {
        if (server_verify || retVal != -453) {
            sl_Close(n->my_socket);
            return retVal;
        }
    }

    SysTickIntRegister(SysTickIntHandler);
    SysTickPeriodSet(80000);
    SysTickEnable();

    return retVal;
}
#endif

static int nw_lwip_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	Timer timer;
    int recvLen = 0;
    int recv_timeout = timeout_ms;
    int rc = 0;

    struct timeval timeout;
    fd_set fdset;

    if(0 > n->my_socket)
		return -1;

    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = timeout_ms * 1000;

    TimerCountdownMS(&timer, timeout_ms);
    if (select(n->my_socket + 1, &fdset, NULL, NULL, &timeout) > 0) {
        if (FD_ISSET(n->my_socket, &fdset)) {
            do {
                rc = recv(n->my_socket, buffer + recvLen, len - recvLen, MSG_DONTWAIT);
                if (rc > 0) {
                    recvLen += rc;
                } else if (rc < 0) {
                    recvLen = rc;
                    break;
                }
            } while (recvLen < len && !TimerIsExpired(&timer));
        }
    }
    return recvLen;
}


static int nw_lwip_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	Timer timer;
    int sentLen = 0;
    int send_timeout = timeout_ms;
    int rc = 0;
    int readysock;

    struct timeval timeout;
    fd_set fdset;

    if(0 > n->my_socket)
		return -1;

    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = timeout_ms * 1000;

    TimerCountdownMS(&timer, timeout_ms);
    do {
        readysock = select(n->my_socket + 1, NULL, &fdset, NULL, &timeout);
    } while (readysock <= 0);
    if (FD_ISSET(n->my_socket, &fdset)) {
        do {
            rc = send(n->my_socket, buffer + sentLen, len - sentLen, MSG_DONTWAIT);
            if (rc > 0) {
                sentLen += rc;
            } else if (rc < 0) {
                sentLen = rc;
                break;
            }
        } while (sentLen < len && !TimerIsExpired(&timer));
    }

    return sentLen;
}

void nw_lwip_disconnect(Network* n)
{
	if(0 > n->my_socket)
		return;
	close(n->my_socket);
	n->my_socket = -1;
//	return 0;
}

int nw_lwip_isconnected(Network* n)
{
	int error = 0;
	int retval;

	if(0 > n->my_socket)
		return 0;
	socklen_t len = sizeof (error);
	retval = getsockopt (n->my_socket, SOL_SOCKET, SO_ERROR, &error, &len);
//	if(retval)
//	{
//		/* there was a problem getting the error code */
//		printf("error getting socket error code: %s\n", strerror(retval));
//		return 0;
//	}
//
//	if(error)
//	{
//		/* socket has a non zero error status */
//		char *err_str;
//
//		err_str=(char *)strerror(error);
//		printf("socket error: %s\n", strlen(err_str) ? err_str:"unknown");
//		return 0;
//	}

	return (retval || error) ? 0 : 1;
}

void NetworkInit(Network* n)
{
	n->my_socket = -1;
	n->mqttread = nw_lwip_read;
	n->mqttwrite = nw_lwip_write;
	n->disconnect = nw_lwip_disconnect;
	n->isconnected = nw_lwip_isconnected;
}
