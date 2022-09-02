#ifndef _Struct_H_
#define _Struct_H_

#include "stm32f10x.h"
#include <string.h>
#include <math.h>
#include "sys.h"

#include "ring_buffer.h"
#include "stdbool.h"
#include "timer.h"
#include "usart.h"
#include "usart2.h"
#include "iic.h"

#include "delay.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 
#include "semphr.h"

#include "tft.h"
#include "LED.h"
#include "hmc5883.h"
#include "gps.h"
#include "Kalman.h"

// #define ASCII_PACKAGE_ENABLE


extern RINGBUFF_T uart1_rx_ring;
extern RINGBUFF_T uart2_rx_ring;

extern SemaphoreHandle_t xSemaphore_uart1_rx;
extern SemaphoreHandle_t xSemaphore_uart2_rx;

typedef union
{
	float d;
	unsigned char outuchar[4];
} Data;


void Data_Conversion(float dat, Data d1, uint8_t *buf);
float Data_Conversion_float(uint8_t *buf);



#endif
