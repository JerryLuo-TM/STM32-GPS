#include "Struct.h"

/* uart1 串口环形缓冲
	debug/Wireless
*/
RINGBUFF_T uart1_rx_ring;
uint8_t uart1_rx_ring_buffer[256] __attribute__ ((aligned (4)));

/* uart2 串口环形缓冲
	GPS
*/
RINGBUFF_T uart2_rx_ring;
uint8_t uart2_rx_ring_buffer[4096] __attribute__ ((aligned (4)));

SemaphoreHandle_t xSemaphore_uart1_rx;
SemaphoreHandle_t xSemaphore_uart2_rx;

#define PACK_ID_BEGIN		0x1E00

#define PACK_ID_LATITUDE	0x1E00
#define PACK_ID_LONGITUDE	0x1E01
#define PACK_ID_N_S			0x1E02
#define PACK_ID_E_W			0x1E03
#define PACK_ID_LITE_NUM	0x1E04
#define PACK_ID_HDOP		0x1E05
#define PACK_ID_ALTITUDE	0x1E06
#define PACK_ID_UTC			0x1E07
#define PACK_ID_ANGLE		0x1E08

#define PACK_ID_END			0x1EFF

void RF_TX_Package(uint16_t pack_id, uint8_t *buffer, uint32_t length)
{
	uint8_t temp_buffer[32];

	if ((buffer == NULL) || (length == 0)) {
		return ;
	}

	if ((pack_id < PACK_ID_BEGIN) || (pack_id > PACK_ID_END)) {
		return ;
	}

	temp_buffer[0] = 0xAA;
	temp_buffer[1] = 0xBB;
	temp_buffer[2] = length + 2;
	temp_buffer[3] = (pack_id >> 8);
	temp_buffer[4] = (pack_id & 0xFF);

	uart1_send_package(temp_buffer, 5);
	uart1_send_package(buffer, length);
}

