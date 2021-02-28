#include MCU_HEADER
#include <stdbool.h>
#include "usart.h"
#include "gpio.h"
#include "iwdg.h"
#include "i2c.h"
#include "dma.h"
#include "rtc.h"
#include "display.h"
#include <generic/serial.h>
#include <dosimeter/geiger_counter.h>
#include "reset_cause.h"
#include <simmodule_drv/sim.h>
#include <generic/flash_mng.h>

extern void SystemClock_Config(void);

/*
 * signal: system start operation
 * there should be event, that turns CPU on and
 * signal_operation should equal "true",
 * system sets this signal to false at the end of operation
 */
volatile uint32_t time_last_lte = 0;
volatile uint32_t time_last_geiger_counter = 0;
volatile bool signal_lte = false;
volatile bool signal_geiger_counter = false;
volatile uint32_t uptime=0;
static void operation_1s(RTC_HandleTypeDef* hrtc);

int main(void)
{
	// here get reset status(should be done before initializing peripherals)
	reset_cause_t res = reset_cause_get();
	/* Reset peripherals,
	 * init flash and system tick timer peripherals */
	HAL_Init();
	// system clock configuration
	SystemClock_Config();
	// configure and init gpio
	MX_GPIO_Init();
	MX_IWDG_Init();
	// usarts
	MX_DMA_Init();
	MX_USART1_UART_Init();
	//MX_USART2_UART_Init();
	MX_I2C1_Init();
	// init RTC
	MX_RTC_Init();
	tty_println("%s", reset_cause_get_name(res));
	// here set some configuration:
	serial_receive_char_callback = sim_receive_data;
	serial_IT_enable(&huart1);
	// flash driver init
	flash_mng_init();
	// write if needed
	//flash_mng_wr_default_values();
	//memset(&fdata, 0, sizeof fdata);
	flash_mng_read();
	tty_println("serv0:%s", fdata.server0_addr);
	tty_println("port:%s", fdata.conf_port);
	// main system cycle
	SIM_CMD_DEBUG("AT");
	SIM_CMD_DEBUG("ATE0");
	SIM_CMD_DEBUG("AT+CMEE=2");
	while (1) {
		// operation
		if(uptime >= 10 + time_last_geiger_counter) {
			time_last_geiger_counter = uptime;
			tim_geiger_counter_callback_1m(NULL);
		}
		else if(uptime >= 60 + time_last_lte) {
			time_last_lte = uptime;
			sim_tcp_con_init();
			sim_tcp_send("\0qwertyew\n\r", 8);
			sim_tcp_con_deinit();
		}
		__HAL_IWDG_RELOAD_COUNTER(&hiwdg);
			// reset watchdog
		// goto sleep
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	}
}
/**
  * @brief  Alarm A callback.
  * @param  hrtc RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	uptime++;
}
