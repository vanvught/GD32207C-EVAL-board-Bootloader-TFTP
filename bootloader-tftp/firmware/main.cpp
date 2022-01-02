/**
 * @file main.cpp
 *
 */
/* Copyright (C) 2021 by Arjan van Vught mailto:info@gd32-dmx.nl
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

#include <cstdio>
#include <cstdint>
#include <cassert>

#include "hardware.h"
#include "network.h"
#include "networkconst.h"
#include "storenetwork.h"
#include "display.h"
#include "ledblink.h"

#include "remoteconfig.h"
#include "remoteconfigparams.h"
#include "storeremoteconfig.h"

#include "firmwareversion.h"
#include "software_version.h"

#include "spiflashinstall.h"
#include "spiflashstore.h"

#include "gd32.h"

#include "debug.h"

#ifndef NDEBUG
extern "C" {
	void mem_info(void);
}
#endif

int main(void) {
    /* enable the key clock */
    rcu_periph_clock_enable(KEY_BOOTLOADER_TFTP_RCU_GPIOx);
    rcu_periph_clock_enable(RCU_AF);
    /* configure button pin as input */
    gpio_init(KEY_BOOTLOADER_TFTP_GPIOx, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEY_BOOTLOADER_TFTP_GPIO_PINx);
//      V <----
    if (gpio_input_bit_get(KEY_BOOTLOADER_TFTP_GPIOx, KEY_BOOTLOADER_TFTP_GPIO_PINx)) {
    	// https://developer.arm.com/documentation/ka001423/1-0
    	//1. Disable interrupt response.
    	__disable_irq();
    	//2. Disable all enabled interrupts in NVIC.
    	memset((uint32_t *)NVIC->ICER, 0xFF, sizeof(NVIC->ICER));
    	/* 3. Disable all enabled peripherals which might generate interrupt requests.
    	*  Clear all pending interrupt flags in those peripherals.
    	*  This part is device-dependent, and you can write it by referring to device datasheet.
    	*/

    	/* Clear all pending interrupt requests in NVIC. */
    	memset((uint32_t *)NVIC->ICPR, 0xFF, sizeof(NVIC->ICPR));
    	// 4. Disable SysTick and clear its exception pending bit.
    	SysTick->CTRL = 0;
    	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    	// 5. Load the vector table address of user application code in to VTOR.
    	SCB->VTOR = FLASH_BASE + OFFSET_UIMAGE;
    	// 6. Use the MSP as the current SP.
    	// Set the MSP with the value from the vector table used by the application.
    	__set_MSP( ((unsigned int *)(SCB->VTOR))[0] );
    	// In thread mode, enable privileged access and use the MSP as the current SP.
    	__set_CONTROL( 0 );
    	// 7. Enable interrupts.
    	__enable_irq();
    	// 8. Call the reset handler
    	const uint32_t* reset_p = (uint32_t *)(FLASH_BASE + OFFSET_UIMAGE + 4);
    	asm volatile ("bx %0;" : : "r"(*reset_p));
    }

	Hardware hw;
	Network nw;
	Display display;
	LedBlink lb;
	FirmwareVersion fw(SOFTWARE_VERSION, __DATE__, __TIME__);

	fw.Print("Bootloader TFTP Server");

	SpiFlashInstall flashInstall;
	SpiFlashStore flashStore;

	StoreNetwork storeNetwork;
	nw.SetNetworkStore(&storeNetwork);
	nw.Init(&storeNetwork);
	nw.Print();

	lb.SetMode(ledblink::Mode::OFF_ON);

	RemoteConfig remoteConfig(remoteconfig::Node::BOOTLOADER_TFTP, remoteconfig::Output::CONFIG);

	StoreRemoteConfig storeRemoteConfig;
	RemoteConfigParams remoteConfigParams(&storeRemoteConfig);

	if (remoteConfigParams.Load()) {
		remoteConfigParams.Set(&remoteConfig);
	}

	remoteConfig.SetEnableReboot(true);

	lb.SetMode(ledblink::Mode::FAST);

#ifndef NDEBUG
	mem_info();
#endif

	while (1) {
		nw.Run();
		remoteConfig.Run();
		lb.Run();
	}
}