void demo_task(void *pvParameters)
{
	uint32_t x, y, satellite_num;
	float angle, hdop, altitude;
	double latitude = 0, longitude = 0;
	uint32_t *p_buf;
	uint8_t data_buf[16];
	static bool title_need_refresh = true;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1)
	{
		LED_B = ~LED_B;

		if (Save_Data_GGA.isUsefull == true) {
			LED_TEST = 0;
			// update date
			Parse_UTC_time((uint8_t*)Save_Data_GGA.UTCTime);
			// 纬度
			latitude = latitude_char2double(Save_Data_GGA.latitude);
			// 经度
			longitude = longitude_char2double(Save_Data_GGA.longitude);
			// HDOP
			hdop = Parse_HDOP((uint8_t *)Save_Data_GGA.HDOP);
			// 卫星数
			satellite_num = Parse_satellite_num((uint8_t *)Save_Data_GGA.satellite_num);
			// 海拔
			altitude = Parse_altitude((uint8_t *)Save_Data_GGA.altitude);
		} else {
			latitude = 0; longitude = 0;
			UTC_datatime.hour = 0;
			UTC_datatime.minute = 0;
			UTC_datatime.second = 0;
			satellite_num = 0;
			hdop = ERROR_DOHP;
			altitude = 0.0;
		}
		angle = HMC5883_get_angle();

		/* load parameter & TX */
		#ifndef ASCII_PACKAGE_ENABLE
			p_buf = (uint32_t*)&data_buf[0];
			*p_buf = (uint32_t)(latitude * 1000000.0);
			RF_TX_Package(PACK_ID_LATITUDE, data_buf, 4);
			*p_buf = (uint32_t)(longitude * 1000000.0);
			RF_TX_Package(PACK_ID_LONGITUDE, data_buf, 4);
			if ((char)latitude, Save_Data_GGA.N_S[0] == 'S') {data_buf[0] = 1;} else {data_buf[0] = 0;}
			RF_TX_Package(PACK_ID_N_S, data_buf, 1);
			if ((char)latitude, Save_Data_GGA.E_W[0] == 'W') {data_buf[0] = 1;} else {data_buf[0] = 0;}
			RF_TX_Package(PACK_ID_E_W, data_buf, 1);
			*p_buf = satellite_num;
			RF_TX_Package(PACK_ID_LITE_NUM, data_buf, 4);
			*p_buf = (uint32_t)(hdop * 1000.0f);
			RF_TX_Package(PACK_ID_HDOP, data_buf, 4);
			*p_buf = (uint32_t)(altitude * 1000.0f);
			RF_TX_Package(PACK_ID_ALTITUDE, data_buf, 4);
			data_buf[0] = UTC_datatime.hour;
			data_buf[1] = UTC_datatime.minute;
			data_buf[2] = UTC_datatime.second;
			RF_TX_Package(PACK_ID_UTC, (uint8_t*)data_buf, 3);
			*p_buf = (int32_t)(angle * 1000.0f);
			RF_TX_Package(PACK_ID_ANGLE, data_buf, 4);
		#endif

		/* show title & item */
		if (title_need_refresh == true) {
			title_need_refresh = false;
			LCD_printf(16, 1, WHITE, BLACK, "北斗导航系统");
			x = 3; y = 0;
			y += 18;LCD_printf(x, y, WHITE, BLUE, "时间: ");
			y += 18;LCD_printf(x, y, WHITE, BLUE, "纬度: ");
			y += 18;LCD_printf(x, y, WHITE, BLUE, "经度: ");
			y += 18;LCD_printf(x, y, WHITE, BLUE, "卫星: ");
			y += 18;LCD_printf(x, y, WHITE, BLUE, "精度: ");
			y += 18;LCD_printf(x, y, WHITE, BLUE, "航向: ");
		}

		x = 3 + 48; y = 0;
		y += 18;LCD_printf(x, y, WHITE, MAGENTA, "%02d:%02d:%02d", UTC_datatime.hour, UTC_datatime.minute, UTC_datatime.second);
		y += 18;LCD_printf(x, y, WHITE, MAGENTA, "%.4f%c", latitude, Save_Data_GGA.N_S[0]);
		y += 18;LCD_printf(x, y, WHITE, MAGENTA, "%.4f%c", longitude, Save_Data_GGA.E_W[0]);
		y += 18;LCD_printf(x, y, WHITE, MAGENTA, "%02d", satellite_num);
		y += 18;LCD_printf(x, y, WHITE, MAGENTA, "%.2f", hdop);
		y += 18;LCD_printf(x, y, WHITE, MAGENTA, "%.2f°", angle);

		vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ/5);
	}
}

void race_task(void *pvParameters)
{
	uint32_t receive_length;
	uint8_t index = 0, ch;
	uint8_t uart_receive_buffer[128];
	while (1)
	{
		if ( xSemaphoreTake( xSemaphore_uart2_rx, portMAX_DELAY) == pdPASS ) {
			receive_length = RingBuffer_GetCount(&uart2_rx_ring);
			if (receive_length > 6) {
				while (receive_length > 0) {
					RingBuffer_Pop(&uart2_rx_ring, &ch);
					receive_length -= 1;
					if (ch == '$') {
						index = 0;
						memset(uart_receive_buffer, 0, sizeof(uart_receive_buffer));
					}
					uart_receive_buffer[index++] = ch;
					if (ch == '\n') {
						if (uart_receive_buffer[0] == '$' && uart_receive_buffer[1] == 'G'&& uart_receive_buffer[2] == 'N'
								&& uart_receive_buffer[3] == 'R'&& uart_receive_buffer[4] == 'M' && uart_receive_buffer[5] == 'C') {
							Parse_RMC(uart_receive_buffer);
						} else if (uart_receive_buffer[0] == '$' && uart_receive_buffer[1] == 'G'&& uart_receive_buffer[2] == 'N'
								&& uart_receive_buffer[3] == 'G'&& uart_receive_buffer[4] == 'G' && uart_receive_buffer[5] == 'A') {
							Parse_GGA(uart_receive_buffer);
	
						} else if (uart_receive_buffer[0] == '$' && uart_receive_buffer[1] == 'G'&& uart_receive_buffer[2] == 'N'
								&& uart_receive_buffer[3] == 'T'&& uart_receive_buffer[4] == 'X' && uart_receive_buffer[5] == 'T') {
								;
						}
						#ifdef ASCII_PACKAGE_ENABLE
							// debug_printf("%s", uart_receive_buffer);
						#endif
						index = 0;
					}
				}
			}
		}
	}
}

