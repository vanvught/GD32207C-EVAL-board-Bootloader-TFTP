/**
 * @file gd32f207c_eval.h
 *
 */
/* Copyright (C) 2021-2024 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef BOARD_GD32F207C_EVAL_H_
#define BOARD_GD32F207C_EVAL_H_

#include <stdint.h>

#if !defined(BOARD_GD32F207C_EVAL)
# error This file should not be included
#endif

#if defined (MCU_GD32F20X_MCU_H_)
# error This file should be included later
#endif

/**
 * LEDs
 */

#define LED1_GPIO_PINx		GPIO_PIN_0
#define LED1_GPIOx			GPIOC
#define LED1_RCU_GPIOx		RCU_GPIOC

#define LED2_GPIO_PINx		GPIO_PIN_2
#define LED2_GPIOx			GPIOC
#define LED2_RCU_GPIOx		RCU_GPIOC

#define LED3_GPIO_PINx		GPIO_PIN_0
#define LED3_GPIOx			GPIOE
#define LED3_RCU_GPIOx		RCU_GPIOE

#define LED4_GPIO_PINx		GPIO_PIN_1
#define LED4_GPIOx			GPIOE
#define LED4_RCU_GPIOx		RCU_GPIOE

#define LED_BLINK_PIN       LED1_GPIO_PINx
#define LED_BLINK_GPIO_PORT LED1_GPIOx
#define LED_BLINK_GPIO_CLK	LED1_RCU_GPIOx

/**
 * KEYs
 */

#define KEY1_PINx						GPIO_PIN_0
#define KEY1_GPIOx						GPIOA
#define KEY1_RCU_GPIOx					RCU_GPIOA

#define KEY2_PINx						GPIO_PIN_13
#define KEY2_GPIOx						GPIOC
#define KEY2_RCU_GPIOx					RCU_GPIOC

#define KEY3_PINx						GPIO_PIN_14
#define KEY3_GPIOx						GPIOB
#define KEY3_RCU_GPIOx					RCU_GPIOB

#define KEY_BOOTLOADER_TFTP_GPIO_PINx	KEY2_PINx
#define KEY_BOOTLOADER_TFTP_GPIOx		KEY2_GPIOx
#define KEY_BOOTLOADER_TFTP_RCU_GPIOx	KEY2_RCU_GPIOx

/**
 * I2C
 */

#define I2C_REMAP			GPIO_I2C0_REMAP
#define I2C_PERIPH			I2C0_PERIPH
#define I2C_RCU_I2Cx			I2C0_RCU_I2C0
#define I2C_SCL_GPIOx	I2C0_SCL_GPIOx
#define I2C_SCL_RCU_GPIOx	I2C0_SCL_RCU_GPIOx
#define I2C_SDA_GPIOx	I2C0_SDA_GPIOx
#define I2C_SDA_RCU_GPIOx	I2C0_SDA_RCU_GPIOx
#define I2C_SCL_GPIO_PINx			I2C0_SCL_GPIO_PINx
#define I2C_SDA_GPIO_PINx			I2C0_SDA_GPIO_PINx

/**
 * SPI
 */
 
#define SPI_PERIPH			SPI2_PERIPH
#define SPI_RCU_SPIx			SPI2_RCU_SPI2
#define SPI_NSS_GPIOx		SPI2_NSS_GPIOx
#define SPI_NSS_RCU_GPIOx	SPI2_NSS_RCU_GPIOx
#define SPI_NSS_GPIO_PINx	SPI2_NSS_GPIO_PINx
#define SPI_RCU_SPIx			SPI2_RCU_SPI2
#define SPI_GPIOx			SPI2_GPIOx
#define SPI_RCU_GPIOx		SPI2_RCU_GPIOx
#define SPI_SCK_GPIO_PINx			SPI2_SCK_GPIO_PINx
#define SPI_MISO_GPIO_PINx		SPI2_MISO_GPIO_PINx
#define SPI_MOSI_GPIO_PINx		SPI2_MOSI_GPIO_PINx
#define SPI_IO2_PIN			SPI2_IO2_GPIO_PINx
#define SPI_IO3_PIN			SPI2_IO3_GPIO_PINx
#define SPI_DMAx			SPI2_DMAx
#define SPI_DMA_CHx			SPI2_TX_DMA_CHx

/**
 * I2S
 */

#define I2S_PERIPH			SPI2_PERIPH
#define I2S_RCU_SPIx			SPI2_RCU_SPI2
#define I2S_RCU_GPIOx		SPI2_RCU_GPIOx
#define I2S_GPIOx			SPI2_GPIOx
#define I2S_CK_GPIO_PINx		SPI2_SCK_GPIO_PINx
#define I2S_SD_GPIO_PINx		SPI2_MOSI_GPIO_PINx
#define I2S_WS_RCU_GPIOx	SPI2_NSS_RCU_GPIOx
#define I2S_WS_GPIOx		SPI2_NSS_GPIOx
#define I2S_WS_GPIO_PINx	SPI2_NSS_GPIO_PINx
#define I2S_DMAx			SPI2_DMAx
#define I2S_DMA_CHx			SPI2_TX_DMA_CHx

