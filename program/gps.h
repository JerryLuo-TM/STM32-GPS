#ifndef __GPS_H
#define __GPS_H

#include "stdio.h"
#include "sys.h"

#define ERROR_DOHP  99.99f

typedef struct {
	char isGetData;		//是否获取到GPS数据
	char isParseData;	//是否解析完成
	char UTCTime[11];		//UTC时间
	char latitude[11];		//纬度
	char N_S[2];            //N/S
	char longitude[12];	    //经度
	char E_W[2];            //E/W
	char isUsefull;         //定位信息是否有效
} SaveData_RMC_t;

typedef struct {
	char isGetData;		//是否获取到GPS数据
	char isParseData;	//是否解析完成
	char UTCTime[11];		//UTC时间
	char latitude[11];		//纬度
	char N_S[2];            //N/S
	char longitude[12];	    //经度
	char E_W[2];            //E/W
	char isUsefull;         //定位信息是否有效
    char satellite_num[3];  //参与定位卫星数
    char HDOP[16];           //DHOP值，漂移值，越小越好
    char altitude[16];       //海拔高度
} SaveData_GGA_t;

typedef struct {
    uint16_t year;  /*!< Range from 1970 to 2099.*/
    uint8_t month;  /*!< Range from 1 to 12.*/
    uint8_t day;    /*!< Range from 1 to 31 (depending on month).*/
    uint8_t dow;
    uint8_t hour;   /*!< Range from 0 to 23.*/
    uint8_t minute; /*!< Range from 0 to 59.*/
    uint8_t second; /*!< Range from 0 to 59.*/
} UTC_datetime_t;

extern SaveData_RMC_t Save_Data_RMC;
extern SaveData_GGA_t Save_Data_GGA;
extern UTC_datetime_t UTC_datatime;

void GPS_Init(void);
void Parse_RMC(unsigned char *buffer);
void Parse_GGA(unsigned char *buffer);
void Parse_UTC_time(uint8_t *buffer);
float Parse_HDOP(uint8_t *buffer);
uint8_t Parse_satellite_num(uint8_t *buffer);
float Parse_altitude(uint8_t *buffer);

double latitude_char2double(char *bytesIn);
double longitude_char2double(char *bytesIn);

#endif


