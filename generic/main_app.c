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
#include <jWrite/jWrite.h>
#include <stdlib.h>
#define DEF_ALLOC_SZ 128

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
char * JSON_create_alloc(void);
void task_fatal_error(char*str);

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
	tty_println("port:%s", fdata.port);
	tty_println("id:%s", fdata.device_id);
	// main system cycle
	SIM_CMD_DEBUG("AT");
	SIM_CMD_DEBUG("ATE0");
	SIM_CMD_DEBUG("AT+CMEE=2");
	// start GPS tracking
	sim_GPS_init();
	sim_GPS_startgetinfo(15);

	// where all data shall be printed
	while (1) {
		// parse task
		if(sim_parse_task_on) {
			sim_task_parse();
		}
		// operation
		else if(uptime >= 40 + time_last_geiger_counter) {
			geiger_counter_callback(uptime - time_last_geiger_counter);
			time_last_geiger_counter = uptime;
		}
		else if(uptime >= 60 + time_last_lte) {
			char*data_tosnd = JSON_create_alloc();
			sim_tcp_con_init();
			// TODO: add GPS
			sim_tcp_send(data_tosnd, strlen(data_tosnd));
			sim_tcp_con_deinit();
			time_last_lte = uptime;
			free(data_tosnd);
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

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0) {
		// geiger counter
		geiger_counter_int_callback();
	}
}

// create JSON struct
char * JSON_create_alloc(void)
{
	char*buf = malloc(DEF_ALLOC_SZ);
	if(buf == NULL)
		task_fatal_error("CANT ALLOC");
	// node open
	jwOpen( buf, DEF_ALLOC_SZ, JW_OBJECT, JW_PRETTY );
	// dev name
	jwObj_string("dev_id", fdata.device_id);
	// GPS
	jwObj_string( "GPS", sim_GPS_get_data() );
	// data
	jwObj_int( "NanoSv", geiger_counter_nanosv_last);
	// uptime
	jwObj_int( "uptime_s", uptime);
	// array end
	jwEnd();
	// node close
	int err= jwClose();
	(void)err;
	return buf;
}

void task_fatal_error(char*str)
{
	tty_println("err:%s", str);
	while(1);
}
