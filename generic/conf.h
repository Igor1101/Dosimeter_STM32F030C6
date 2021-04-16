/*
 * conf.h
 *
 *  Created on: Feb 2, 2021
 *      Author: igor
 */

#ifndef CONF_H_
#define CONF_H_

// this is sample conf file
// only example(invalid) values are provided in sample conf file
// all values should be present as strings, they will be saved to the
// end page of FLASH memory of the MCU
// MOBILE PHONE FOR DIAGNOSTICS
#define CONF_MOBILE_PHONE "+380950000000"
// MAIN SERVER TRANSFER JSON DATA TO
#define CONF_SERV0_ADDR "00.00.100.002"
// SPARE SERVER
#define CONF_SERV1_ADDR "1.3.3.4"
// PORT OF SERVER APPLICATION
#define CONF_PORT		"41"
// DEVICE IDENTIFIER NAME STRING (30 CHARACTERS MAX)
#define CONF_DEVICE_ID	"device_name01"


#endif /* CONF_H_ */
