/**
 * @file gd32f207c_eval.h
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

#ifndef GD32F207C_EVAL_H
#define GD32F207C_EVAL_H

#define LED2_PIN            GPIO_PIN_0
#define LED2_GPIO_PORT      GPIOC
#define LED2_GPIO_CLK       RCU_GPIOC

#define LED3_PIN            GPIO_PIN_2
#define LED3_GPIO_PORT      GPIOC
#define LED3_GPIO_CLK       RCU_GPIOC

#define LED4_PIN            GPIO_PIN_0
#define LED4_GPIO_PORT      GPIOE
#define LED4_GPIO_CLK       RCU_GPIOE

#define LED5_PIN            GPIO_PIN_1
#define LED5_GPIO_PORT      GPIOE
#define LED5_GPIO_CLK       RCU_GPIOE

#define USART0_RCU_CLK		RCU_USART0
#define USART0_GPIO_PORT	GPIOA
#define USART0_GPIO_CLK		RCU_GPIOA
#define USART0_TX_PIN		GPIO_PIN_9
#define USART0_RX_PIN		GPIO_PIN_10

#define UART3_RCU_CLK		RCU_UART3
#define UART3_GPIO_PORT		GPIOC
#define UART3_GPIO_CLK		RCU_GPIOC
#define UART3_TX_PIN		GPIO_PIN_10
#define UART3_RX_PIN		GPIO_PIN_11

#define UART4_RCU_CLK		RCU_UART4
#define UART4_GPIO_TX_PORT	GPIOC
#define UART4_GPIO_TX_CLK	RCU_GPIOC
#define UART4_GPIO_RX_PORT	GPIOD
#define UART4_GPIO_RX_CLK	RCU_GPIOD
#define UART4_TX_PIN		GPIO_PIN_12
#define UART4_RX_PIN		GPIO_PIN_2

#define USART5_RCU_CLK		RCU_USART5
#define USART5_GPIO_PORT	GPIOC
#define USART5_GPIO_CLK		RCU_GPIOC
#define USART5_TX_PIN		GPIO_PIN_6
#define USART5_RX_PIN		GPIO_PIN_7

#endif /* GD32F207C_EVAL_H */
