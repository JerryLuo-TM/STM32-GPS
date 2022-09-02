#ifndef __HMC5883_H
#define __HMC5883_H

#include "stdio.h"
#include "sys.h"

/******************************************************************************
                                define
******************************************************************************/
#define HMC5883_ADD_BASS		0x3C	//address
#define HMC5883_Write_ADD		(HMC5883_ADD_BASS | 0x00)	//write
#define HMC5883_Read_ADD		(HMC5883_ADD_BASS | 0x01)	//read


void HMC5883_Write_Byte(uint8_t REG_ADD, uint8_t dat);
uint8_t HMC5883_Read_Byte(uint8_t REG_ADD);
void HMC5883_Write_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len);
void HMC5883_Read_Buf_Len(uint8_t REG_ADD, uint8_t *pBuff, uint8_t len);

void HMC5883_Init(void);

float HMC5883_get_angle(void);

#endif


