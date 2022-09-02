#ifndef __USART2_H
#define __USART2_H

#include "stdio.h"	
#include "sys.h" 
#include "stdbool.h"


/*GPS */
#define UART2_TX_BUFFER_SIZE	128
#define UART2_RX_BUFFER_SIZE	512

extern uint8_t g_uart2_dma_tx_buffer[UART2_TX_BUFFER_SIZE];
extern uint8_t g_uart2_dma_rx_buffer[UART2_RX_BUFFER_SIZE];

extern volatile bool g_uart2_tx_is_transfer;

void UART2_Init(uint32_t bound);
void uart2_sendbyte(uint8_t dat);
void uart2_send_string(unsigned char *buf,unsigned short length);
void debug_printf( const char * format, ... );

void UART2_TX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx);
void UART2_RX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx);

void UART2_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx, uint16_t len);

#endif


