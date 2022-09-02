#include "Struct.h"
#include "sys.h"
#include "iic.h"

#include <stdio.h>
#include <stdarg.h>
#include <math.h>       /* atan2 */

#include "ring_buffer.h"

#include "hmc5883.h"

void HMC5883_Write_Byte(uint8_t REG_ADD, uint8_t dat)
{
	IIC_Start();

	IIC_Send_Byte(HMC5883_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(dat);
	IIC_Wait_Ack();

	IIC_Stop();
}

uint8_t HMC5883_Read_Byte(uint8_t REG_ADD)
{
	uint8_t ReData;
	IIC_Start();

	IIC_Send_Byte(HMC5883_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(HMC5883_Read_ADD);
	IIC_Wait_Ack();

	ReData = IIC_Read_Byte(0);
	IIC_Stop();

	return ReData;
}

void HMC5883_Write_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len)
{
	uint32_t i;

	IIC_Start();

	IIC_Send_Byte(HMC5883_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	for(i = 0;i < len; i++) {
		IIC_Send_Byte(*pBuff++);
		IIC_Wait_Ack();
	}

	IIC_Stop();
}

void HMC5883_Read_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len)
{
	uint32_t i;

	IIC_Start();

	IIC_Send_Byte(HMC5883_Write_ADD);
	IIC_Wait_Ack();

	IIC_Send_Byte(REG_ADD);
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(HMC5883_Read_ADD);
	IIC_Wait_Ack();

	for(i = 0; i < len; i++) {
		if(i == (len - 1))	{
			*pBuff++ = IIC_Read_Byte(0);
		} else {
			*pBuff++ = IIC_Read_Byte(1);
		}
	}
	IIC_Stop();
}

void HMC5883_Init(void)
{
	IIC_Init();

	/* system mode： 
		0x00 连续测量模式 
		0x01 单一测量模式
		0x02 闲置模式
		0x03 闲置模式
	*/
	HMC5883_Write_Byte(0x02, 0x00); 

	/* config A:
		平均数[6:5]:     00:	1  
						01:		2  
						10:		4 11:8
		输出速率[4:2]:	 000: 	0.75  
						001:	1.5
						010:	3
						011:	7.5
						100:	15
						101:	30
						110: 	75
						111:    不使用
	*/
	HMC5883_Write_Byte(0x00, 0x78);
	HMC5883_Write_Byte(0x01, 0x20);
}

float g_angle[3];
float HMC5883_get_angle(void)
{
	uint16_t x, y, z;
	uint8_t temp_buffer[6];
	double angle;

	(void)z;

	/* 连续读取 */
	HMC5883_Read_Buf_Len(0x03, temp_buffer, 6);

    x = (temp_buffer[0] << 8) | temp_buffer[1]; //Combine MSB and LSB of X Data output register
    z = (temp_buffer[2] << 8) | temp_buffer[3]; //Combine MSB and LSB of Z Data output register
    y = (temp_buffer[4] << 8) | temp_buffer[5]; //Combine MSB and LSB of Y Data output register

	if ((x & 0x8000) !=0) {g_angle[0] = (float)(x - 0xFFFF);} else {g_angle[0] = (float)x;}
	if ((y & 0x8000) !=0) {g_angle[1] = (float)(y - 0xFFFF);} else {g_angle[1] = (float)y;}
	if ((z & 0x8000) !=0) {g_angle[2] = (float)(z - 0xFFFF);} else {g_angle[2] = (float)z;}

	angle = (float)(atan2((double)g_angle[1], (double)g_angle[0]) * (double)(180.0 / 3.14159265)) + 90.0f; // angle in degrees
	angle = KalmanFilter(angle);
	// debug_printf("222 x:%f, y:%f, z:%f angle:%f\r\n", g_angle[0], g_angle[1], g_angle[2], angle);

	/* 磁偏角未做椭圆补偿，与实际会存在无差 */

	return (float)angle;
}
