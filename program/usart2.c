#include "Struct.h"
#include "sys.h"
#include "usart2.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

uint8_t g_uart2_dma_tx_buffer[UART2_TX_BUFFER_SIZE] __attribute__ ((aligned (4)));
uint8_t g_uart2_dma_rx_buffer[UART2_RX_BUFFER_SIZE] __attribute__ ((aligned (4)));


volatile bool g_uart2_tx_is_transfer = false;

// 串口2 初始化
void UART2_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//使能串口时钟和串口引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_DeInit(USART2);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 			//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化GPIOA.2

	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//初始化GPIOA.3

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;					//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART2, &USART_InitStructure);		//初始化串口2

	// USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启串口接收中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	//空闲中断
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	//使能串口DMA接收
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);	//使能串口DMA接收
	USART_Cmd(USART2, ENABLE);                    	//使能串口2

	UART2_TX_DMA_Config(DMA1_Channel7);
	UART2_RX_DMA_Config(DMA1_Channel6);
}

// DMA1 道配7 UART2 TX
// DMA1_Channel7
void UART2_TX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

	//DMA 配置
	DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;	//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart2_dma_tx_buffer;	//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;					//数据传输方向
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart2_dma_tx_buffer);		//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;						//DMA通道
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//配置DMA传输完成后产生中断
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
}

// DMA1 道配6 UART2 RX
// DMA1_Channel6
void UART2_RX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

	//DMA 配置
	DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;			//DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart2_dma_rx_buffer;		//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//数据传输方向
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart2_dma_rx_buffer);			//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//DMA通道
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//使能DMA通道
	DMA_Cmd(DMA_CHx, ENABLE);

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器

	//配置DMA传输完成后产生中断
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
}

// 开启一次DMA传输
void UART2_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx, uint16_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE);
	DMA_SetCurrDataCounter(DMA_CHx, len);
	DMA_Cmd(DMA_CHx, ENABLE);
}

// USART2 发送单字节
void uart2_sendbyte(uint8_t dat)
{
	while((USART2->SR&0x40)==0);
	USART2->DR=(uint8_t) dat;
}

// USART2 发送字符串
void uart2_send_string(unsigned char *buf,unsigned short length)
{
	u16 i;
	for(i=0; i<length; i++) { //循环发送数据
		uart2_sendbyte(*buf++);
	}
}

// USART2 接收中断
void USART2_IRQHandler(void) //串口2中断服务程序
{
	uint8_t Res;
	uint16_t count;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	(void)Res;

	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
		Res = USART2->SR;
		Res = USART2->DR;
		DMA_Cmd(DMA1_Channel6, DISABLE); //关闭USART2
		count = sizeof(g_uart2_dma_rx_buffer) - DMA_GetCurrDataCounter(DMA1_Channel6);
		__disable_irq();
		if (count <= RingBuffer_GetFree(&uart2_rx_ring)) {
			RingBuffer_InsertMult(&uart2_rx_ring, g_uart2_dma_rx_buffer, count);
		}
		__enable_irq();
		DMA_SetCurrDataCounter(DMA1_Channel6, sizeof(g_uart2_dma_rx_buffer));
		DMA_Cmd(DMA1_Channel6, ENABLE); //启动USART2
		if (xSemaphore_uart2_rx != NULL) {
			xSemaphoreGiveFromISR( xSemaphore_uart2_rx, &xHigherPriorityTaskWoken );
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	} else if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		Res = USART2->DR;    //读出寄存器数据
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

// UART2 DMA RX
void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC6)) {
		//清除全部中断标志
        DMA_ClearITPendingBit(DMA1_IT_GL6);
		/* 接收模式buffer 足够大一般不会触发该终端 */
    }
}

// UART2 DMA TX
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7)) { /* 传输完成 */
		//清除传输完成中断
        DMA_ClearITPendingBit(DMA1_IT_TC7);
		g_uart2_tx_is_transfer = false;
    } else if(DMA_GetITStatus(DMA1_IT_TE7)) { /* 传输错误 */
		//清除传输错误中断
        DMA_ClearITPendingBit(DMA1_IT_TE7);
		g_uart2_tx_is_transfer = false;
	} else {	/* 其它中断 */
		//清除全部中断标志
        DMA_ClearITPendingBit(DMA1_IT_GL7);
	}
}



