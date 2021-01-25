/*
 * serial.c
 */
#include MCU_HEADER
#include "serial.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

// buffer used for generating string from formatted text
static char formatbuf[MAX_FORMAT_TXT_SZ];
static char inputbuf[MAX_INPUT_SZ];
static bool receiving_answer = false;
void (*serial_receive_char_callback)(int) = NULL;

int serial_write_blk(serial_t *huart, char*pData, uint16_t Size)
{
	return (int)HAL_UART_Transmit(huart, (uint8_t*)pData, Size, 200);
}


int serial_write(serial_t *huart, char*pData, uint16_t Size)
{
	// first we need to save this data to own buffer
	// since it may be changed outside before transmitten
	static  char buf[MAX_FORMAT_TXT_SZ];
	memcpy(buf, pData, Size);
	while(HAL_UART_Transmit_DMA(huart, (uint8_t*)buf, Size) != HAL_OK);
	return 0;
}


int serial_print(serial_t*huart, char*data)
{
	return serial_write(huart, data, strlen(data));
}

int serial_printf(serial_t*huart, char*format, ...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(formatbuf, sizeof formatbuf, format, arg);
	int result = serial_print(huart, formatbuf);
	va_end(arg);
	return result;
}

void serial_clear(void)
{
	memset(inputbuf, 0, sizeof inputbuf);
}

int serial_println(serial_t*huart, char*format, ...)
{
	va_list arg;
	va_start(arg, format);
	vsnprintf(formatbuf, sizeof formatbuf, format, arg);
	int result = serial_print(huart, formatbuf);
	serial_print(huart, "\r\n");
	va_end(arg);
	return result;
}

// here get RX interrupt callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}

static uint8_t data_to_recv;
void serial_getchar_IT(serial_t*huart)
{
	HAL_UART_Receive_IT(huart, &data_to_recv, 1);
}

// interrupt low level enable force
void serial_IT_enable(serial_t*huart)
{
	SET_BIT(huart->Instance->CR1, USART_CR1_RXNEIE);
}

// block and receive data
char* serial_getdata(serial_t*huart, uint32_t timeout)
{
	receiving_answer = true;
	HAL_UART_Receive(huart, (uint8_t*)&inputbuf, sizeof inputbuf, timeout);
	// previous func disabled interrupt after receiving data
	// enable it again!
	receiving_answer = false;
	serial_IT_enable(huart);
	return inputbuf;
}

void USART1_IRQHandler(void)
{
	if (huart1.Instance->ISR & USART_ISR_RXNE) {
		// here receive callback
		if(serial_receive_char_callback != NULL) {
			serial_receive_char_callback((int)huart1.Instance->RDR);
		}
	} else {
		HAL_UART_IRQHandler(&huart1);
	}
}
