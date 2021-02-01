/*
 * flash_mng.h
 *
 *  Created on: Jan 31, 2021
 *      Author: igor
 */

#ifndef FLASH_MNG_H_
#define FLASH_MNG_H_

#include MCU_HEADER
typedef struct {
	char server0_addr[32];
	char server1_addr[32];
	char mobile_phone[16];
	char unused[16];
} flash_data_t;
extern flash_data_t fdata;

void flash_mng_init(void);
void flash_mng_erase(void);
void flash_mng_read(void);
void flash_mng_write(void);

#endif /* FLASH_MNG_H_ */