/**
 * U(S)ART
 */

// #define USART0_REMAP
#define USART1_REMAP
#define USART2_FULL_REMAP
// #define USART2_PARTIAL_REMAP
// #define UART3_REMAP
// #define USART5_REMAP
// #define UART6_REMAP

/**
 * Panel LEDs
 */
 
#ifdef __cplusplus
namespace hal {
namespace panelled {
static constexpr uint32_t ACTIVITY = 0;
static constexpr uint32_t ARTNET = 0;
static constexpr uint32_t DDP = 0;
static constexpr uint32_t SACN = 0;
static constexpr uint32_t LTC_IN = 0;
static constexpr uint32_t LTC_OUT = 0;
static constexpr uint32_t MIDI_IN = 0;
static constexpr uint32_t MIDI_OUT = 0;
static constexpr uint32_t OSC_IN = 0;
static constexpr uint32_t OSC_OUT = 0;
static constexpr uint32_t TCNET = 0;
// DMX
static constexpr uint32_t PORT_A_RX = 0;
static constexpr uint32_t PORT_A_TX = 0;
}  // namespace panelled
}  // namespace hal
#endif

/**
 * SPI flash
 */

#define SPI_FLASH_CS_GPIOx			GPIOE
#define SPI_FLASH_CS_RCU_GPIOx		RCU_GPIOE
#define SPI_FLASH_CS_GPIO_PINx		GPIO_PIN_3

/*
#define SPI_FLASH_WP_GPIO_PINx		SPI_IO3_PIN
#define SPI_FLASH_HOLD_GPIO_PINx	SPI_IO4_PIN
*/

/**
 * USB
 */

#define USB_HOST_VBUS_GPIOx			GPIOD
#define USB_HOST_VBUS_RCU_GPIOx		RCU_GPIOD
#define USB_HOST_VBUS_GPIO_PINx		GPIO_PIN_13

/**
 * EXT PHY
 */

#define LINK_CHECK_GPIO_CLK				RCU_GPIOB
#define LINK_CHECK_GPIO_PORT			GPIOB
#define LINK_CHECK_GPIO_PIN 			GPIO_PIN_0
#define LINK_CHECK_EXTI_LINE			EXTI_0
#define LINK_CHECK_EXTI_IRQn			EXTI0_IRQn
#define LINK_CHECK_IRQ_HANDLE			EXTI0_IRQHandler

#define LINK_CHECK_EXTI_CLK				RCU_AF
#define LINK_CHECK_EXTI_PORT_SOURCE		GPIO_PORT_SOURCE_GPIOB
#define LINK_CHECK_EXTI_PIN_SOURCE		GPIO_PIN_SOURCE_0
#define LINK_CHECK_EXTI_SOURCE_CONFIG	gpio_exti_source_select
#define LINK_CHECK_GPIO_CONFIG			gpio_init(LINK_CHECK_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, LINK_CHECK_GPIO_PIN);

/**
 * MCU and BOARD name
 */

#define GD32_MCU_NAME			"GD32F207"
#define GD32_BOARD_NAME			"GD32F207C_EVAL"

#include "mcu/gd32f207_mcu.h"
#include "gd32_gpio.h"

#define GD32_BOARD_LED1			GD32_PORT_TO_GPIO(GD32_GPIO_PORTC, 0)
#define GD32_BOARD_LED2			GD32_PORT_TO_GPIO(GD32_GPIO_PORTC, 2)
#define GD32_BOARD_LED3			GD32_PORT_TO_GPIO(GD32_GPIO_PORTE, 0)
#define GD32_BOARD_LED4			GD32_PORT_TO_GPIO(GD32_GPIO_PORTE, 1)
#define GD32_BOARD_STATUS_LED	GD32_BOARD_LED1

/**
 * LCD
 */

#define DISPLAYTIMEOUT_GPIO		GD32_PORT_TO_GPIO(GD32_GPIO_PORTC, 13)	// KEY2

/**
 * Pixel DMX
 */

#define PIXELDMXSTARTSTOP_GPIO	GD32_BOARD_LED2

/**
 * SPI LCD
 */

#define SPI_LCD_RST_PIN		GD32_PORT_TO_GPIO(GD32_GPIO_PORTA, 6)
#define SPI_LCD_DC_GPIO		GD32_PORT_TO_GPIO(GD32_GPIO_PORTA, 14)
#define SPI_LCD_BL_GPIO		GD32_PORT_TO_GPIO(GD32_GPIO_PORTA, 11)
#if defined(SPI_LCD_HAVE_CS_GPIO)
# define SPI_LCD_CS_GPIO	GD32_PORT_TO_GPIO(GD32_GPIO_PORTA, 15)
#endif

/**
 * FT8xx LCD
 */

#define FT8XX_LCD_DC_GPIO	GD32_PORT_TO_GPIO(GD32_GPIO_PORTA, 14)
#define FT8XX_LCD_CS_GPIO	GD32_PORT_TO_GPIO(GD32_GPIO_PORTA, 15)

#include "gpio_header.h"

#endif /* BOARD_GD32F207C_EVAL_H_ */
