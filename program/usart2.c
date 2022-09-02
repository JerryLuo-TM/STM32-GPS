#include "Struct.h"
#include "sys.h"
#include "usart2.h"

#include <stdio.h>
#include <stdarg.h>
#include "ring_buffer.h"

uint8_t g_uart2_dma_tx_buffer[UART2_TX_BUFFER_SIZE] __attribute__ ((aligned (4)));
uint8_t g_uart2_dma_rx_buffer[UART2_RX_BUFFER_SIZE] __attribute__ ((aligned (4)));


volatile bool g_uart2_tx_is_transfer = false;

// ����2 ��ʼ��
void UART2_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//ʹ�ܴ���ʱ�Ӻʹ�������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_DeInit(USART2);

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 			//PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOA.2

	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOA.3

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;					//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);		//��ʼ������2

	// USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//�������ڽ����ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	//�����ж�
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);	//ʹ�ܴ���DMA����
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);	//ʹ�ܴ���DMA����
	USART_Cmd(USART2, ENABLE);                    	//ʹ�ܴ���2

	UART2_TX_DMA_Config(DMA1_Channel7);
	UART2_RX_DMA_Config(DMA1_Channel6);
}

// DMA1 ����7 UART2 TX
// DMA1_Channel7
void UART2_TX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

	//DMA ����
	DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;	//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart2_dma_tx_buffer;	//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;					//���ݴ��䷽��
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart2_dma_tx_buffer);		//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;						//DMAͨ��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//����DMA������ɺ�����ж�
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
}

// DMA1 ����6 UART2 RX
// DMA1_Channel6
void UART2_RX_DMA_Config(DMA_Channel_TypeDef* DMA_CHx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

	//DMA ����
	DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;			//DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uart2_dma_rx_buffer;		//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//���ݴ��䷽��
	DMA_InitStructure.DMA_BufferSize = sizeof(g_uart2_dma_rx_buffer);			//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//DMAͨ��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);

	//ʹ��DMAͨ��
	DMA_Cmd(DMA_CHx, ENABLE);

	//NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//����DMA������ɺ�����ж�
	DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE);
}

// ����һ��DMA����
void UART2_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx, uint16_t len)
{
	DMA_Cmd(DMA_CHx, DISABLE);
	DMA_SetCurrDataCounter(DMA_CHx, len);
	DMA_Cmd(DMA_CHx, ENABLE);
}

// USART2 ���͵��ֽ�
void uart2_sendbyte(uint8_t dat)
{
	while((USART2->SR&0x40)==0);
	USART2->DR=(uint8_t) dat;
}

// USART2 �����ַ���
void uart2_send_string(unsigned char *buf,unsigned short length)
{
	u16 i;
	for(i=0; i<length; i++) { //ѭ����������
		uart2_sendbyte(*buf++);
	}
}

// USART2 �����ж�
void USART2_IRQHandler(void) //����2�жϷ������
{
	uint8_t Res;
	uint16_t count;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	(void)Res;

	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
		Res = USART2->SR;
		Res = USART2->DR;
		DMA_Cmd(DMA1_Channel6, DISABLE); //�ر�USART2
		count = sizeof(g_uart2_dma_rx_buffer) - DMA_GetCurrDataCounter(DMA1_Channel6);
		__disable_irq();
		if (count <= RingBuffer_GetFree(&uart2_rx_ring)) {
			RingBuffer_InsertMult(&uart2_rx_ring, g_uart2_dma_rx_buffer, count);
		}
		__enable_irq();
		DMA_SetCurrDataCounter(DMA1_Channel6, sizeof(g_uart2_dma_rx_buffer));
		DMA_Cmd(DMA1_Channel6, ENABLE); //����USART2
		if (xSemaphore_uart2_rx != NULL) {
			xSemaphoreGiveFromISR( xSemaphore_uart2_rx, &xHigherPriorityTaskWoken );
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	} else if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		Res = USART2->DR;    //�����Ĵ�������
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

// UART2 DMA RX
void DMA1_Channel6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC6)) {
		//���ȫ���жϱ�־
        DMA_ClearITPendingBit(DMA1_IT_GL6);
		/* ����ģʽbuffer �㹻��һ�㲻�ᴥ�����ն� */
    }
}

// UART2 DMA TX
void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC7)) { /* ������� */
		//�����������ж�
        DMA_ClearITPendingBit(DMA1_IT_TC7);
		g_uart2_tx_is_transfer = false;
    } else if(DMA_GetITStatus(DMA1_IT_TE7)) { /* ������� */
		//�����������ж�
        DMA_ClearITPendingBit(DMA1_IT_TE7);
		g_uart2_tx_is_transfer = false;
	} else {	/* �����ж� */
		//���ȫ���жϱ�־
        DMA_ClearITPendingBit(DMA1_IT_GL7);
	}
}



