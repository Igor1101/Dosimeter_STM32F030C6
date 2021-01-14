/*
 * display.h
 *
 *  Created on: Jan 14, 2021
 *      Author: igor
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
#include <stdbool.h>
#include "i2c.h"

#define I2C_INSTANCE &hi2c1
bool display_addr(uint16_t*addr_out);
bool display_scan(void);

#endif /* DISPLAY_H_ */
