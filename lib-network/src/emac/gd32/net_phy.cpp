/**
 * net_phy.cpp
 *
 */
/* Copyright (C) 2023 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include <cstdint>

#include "emac/phy.h"
#include "emac/mmi.h"

#include "gd32.h"

#include "debug.h"

extern volatile uint32_t s_nSysTickMillis;

namespace net {

bool phy_read(uint32_t nAddress, const uint32_t nRegister, uint16_t &nValue) {
	const auto bResult = enet_phy_write_read(ENET_PHY_READ, nAddress, nRegister, &nValue) == SUCCESS;
//	DEBUG_PRINTF("%d %.2x %.2x %.4x", bResult, nAddress, nRegister, nValue);
	return bResult;
}

bool phy_write(uint32_t nAddress, const uint32_t nRegister, uint16_t nValue) {
	const auto bResult = enet_phy_write_read(ENET_PHY_WRITE, nAddress, nRegister, &nValue) == SUCCESS;
//	DEBUG_PRINTF("%d %.2x %.2x %.4x", bResult, nAddress, nRegister, nValue);
	return bResult;
}

bool phy_config(const uint32_t nAddress) {
	DEBUG_ENTRY

	uint32_t reg = ENET_MAC_PHY_CTL;
	reg &= ~ENET_MAC_PHY_CTL_CLR;

	const uint32_t ahbclk = rcu_clock_freq_get(CK_AHB);

#if defined GD32F10X_CL
	if (ENET_RANGE(ahbclk, 20000000U, 35000000U)) {
		reg |= ENET_MDC_HCLK_DIV16;
	} else if (ENET_RANGE(ahbclk, 35000000U, 60000000U)) {
		reg |= ENET_MDC_HCLK_DIV26;
	} else if (ENET_RANGE(ahbclk, 60000000U, 90000000U)) {
		reg |= ENET_MDC_HCLK_DIV42;
	} else if ((ENET_RANGE(ahbclk, 90000000U, 108000000U)) || (108000000U == ahbclk)) {
		reg |= ENET_MDC_HCLK_DIV62;
	} else {
		return false;
	}
#elif defined GD32F20X
	if (ENET_RANGE(ahbclk, 20000000U, 35000000U)) {
		reg |= ENET_MDC_HCLK_DIV16;
	} else if (ENET_RANGE(ahbclk, 35000000U, 60000000U)) {
		reg |= ENET_MDC_HCLK_DIV26;
	} else if (ENET_RANGE(ahbclk, 60000000U, 100000000U)) {
		reg |= ENET_MDC_HCLK_DIV42;
	} else if ((ENET_RANGE(ahbclk, 100000000U, 120000000U)) || (120000000U == ahbclk)) {
		reg |= ENET_MDC_HCLK_DIV62;
	} else {
		return false;
	}
#elif defined GD32F4XX
	if (ENET_RANGE(ahbclk, 20000000U, 35000000U)) {
		reg |= ENET_MDC_HCLK_DIV16;
	} else if (ENET_RANGE(ahbclk, 35000000U, 60000000U)) {
		reg |= ENET_MDC_HCLK_DIV26;
	} else if (ENET_RANGE(ahbclk, 60000000U, 100000000U)) {
		reg |= ENET_MDC_HCLK_DIV42;
	} else if (ENET_RANGE(ahbclk, 100000000U, 150000000U)) {
		reg |= ENET_MDC_HCLK_DIV62;
	} else if ((ENET_RANGE(ahbclk, 150000000U, 200000000U)) || (200000000U == ahbclk)) {
		reg |= ENET_MDC_HCLK_DIV102;
	} else {
		return false;
	}
#endif

	ENET_MAC_PHY_CTL = reg;

	if (!phy_write(nAddress, mmi::REG_BMCR, mmi::BMCR_RESET)) {
		DEBUG_PUTS("PHY reset failed");
		return false;
	}

	/*
	 * Poll the control register for the reset bit to go to 0 (it is
	 * auto-clearing).  This should happen within 0.5 seconds per the
	 * IEEE spec.
	 */

	const auto nMillis = s_nSysTickMillis;
	uint16_t nValue;

	while (s_nSysTickMillis - nMillis < 500) {
		if (!phy_read(nAddress, mmi::REG_BMCR, nValue)) {
			DEBUG_PUTS("PHY status read failed");
			return false;
		}

		if (!(nValue & mmi::BMCR_RESET)) {
			DEBUG_PRINTF("%u", s_nSysTickMillis - nMillis);
			DEBUG_EXIT
			return true;
		}
	}

	if (nValue & mmi::BMCR_RESET) {
		DEBUG_PUTS("PHY reset timed out");
		return false;
	}

	DEBUG_PRINTF("%u", s_nSysTickMillis - nMillis);
	DEBUG_EXIT
	return true;
}

}  // namespace net
