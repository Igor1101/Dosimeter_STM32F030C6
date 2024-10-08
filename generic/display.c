/*
 * display.c
 *
 *  Created on: Jan 14, 2021
 *      Author: igor
 */

#include MCU_HEADER
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "i2c.h"
#include "display.h"


static bool addr_correct = false;
static uint16_t addr;
static bool backlight = true;
static const uint32_t LCD_WAIT =  (5);

// tty
static uint8_t ttybuf[DLINES][DLINECHARS];

void tty_puts(char*str)
{
	static bool first_time = true;
	if(first_time) {
		first_time = false;
		memset(ttybuf, ' ', sizeof ttybuf);
	}
	// for each string copy to the next
	for(int i=0; i<DLINES; i++) {
		memcpy(ttybuf[i], ttybuf[i+1], DLINECHARS);
	}
	memset(ttybuf[DLINES-1], ' ', DLINECHARS);
	memcpy((char*)ttybuf[DLINES-1], str, strlen(str));
	tty_flush();
	display_set_DDRAM(DLINE3+strlen(str));
}

int tty_println(char*format, ...)
{
	va_list arg;
	va_start(arg, format);
	static char res[DLINECHARS];
	vsnprintf(res, DLINECHARS, format, arg);
	tty_puts(res);
	va_end(arg);
	return 0;
}

void tty_flush(void)
{
	display_set_DDRAM(DLINE0);
	display_write(ttybuf[0], DLINECHARS);
	display_set_DDRAM(DLINE1);
	display_write(ttybuf[1], DLINECHARS);
	display_set_DDRAM(DLINE2);
	display_write(ttybuf[2], DLINECHARS);
	display_set_DDRAM(DLINE3);
	display_write(ttybuf[3], DLINECHARS);
}

bool display_addr(uint16_t*addr_out)
{
	if(addr_out != NULL)
		*addr_out = addr;
	return addr_correct;
}
// here search for I2C device
bool display_scan(void)
{
    HAL_StatusTypeDef result;
    for(uint16_t addri = 0; addri < 128; addri++) {
        result = HAL_I2C_IsDeviceReady(I2C_INSTANCE, addri << 1, 1, 20);
        if(result == HAL_OK) {
        	addr = addri;
        	addr_correct = true;
        	return true;
        }
    }
    return false;
}

void display_init(void)
{
	// set 4-bit mode, 2 lines, 5x7 format
    display_cmd(C_FSET | C_FSET_DL );
    // set display & cursor home (keep this!)
    display_cmd(C_RETHOME);
    // set display, cursor on
    display_cmd(C_CTRL| C_CTRL_DISP | C_CTRL_CURSOR | C_CTRL_BLINK);
    // clear display (optional here)
    display_cmd(C_CLR);
}
HAL_StatusTypeDef display_bus(uint16_t data)
{
	// if display is not initialized
	if(!addr_correct) {
		// then find and initialize it!
		display_scan();
		// initialize it if found ! (this may cause short recursion)
		if(display_addr(NULL)) {
			display_init();
		}
	}
	/*
	 * local software display_bus has the following struct:
	 * | BACKLIGHT | EN | RW | RS | 8-bit user data |
	 *
	 * real display bus has the following:
	 * highest 4 bits: data (part of cmd or part of output data)
	 * lowest 4 bits:
	 * BACKLIGHT EN RW RS
	 */
	while(HAL_I2C_IsDeviceReady(I2C_INSTANCE, addr<<1, 1, HAL_MAX_DELAY) != HAL_OK);

	uint8_t datau = (uint8_t)data ;
	uint8_t config = (uint8_t)(data>>8);

	// highest 4 bits
    uint8_t high = datau & 0xF0;
    // lowest 4 bits
    uint8_t low = (datau << 4) & 0xF0;
    // display config
    uint8_t arr[4];
    arr[0] = high|config|(1<< P_OFFS_EN);
    arr[1] = high | config;
    arr[2] = low|config|(1 << P_OFFS_EN);
    arr[3] = low|config;

    HAL_StatusTypeDef result =
    		HAL_I2C_Master_Transmit(I2C_INSTANCE, addr<<1,
    				arr, sizeof(arr), HAL_MAX_DELAY);
    HAL_Delay(LCD_WAIT);
    return result;
}

uint16_t display_current_config(void)
{
	if(backlight) {
		return P_BACKLIGHT;
	}
	return 0;
}

void display_setbacklight(bool back)
{
	backlight = back;
}

HAL_StatusTypeDef display_bus_conf(uint16_t data)
{
	return display_bus(data | display_current_config());
}

HAL_StatusTypeDef display_cmd(uint8_t cmd)
{
	return display_bus_conf(cmd);
}

HAL_StatusTypeDef display_write_RAM(uint8_t data)
{
	return display_bus_conf(P_RS|data);
}

HAL_StatusTypeDef display_set_DDRAM(uint8_t addr)
{
	return display_cmd(C_DRAM|addr);
}

HAL_StatusTypeDef display_set_CGRAM(uint8_t addr)
{
	return display_cmd(C_CGRAM|addr);
}

ssize_t display_write(const void *buf, size_t count)
{
	ssize_t i;
	for(i=0; i<count; i++) {
		if(display_write_RAM(((uint8_t*)buf)[i]) != HAL_OK) {
			return i;
		}
	}
	return i;
}

void display_puts(char*str)
{
	display_write(str, strlen(str));
}
