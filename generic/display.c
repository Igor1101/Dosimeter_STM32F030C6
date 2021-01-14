/*
 * display.c
 *
 *  Created on: Jan 14, 2021
 *      Author: igor
 */

#include MCU_HEADER
#include <stdbool.h>
#include "i2c.h"
#include "display.h"


static bool addr_correct = false;
static uint16_t addr;
static bool backlight = true;
const uint32_t LCD_WAIT =  (5);

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

HAL_StatusTypeDef display_bus(uint16_t data)
{
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
