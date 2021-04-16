/*
 * flash_mng.c
 *
 *  Created on: Jan 31, 2021
 *      Author: igor
 */

#include MCU_HEADER
#include <string.h>
#include <generic/display.h>
#include "flash_mng.h"
#include "conf.h"
#define FLASH_SIZE_REG  (uint16_t*)0x1FFFF7CC
#define FLASH_START 0x8000000
extern char _flash_data_end;

static uint16_t fsize_kb;
static uint32_t*fblk_start;
static bool init = false;
flash_data_t fdata;
// init
void flash_mng_init(void)
{
	fsize_kb = *FLASH_SIZE_REG;
	tty_println("fsize=%dKB, ", fsize_kb);
	tty_println("fend=%08x", &_flash_data_end);
	// use 1 kb of data
	fblk_start = (uint32_t*)(FLASH_START + fsize_kb * 1024  - 1024);
	tty_println("fdata=%08x", fblk_start);
	if( (uintptr_t)fblk_start < (uintptr_t)&_flash_data_end ) {
		tty_println("err:data overlap");
		return;
	}
	// configure erase struct
	init = true;
}
// erase
void flash_mng_erase(void)
{
	if(!init)
		return;
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef pg_erase;
	pg_erase.NbPages = 1;
	pg_erase.PageAddress = (uint32_t)fblk_start;
	pg_erase.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t page_error = 0;
	HAL_FLASHEx_Erase(&pg_erase, &page_error);
	HAL_FLASH_Lock();
}

void flash_mng_write(void)
{
	if(!init)
		return;
	HAL_FLASH_Unlock();
	// prorgram
	for(uint32_t i = 0; i <= sizeof fdata / sizeof (uint32_t); i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uintptr_t)(fblk_start + i),
				*((uint32_t*)&fdata + i));
	}
	HAL_FLASH_Lock();
}

void flash_mng_read(void)
{
	if(!init)
		return;
	for(uint32_t i = 0; i <= sizeof fdata / sizeof (uint32_t); i++) {
		*(((uint32_t*)&fdata)+i) = *(uint32_t*)(fblk_start+i);
	}
}
void flash_mng_wr_default_values(void)
{
	// erase
	flash_mng_erase();
	// create struct with def values
	strncpy(fdata.mobile_phone, CONF_MOBILE_PHONE, sizeof fdata.mobile_phone);
	strncpy(fdata.server0_addr, CONF_SERV0_ADDR, sizeof fdata.server0_addr);
	strncpy(fdata.server1_addr, CONF_SERV1_ADDR, sizeof fdata.server1_addr);
	strncpy(fdata.device_id, CONF_DEVICE_ID, sizeof fdata.device_id);
	strncpy(fdata.port, CONF_PORT, sizeof fdata.port);
	// write
	flash_mng_write();
}
