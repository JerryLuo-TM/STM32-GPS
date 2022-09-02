#ifndef __TFT_H
#define __TFT_H

#include "stdio.h"
#include "sys.h"

#define LCD_SCL     PAout(5)
#define LCD_SDA     PAout(7)
#define LCD_CS      PBout(0)

#define LCD_RS      PBout(2)
#define LCD_RST     PBout(1)

#define LCD_BL      PBout(10)



//画笔颜色
#define BLACK   0x0000        // 黑色：    0,   0,   0 //
#define BLUE    0x001F        // 蓝色：    0,   0, 255 //
#define GREEN   0x07E0        // 绿色：    0, 255,   0 //
#define CYAN    0x07FF        // 青色：    0, 255, 255 //
#define RED     0xF800        // 红色：  255,   0,   0 //
#define MAGENTA 0xF81F        // 品红：  255,   0, 255 //
#define YELLOW  0xFFE0        // 黄色：  255, 255, 0   //
#define WHITE   0xFFFF        // 白色：  255, 255, 255 //
#define NAVY    0x000F        // 深蓝色：  0,   0, 128 //
#define DGREEN  0x03E0        // 深绿色：  0, 128,   0 //
#define DCYAN   0x03EF        // 深青色：  0, 128, 128 //
#define MAROON  0x7800        // 深红色：128,   0,   0 //
#define PURPLE  0x780F        // 紫色：  128,   0, 128 //
#define OLIVE   0x7BE0        // 橄榄绿：128, 128,   0 //
#define LGRAY   0xC618        // 灰白色：192, 192, 192 //
#define DGRAY   0x7BEF        // 深灰色：128, 128, 128 //
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 

#define LIGHTGREEN     	 0X841F //浅绿色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


extern uint16_t POINT_COLOR, BACK_COLOR;

void LCD_GPIOInit(void);
void LCD_Init(void);
void LCD_DispColor(uint32_t color);
void LCD_BlockWrite(uint32_t Xstart,uint32_t Xend,uint32_t Ystart,uint32_t Yend);
void LCD_ColorBox(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong,uint16_t color);

void LCD_SetPixel(uint16_t  x,uint16_t  y,uint16_t  color);
void LCD_SetBigPixel(uint16_t  x,uint16_t  y,uint16_t  color);
void LCD_DispPic(uint16_t x,uint16_t y,uint16_t w, uint16_t h,const uint8_t *p);

void LCD_showChar(uint32_t x, uint32_t y, unsigned int Back, unsigned int Front, uint8_t num, uint8_t size);
void LCD_showStr(uint32_t x, uint32_t y, unsigned int Back, unsigned int Front, uint8_t *p,uint8_t size);

void LCD_showHZ_16(unsigned int x, unsigned int y, unsigned int Back, unsigned int Front, unsigned char *str);
void LCD_showHZ_16_str(unsigned int x,unsigned int y,unsigned int Back,unsigned int Front,unsigned char *str);
void LCD_printf(unsigned int x, unsigned int y, unsigned int Back, unsigned int Front, const char * format, ...);

#endif


