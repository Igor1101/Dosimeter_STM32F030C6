/*
 * flash_mng.c
 *
 *  Created on: Jan 31, 2021
 *      Author: igor
 */

#include MCU_HEADER
#include <generic/display.h>
#include "flash_mng.h"
#define FLASH_SIZE_REG  (uint16_t*)0x1FFFF7CC

static uint16_t fsize_kb;
static uintptr_t fblk_start;
static bool init = false;
flash_data_t fdata;
// init
void flash_mng_init(void)
{
	fsize_kb = *FLASH_SIZE_REG;
	tty_println("fsize=%dKB, using last page", fsize_kb);
	// use 1 kb of data
	fblk_start = fsize_kb * 1024  - 1024;
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
	pg_erase.PageAddress = fblk_start;
	pg_erase.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t page_error = 0;
	HAL_FLASHEx_Erase(&pg_erase, &page_error);
	HAL_FLASH_Lock();
}

void flash_mng_write(void)
{
	HAL_FLASH_Unlock();
	// prorgram
	for(uint32_t i = 0; i <= sizeof fdata; i+=sizeof(uint32_t)) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, fblk_start + i,*(((uint8_t*)&fdata)+i));
	}
	HAL_FLASH_Lock();
}

void flash_mng_read(void)
{
	for(uint32_t i = 0; i <= sizeof fdata; i+=sizeof(uint32_t)) {
		*(((uint8_t*)&fdata)+i) = *(uint32_t*)(fblk_start+i);
	}
}
