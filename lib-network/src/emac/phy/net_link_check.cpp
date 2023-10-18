/**
 * net_link_check.cpp
 *
 */
/* Copyright (C) 2022-2023 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include "emac/net_link_check.h"
#include "emac/phy.h"
#include "emac/mmi.h"

#define PHY_ADDRESS	1

namespace net {
#if defined (ENET_LINK_CHECK_USE_INT)
void link_interrupt_init() {
    link_pin_enable();
    link_pin_recovery();
    link_gpio_init();
    link_exti_init();
}
#endif

#if defined (ENET_LINK_CHECK_USE_PIN_POLL)
void link_pin_poll_init() {
    link_pin_enable();
    link_pin_recovery();
    link_gpio_init();
}
#endif

net::Link link_status_read() {
	uint16_t nValue = 0;
	phy_read(PHY_ADDRESS, mmi::REG_BMSR, nValue);

	if (mmi::BMSR_LINKED_STATUS == (nValue & mmi::BMSR_LINKED_STATUS)) {
		return net::Link::STATE_UP;
	}

	return net::Link::STATE_DOWN;
}
}  // namespace net
