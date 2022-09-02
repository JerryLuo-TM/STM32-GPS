#include "Struct.h"
#include "usart2.h"

#include "gps.h"

/* 接收机时间、位置及定位相关的数据 */
unsigned char const GGA_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x00, 0xFA, 0x0F};
unsigned char const GGA_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x01, 0xFB, 0x10};
/* */
unsigned char const GLL_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11};
unsigned char const GLL_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x01, 0xFC, 0x12};
/* 用于定位的卫星编号与 DOP 信息 */
unsigned char const GSA_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};
unsigned char const GSA_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x01, 0xFD, 0x14};
/* 可见卫星的卫星编号及其仰角、方位角、载噪比等信息 */
unsigned char const GSV_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17};
unsigned char const GSV_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x04, 0x01, 0xFF, 0x18};
/* 推荐的最简导航传输数据 */
unsigned char const RMC_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x05, 0x00, 0xFF, 0x19};
unsigned char const RMC_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x05, 0x01, 0x00, 0x1A};
/* 地面速度信息，地面速率为 */
unsigned char const VTG_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x1B};
unsigned char const VTG_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x06, 0x01, 0x01, 0x1C};

unsigned char const ZDA_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x07, 0x00, 0x01, 0x1D};
unsigned char const ZDA_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x07, 0x01, 0x02, 0x1E};

unsigned char const GST_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x08, 0x00, 0x02, 0x1F};
unsigned char const GST_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x08, 0x01, 0x03, 0x20};
/* 文本信息 */
unsigned char const GNTXT_OFF[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x20, 0x00, 0x1A, 0x4F};
unsigned char const GNTXT_ON[] =  {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x20, 0x01, 0x1B, 0x50};
/* 启动模式配置 */
unsigned char const COLD_START[] =  {0xF1, 0xD9, 0x06, 0x04, 0x01, 0x00, 0x00, 0x48, 0x22};
unsigned char const WARM_START[] =  {0xF1, 0xD9, 0x06, 0x04, 0x01, 0x00, 0x01, 0x49, 0x23};
unsigned char const HOTE_START[] =  {0xF1, 0xD9, 0x06, 0x04, 0x01, 0x00, 0x02, 0x4A, 0x24};
/* 输出速率 */
unsigned char const NEMA_1HZ[] =  {0xF1, 0xD9, 0x06, 0x42, 0x14, 0x00, 0x00, 0x01, 0x38, 0x35, 0xE8, 0x03, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0xB5, 0x6B};
unsigned char const NEMA_5HZ[] =  {0xF1, 0xD9, 0x06, 0x42, 0x14, 0x00, 0x00, 0x05, 0x38, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x61, 0x05};
unsigned char const NEMA_10HZ[] = {0xF1, 0xD9, 0x06, 0x42, 0x14, 0x00, 0x00, 0x0A, 0x38, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x02, 0x24};
/* 定位模式配置 */
unsigned char const GNSS_L1[]    =  {0xF1, 0xD9, 0x06, 0x0C, 0x04, 0x00, 0x37, 0x00, 0x00, 0x04, 0x51, 0x7C};
unsigned char const GNSS_L1_L5[] =  {0xF1, 0xD9, 0x06, 0x0C, 0x04, 0x00, 0x37, 0x82, 0x10, 0x04, 0xE3, 0x22};
unsigned char const GNSS_BD[]    =  {0xF1, 0xD9, 0x06, 0x0C, 0x04, 0x00, 0x04, 0x80, 0x00, 0x00, 0x9A, 0x2C};
unsigned char const GNSS_GPS[]   =  {0xF1, 0xD9, 0x06, 0x0C, 0x04, 0x00, 0x01, 0x02, 0x00, 0x00, 0x19, 0xA6};
unsigned char const GNSS_ALL[]   =  {0xF1, 0xD9, 0x06, 0x0C, 0x04, 0x00, 0x37, 0x82, 0x10, 0x04, 0xE3, 0x22};

/* 存入flash */
unsigned char const SAVE_CONFIG[] = {0xF1, 0xD9, 0x06, 0x09, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x00, 0x00, 0x00, 0x46, 0xB7};


SaveData_RMC_t Save_Data_RMC;
SaveData_GGA_t Save_Data_GGA;
UTC_datetime_t UTC_datatime;

