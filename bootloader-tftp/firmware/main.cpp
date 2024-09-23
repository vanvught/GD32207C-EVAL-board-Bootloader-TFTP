/**
 * @file main.cpp
 *
 */
/* Copyright (C) 2022-2024 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include "hardware.h"
#include "network.h"
#include "networkconst.h"
#include "display.h"

#include "remoteconfig.h"
#include "remoteconfigparams.h"

#include "firmwareversion.h"
#include "software_version.h"

#include "flashcodeinstall.h"
#include "configstore.h"

#include "gd32.h"

void Hardware::RebootHandler() {

}

int main(void) {
	rcu_periph_clock_enable(KEY_BOOTLOADER_TFTP_RCU_GPIOx);
#if defined (GD32F4XX) || defined (GD32H7XX)
	rcu_periph_clock_enable(RCU_PMU);
# if defined (GD32F4XX)
	pmu_backup_ldo_config(PMU_BLDOON_ON);
# endif
	rcu_periph_clock_enable(RCU_BKPSRAM);
	pmu_backup_write_enable();
	gpio_mode_set(KEY_BOOTLOADER_TFTP_GPIOx, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY_BOOTLOADER_TFTP_GPIO_PINx);
#else
	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(KEY_BOOTLOADER_TFTP_RCU_GPIOx);
	if constexpr (KEY_BOOTLOADER_TFTP_GPIOx == GPIOA) {
		if constexpr ((KEY_BOOTLOADER_TFTP_GPIO_PINx == GPIO_PIN_13) || (KEY_BOOTLOADER_TFTP_GPIO_PINx == GPIO_PIN_14)) {
			gpio_pin_remap_config(GPIO_SWJ_DISABLE_REMAP, ENABLE);
		}
	}
	gpio_init(KEY_BOOTLOADER_TFTP_GPIOx, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEY_BOOTLOADER_TFTP_GPIO_PINx);
#endif

	const auto isNotRemote = (bkp_data_read(BKP_DATA_1) != 0xA5A5);
	const auto isNotKey = (gpio_input_bit_get(KEY_BOOTLOADER_TFTP_GPIOx, KEY_BOOTLOADER_TFTP_GPIO_PINx));

	if (isNotRemote && isNotKey) {
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
	Display display(4);
	ConfigStore configStore;
	Network nw;
	FirmwareVersion fw(SOFTWARE_VERSION, __DATE__, __TIME__);
	FlashCodeInstall flashCodeInstall;

	printf("Remote=%c, Key=%c\n", isNotRemote ? 'N' : 'Y', isNotKey ? 'N' : 'Y');
	fw.Print("Bootloader TFTP Server");

	hw.SetMode(hardware::ledblink::Mode::OFF_ON);

	RemoteConfig remoteConfig(remoteconfig::Node::BOOTLOADER_TFTP, remoteconfig::Output::CONFIG);

	RemoteConfigParams remoteConfigParams;
	remoteConfigParams.Load();
	remoteConfigParams.Set(&remoteConfig);

	remoteConfig.SetEnableReboot(true);

	display.Printf(3, "Bootloader TFTP Srvr");

	hw.SetMode(hardware::ledblink::Mode::FAST);
	hw.WatchdogInit();

	while (1) {
		hw.WatchdogFeed();
		nw.Run();
		remoteConfig.Run();
		hw.Run();
	}
}
