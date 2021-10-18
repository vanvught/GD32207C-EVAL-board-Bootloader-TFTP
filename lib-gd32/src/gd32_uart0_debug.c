/**
 * @file gd32_uart0_debug.c
 *
 */
/* Copyright (C) 2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>

#include "gd32_board.h"

#include "gd32f20x_usart.h"

void uart0_init(void) {
	/* enable GPIO clock */
	rcu_periph_clock_enable(USART0_RCU_CLK);
	/* enable USART clock */
	rcu_periph_clock_enable(USART0_GPIO_CLK);
	/* connect port to USARTx_Tx */
	gpio_init(USART0_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, USART0_TX_PIN);
	/* connect port to USARTx_Rx */
	gpio_init(USART0_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, USART0_RX_PIN);
	/* USART configure */
	usart_deinit(USART0);
	usart_baudrate_set(USART0, 115200U);
	usart_word_length_set(USART0, USART_WL_8BIT);
	usart_stop_bit_set(USART0, USART_STB_1BIT);
	usart_parity_config(USART0, USART_PM_NONE);
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_enable(USART0);
}

void uart0_putc(int c) {
	if (c == '\n') {
		while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
			;
		usart_data_transmit(USART0, (uint8_t) '\r');
	}

	while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
			;

	usart_data_transmit(USART0, (uint8_t) c);
}

void uart0_puts(char *s) {
	while (*s != '\0') {
		if (*s == '\n') {
			uart0_putc('\r');
		}
		uart0_putc(*s++);
	}
}
