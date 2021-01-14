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

static bool addr_correct;
static uint16_t addr;

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
