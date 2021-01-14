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
	// display

	if(display_scan()) {
		uint16_t addr;
		display_addr(&addr);
		pr_debugln("display found at 0x%x", addr);
	}
    // set 4-bit mode, 2 lines, 5x7 format
    display_cmd(C_FSET | C_FSET_DL );
    // set display & cursor home (keep this!)
    display_cmd(C_RETHOME);
    // set display, cursor on
    display_cmd(C_CTRL| C_CTRL_DISP | C_CTRL_CURSOR | C_CTRL_BLINK);
    // clear display (optional here)
    display_cmd(C_CLR);
    uint8_t i=0;
    display_set_DDRAM(DLINE0);
    display_write_RAM('1');
    display_set_DDRAM(DLINE1);
    display_write_RAM('2');
    display_set_DDRAM(DLINE2);
    display_write_RAM('3');
    display_set_DDRAM(DLINE3);
    display_write_RAM('4');
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
