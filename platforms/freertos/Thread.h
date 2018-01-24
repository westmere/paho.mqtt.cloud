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

#if !defined(MQTT_MUTEX_H)
#define MQTT_MUTEX_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

typedef unsigned portBASE_TYPE UBaseType_t;
typedef xQueueHandle TaskHandle_t;
typedef xQueueHandle TaskHandle_t;
typedef xQueueHandle SemaphoreHandle_t;
typedef xTimeOutType TimeOut_t;
typedef portTickType TickType_t;
#define portTICK_PERIOD_MS portTICK_RATE_MS

typedef struct Thread
{
	TaskHandle_t task;
} Thread;

typedef struct Mutex
{
	SemaphoreHandle_t sem;
} Mutex;

void MutexInit(Mutex*);
int MutexLock(Mutex*);
int MutexUnlock(Mutex*);

#endif