void create_app_task(void)
{
	/* uart1 rx ring buffer init */
	RingBuffer_Init(&uart1_rx_ring, uart1_rx_ring_buffer, 1, sizeof(uart1_rx_ring_buffer));
	RingBuffer_Init(&uart2_rx_ring, uart2_rx_ring_buffer, 1, sizeof(uart2_rx_ring_buffer));

	/* uart2 rx ring buffer init */
	xSemaphore_uart1_rx = xSemaphoreCreateBinary();
	xSemaphore_uart2_rx = xSemaphoreCreateBinary();

	//create demo task
    xTaskCreate((TaskFunction_t )demo_task,
                (const char*    )"demo_task",
                (uint16_t       )4096/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )3,
                (TaskHandle_t*  )NULL);

	//create demo task
    xTaskCreate((TaskFunction_t )race_task,
                (const char*    )"race_task",
                (uint16_t       )1024/sizeof(StackType_t),
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )NULL);
}

void HSI_SetSysClock(uint32_t pllmul)
{
	__IO uint32_t HSIStartUpStatus = 0;

	// 把 RCC 外设初始化成复位状态，这句是必须的
	RCC_DeInit();

	//使能 HSI
	RCC_HSICmd(ENABLE);

	// 等待 HSI 就绪
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;

	// 只有 HSI 就绪之后则继续往下执行
	if (HSIStartUpStatus == RCC_CR_HSIRDY) {
		//-------------------------------------------------------------//

		// 使能 FLASH 预存取缓冲区
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		// SYSCLK 周期与闪存访问时间的比例设置，这里统一设置成 2
		// 设置成 2 的时候，SYSCLK 低于 48M 也可以工作，如果设置成 0 或者 1 的时候，
		// 如果配置的 SYSCLK 超出了范围的话，则会进入硬件错误，程序就死了
		// 0：0 < SYSCLK <= 24M
		// 1：24< SYSCLK <= 48M
		// 2：48< SYSCLK <= 72M
		FLASH_SetLatency(FLASH_Latency_2);
		//------------------------------------------------------------//

		// AHB 预分频因子设置为 1 分频，HCLK = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		// APB2 预分频因子设置为 1 分频，PCLK2 = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		// APB1 预分频因子设置为 1 分频，PCLK1 = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//-----------设置各种频率主要就是在这里设置-------------------//
		// 设置 PLL 时钟来源为 HSE，设置 PLL 倍频因子
		// PLLCLK = 4MHz * pllmul
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, pllmul);
		//-- -----------------------------------------------------//

		// 开启 PLL
		RCC_PLLCmd(ENABLE);

		// 等待 PLL 稳定
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
		}

		// 当 PLL 稳定之后，把 PLL 时钟切换为系统时钟 SYSCLK
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		// 读取时钟切换状态位，确保 PLLCLK 被选为系统时钟
		while (RCC_GetSYSCLKSource() != 0x08) {
		}
	} else {
		// 如果 HSI 开启失败，那么程序就会来到这里，用户可在这里添加出错的代码处理
		// 当 HSE 开启失败或者故障的时候，单片机会自动把 HSI 设置为系统时钟，
		// HSI 是内部的高速时钟，8MHZ
		while (1);
	}
}

int main(void)
{
	/* reset handle had init sysclk */
	/* modify ext freq HSE_VALUE stm32f10x.h */

	HSI_SetSysClock(RCC_PLLMul_9);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组

	/* Delay Init */
	delay_init();

	/* LED Init */
	LED_init();

	/* UART1 DEBUG */
	UART1_Init(57600); //APB2 peripheral = 72Mhz

	/* UART2 GPS */
	UART2_Init(115200);

	delay_ms(1000);
	GPS_Init();

	/* Timestamp timer init @100us*/
	TIM3_Int_Init(999,7199);

	/* create app task */
	create_app_task();

	HMC5883_Init();

	LCD_Init();
	LCD_DispColor(WHITE);

	/* 开启任务调度 */
	vTaskStartScheduler();

	return 0;
}









