/*
 * serial.c
 */
#include "stm32f0xx_hal.h"
#include "serial.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

// buffer used for generating string from formatted text
static char formatbuf[MAX_FORMAT_TXT_SZ];
static char inputbuf[MAX_INPUT_SZ];
static bool receiving_answer = false;

int serial_write_blk(serial_t *huart, char*pData, uint16_t Size)
{
	return (int)HAL_UART_Transmit(huart, (uint8_t*)pData, Size, 200);
}


int serial_print(serial_t*huart, char*data)
{
	return serial_write_blk(huart, data, strlen(data));
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
	if(huart == &huart1)
		USART1->CR1 |= USART_CR1_RXNEIE;
	//if(huart == &huart2)
	//	USART2->CR1 |= USART_CR1_RXNEIE;
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
/**
  * USART2 global interrupt, enabled in NVIC
  */
void USART2_IRQHandler(void)
{
	// default handler if receiving answer to CMD
	if(receiving_answer)
		HAL_UART_IRQHandler(&huart2);
	// simply get a char
	/*
	 if ( USART2->SR & USART_SR_RXNE) {
		 pr_debugf("charrecv:%c", USART2->DR);
	 }*/
}
