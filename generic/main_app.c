#include "stm32f0xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "iwdg.h"
#include <generic/serial.h>
#include <dosimeter/geiger_counter.h>
#include "reset_cause.h"
extern void SystemClock_Config(void);

static int events_counter;

int main(void)
{
	// here get reset status
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
	pr_debugln("started peripherals");
	pr_debugln("Reset status:%s", reset_cause_get_name(res));
	while (1) {
		// reset watchdog
		//__HAL_IWDG_RELOAD_COUNTER(&hiwdg);
		// goto sleep
		//HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	}
}
// interrupt handlers
// Systick handler

void SysTick_Handler(void)
{
	events_counter++;
	if(events_counter%60000 == 0) {
		tim_geiger_counter_callback_1m(NULL);
	}
}

