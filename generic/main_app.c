#include MCU_HEADER
#include "usart.h"
#include "gpio.h"
#include "iwdg.h"
#include "i2c.h"
#include "dma.h"
#include "display.h"
#include <generic/serial.h>
#include <dosimeter/geiger_counter.h>
#include "reset_cause.h"
extern void SystemClock_Config(void);

static int events_counter;

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
	//MX_IWDG_Init();
	// usarts
	MX_USART1_UART_Init();
	//MX_USART2_UART_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	tty_println("%s", reset_cause_get_name(res));
	while (1) {
		//display_write_RAM(i);
		//i++;
		//HAL_Delay(300);
		// reset watchdog
		//__HAL_IWDG_RELOAD_COUNTER(&hiwdg);
		// goto sleep
		//HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	}
}
