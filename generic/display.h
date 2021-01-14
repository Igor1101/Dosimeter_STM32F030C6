/*
 * display.h
 *
 *  Created on: Jan 14, 2021
 *      Author: igor
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
#include MCU_HEADER
#include <stdbool.h>
#include <sys/types.h>
#include "i2c.h"
// lcd 2004a
#define DLINES 4
#define DLINECHARS 20
 // CMDs
// clear display
#define C_CLR		0b00000001

// return home
#define C_RETHOME 	0b00000010

// entry set
#define C_ENTRYMODE 		0b00000100
#define C_ENTRYMODE_DIREC1 	0b00000010
#define C_ENTRYMODE_SHIFT 	0b00000001

// display control
#define C_CTRL				0b00001000
#define C_CTRL_DISP			0b00000100
#define C_CTRL_CURSOR		0b00000010
#define C_CTRL_BLINK		0b00000001

// shift
#define C_SHIFT				0b00010000
#define C_SHIFT_CURSOR		0b00001000
#define C_SHIFT_DISPLAY		0b00000100

// function set
#define C_FSET				0b00100000
#define C_FSET_DL			0b00010000
#define C_FSET_N_LINE		0b00001000
#define C_FSET_N_FONT		0b00000100

// set CGRAM ADDR
#define C_CGRAM				0b01000000
// set DRAM ADDR
#define C_DRAM				0b10000000

// pins offsets
#define P_OFFS_RS    	(0)
#define P_OFFS_RW    	(1)
#define P_OFFS_EN    	(2)
#define P_OFFS_BACKLIGHT (3)

// pins software
#define P_RS		(1<<(P_OFFS_RS+8))
#define P_RW		(1<<(P_OFFS_RW+8))
#define P_EN		(1<<(P_OFFS_EN+8))
#define P_BACKLIGHT	(1<<(P_OFFS_BACKLIGHT+8))

// some of the DDRAM addresses:
#define DLINE0		(0)
#define DLINE1		(60)
#define DLINE2		(20)
#define DLINE3		(84)

#define I2C_INSTANCE &hi2c1
bool display_addr(uint16_t*addr_out);
bool display_scan(void);
HAL_StatusTypeDef display_bus(uint16_t data);
uint16_t display_current_config(void);
void display_setbacklight(bool back);
HAL_StatusTypeDef display_bus_conf(uint16_t data);
HAL_StatusTypeDef display_cmd(uint8_t cmd);
HAL_StatusTypeDef display_write_RAM(uint8_t data);
HAL_StatusTypeDef display_set_DDRAM(uint8_t addr);
ssize_t display_write(const void *buf, size_t count);
void tty_flush(void);
void tty_puts(char*str);
int tty_println(char*format, ...);
void display_init(void);

#endif /* DISPLAY_H_ */