void GPS_Init(void)
{
    uart2_send_string((unsigned char*)&GNSS_ALL[0], sizeof(GNSS_ALL));
    uart2_send_string((unsigned char*)&GLL_OFF[0], sizeof(GLL_OFF));
    uart2_send_string((unsigned char*)&GSA_OFF[0], sizeof(GSA_OFF));
    uart2_send_string((unsigned char*)&GSV_OFF[0], sizeof(GSV_OFF));
    uart2_send_string((unsigned char*)&VTG_OFF[0], sizeof(VTG_OFF));
    uart2_send_string((unsigned char*)&ZDA_OFF[0], sizeof(ZDA_OFF));
    uart2_send_string((unsigned char*)&GST_OFF[0], sizeof(GST_OFF));

    uart2_send_string((unsigned char*)&HOTE_START[0], sizeof(HOTE_START));
    
    uart2_send_string((unsigned char*)&NEMA_1HZ[0], sizeof(NEMA_1HZ));

    uart2_send_string((unsigned char*)&SAVE_CONFIG[0], sizeof(SAVE_CONFIG));
}

void Parse_RMC(unsigned char *buffer)
{
    uint32_t i;
	char *subString;
	char *subStringNext;
    char usefullBuffer[2];
    SaveData_RMC_t *p_buf = (SaveData_RMC_t *)buffer;
    for (i = 0 ; i <= 6 ; i++) {
        if (i == 0) {
            if ((subString = strstr((const char*)p_buf, ",")) == NULL) {
                return ;
            }
        } else {
            subString++;
            if ((subStringNext = strstr(subString, ",")) != NULL) {
                switch(i) {
                    case 1:memcpy(Save_Data_RMC.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
                    case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取定位状态
                    case 3:memcpy(Save_Data_RMC.latitude, subString, subStringNext - subString);break;	//获取纬度信息
                    case 4:memcpy(Save_Data_RMC.N_S, subString, subStringNext - subString);break;	//获取N/S
                    case 5:memcpy(Save_Data_RMC.longitude, subString, subStringNext - subString);break;	//获取经度信息
                    case 6:memcpy(Save_Data_RMC.E_W, subString, subStringNext - subString);
                            Save_Data_RMC.isParseData = true;break;	//获取E/W
                    default:break;
                }

                subString = subStringNext;
                if (usefullBuffer[0] == 'A')
                    Save_Data_RMC.isUsefull = true;
                else if(usefullBuffer[0] == 'V')
                    Save_Data_RMC.isUsefull = false;
            } else {
                return ;
            }
        }
    }
}

void Parse_GGA(unsigned char *buffer)
{
    uint32_t i;
	char *subString;
	char *subStringNext;
    char statusBuffer[4];
    SaveData_GGA_t *p_buf = (SaveData_GGA_t *)buffer;
    for (i = 0 ; i <= 9 ; i++) {
        if (i == 0) {
            if ((subString = strstr((const char*)p_buf, ",")) == NULL) {
                return ;
            }
        } else {
            subString++;
            if ((subStringNext = strstr(subString, ",")) != NULL) {
                switch(i) {
                    case 1:memcpy(Save_Data_GGA.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
                    case 2:memcpy(Save_Data_GGA.latitude, subString, subStringNext - subString);break;	//获取纬度信息
                    case 3:memcpy(Save_Data_GGA.N_S, subString, subStringNext - subString);break;	//获取N/S
                    case 4:memcpy(Save_Data_GGA.longitude, subString, subStringNext - subString);break;	//获取经度信息
                    case 5:memcpy(Save_Data_GGA.E_W, subString, subStringNext - subString);break;	//获取E/W
                    case 6:memcpy(statusBuffer, subString, subStringNext - subString);break;        //获取状态指示
                    case 7:memcpy(Save_Data_GGA.satellite_num, subString, subStringNext - subString);break; //获取卫星数
                    case 8:memcpy(Save_Data_GGA.HDOP, subString, subStringNext - subString);break; //获取HDOP
                    case 9:memcpy(Save_Data_GGA.altitude, subString, subStringNext - subString);
                            Save_Data_GGA.isParseData = true; break; //获取海拔
                    default:break;
                }

                subString = subStringNext;
                if (statusBuffer[0] == '1')
                    Save_Data_GGA.isUsefull = true;
                else if(statusBuffer[0] == '0')
                    Save_Data_GGA.isUsefull = false;
            } else {
                return ;
            }
        }
    }
}

// 纬度
double latitude_char2double(char *bytesIn)
{
	float integer;
	float decimal;
	double result;
	integer = (float)(bytesIn[0] - '0') * 10.0f + (float)(bytesIn[1] - '0');
    decimal = (float)(bytesIn[2] - '0') * 10.0f + (float)(bytesIn[3] - '0')
                + (float)(bytesIn[5] - '0') * 0.1f
                + (float)(bytesIn[6] - '0') * 0.01f;
                + (float)(bytesIn[7] - '0') * 0.001f;
                + (float)(bytesIn[8] - '0') * 0.0001f;
                + (float)(bytesIn[9] - '0') * 0.00001f;
	result = (integer + decimal / 60.0f);
	return result;
}

// 经度
double longitude_char2double(char *bytesIn)
{
	float integer;
	float decimal;
	double result;
	integer = (float)(bytesIn[0] - '0') * 100.0f + (float)(bytesIn[1] - '0') * 10.0f  + (float)(bytesIn[2] - '0');
    decimal = (float)(bytesIn[3] - '0') * 10.0f + (float)(bytesIn[4] - '0')
                + (float)(bytesIn[6] - '0') * 0.1f
                + (float)(bytesIn[7] - '0') * 0.01f;
                + (float)(bytesIn[8] - '0') * 0.001f;
                + (float)(bytesIn[9] - '0') * 0.0001f;
	result = (integer + decimal / 60.0f);
	return result;
}

void Parse_UTC_time(uint8_t *buffer)
{
    if (buffer == NULL) {
        return ;
    }

    if (buffer[6] != '.') {
        return ;
    }

    UTC_datatime.hour = (buffer[0] - '0') * 10 + (buffer[1] - '0');
    UTC_datatime.minute = (buffer[2] - '0') * 10 + (buffer[3] - '0');
    UTC_datatime.second = (buffer[4] - '0') * 10 + (buffer[5] - '0');
    // UTC/GMT +8
    UTC_datatime.hour += 8;

    if (UTC_datatime.hour >= 24) {
        UTC_datatime.hour = UTC_datatime.hour - 16;
    }
}

float Parse_HDOP(uint8_t *buffer)
{
    uint8_t i, integer_length;
    float value = 0.0f;
    char *subString;

    if (buffer == NULL) {
        return ERROR_DOHP;
    }

    if ((subString = strstr((const char*)buffer, ".")) == NULL) {
        return ERROR_DOHP;
    }

    /* integer */
    integer_length = subString - (char*)&buffer[0];
    for (i = 0; i < integer_length; i++) {
        if ((buffer[i] < '0') || (buffer[i] > '9')) {
            LED_R = 1;
            return ERROR_DOHP;
        }
        value += ((float)(buffer[i] - '0') * pow(10.0f, (float)(integer_length - i - 1)));
    }
    /* decimal */
    integer_length += 1; /* '.' */
    if ((buffer[integer_length] < '0') || (buffer[integer_length] > '9') 
        || (buffer[integer_length + 1] < '0') || (buffer[integer_length + 1] < '0')) {
        LED_R = 1;
        return ERROR_DOHP;
    }
    value += ((float)(buffer[integer_length] - '0') * 0.1f 
            + (float)(buffer[integer_length + 1] - '0') * 0.01f);

    return value;
}

uint8_t Parse_satellite_num(uint8_t *buffer)
{
    uint8_t temp_num;

    if (buffer == NULL) {
        return 0;
    }

    if ((buffer[0] < '0') || (buffer[0] > '9') || (buffer[1] < '0') || (buffer[1] < '0')) {
        LED_R = 1;
        return 0;
    }

    temp_num = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    if (temp_num > 99) {
        temp_num = 99;
    }

    return temp_num;
}

float Parse_altitude(uint8_t *buffer)
{
    uint8_t i, integer_length;
    float value = 0.0f;
    char *subString;

    if (buffer == NULL) {
        return ERROR_DOHP;
    }

    if ((subString = strstr((const char*)buffer, ".")) == NULL) {
        return ERROR_DOHP;
    }

    /* integer */
    integer_length = subString - (char*)&buffer[0];
    for (i = 0; i < integer_length; i++) {
        if ((buffer[i] < '0') || (buffer[i] > '9')) {
            LED_R = 1;
            return ERROR_DOHP;
        }
        value += ((float)(buffer[i] - '0') * pow(10.0f, (float)(integer_length - i - 1)));
    }

    /* decimal */
    integer_length += 1; /* '.' */
    if ((buffer[integer_length] < '0') || (buffer[integer_length] > '9')) {
        LED_R = 1;
        return ERROR_DOHP;
    }
    value += (float)(buffer[integer_length] - '0') * 0.1f ;

    return value;
}

