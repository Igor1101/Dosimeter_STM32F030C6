/*
 * serial.h
 */

#ifndef INC_SERIAL_H_
#define INC_SERIAL_H_

#include MCU_HEADER
// defined local definitions for UARTs
#define MAX_FORMAT_TXT_SZ 64
#define MAX_INPUT_SZ	64
#define SERIAL_DEBUG  &huart1
#define SERIAL_AT &huart1
#define pr_debugln(...) serial_println(SERIAL_DEBUG, __VA_ARGS__)
#define pr_debugf(...) serial_printf(SERIAL_DEBUG, __VA_ARGS__)

// here typedef some long types
typedef UART_HandleTypeDef serial_t;
// declared UARTs
extern serial_t huart1;
extern serial_t huart2;

// useful exported functions
int serial_write(serial_t *huart, char*pData, uint16_t Size);
int serial_write_noblk(serial_t *huart, char*pData, uint16_t Size);
int serial_print(serial_t*huart, char*data);
int serial_printf(serial_t*huart, char*format, ...);
int serial_println(serial_t*huart, char*format, ...);
char* serial_getdata(serial_t*huart, uint32_t timeout);
void serial_clear(void);
extern void (*serial_receive_char_callback)(int);

// low level
void serial_IT_enable(serial_t*huart);


#endif /* INC_SERIAL_H_ */
