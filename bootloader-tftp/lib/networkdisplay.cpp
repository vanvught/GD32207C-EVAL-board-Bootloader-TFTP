/**
 * @file networkdisplay.cpp
 *
 */
/* Copyright (C) 2022 by Arjan van Vught mailto:info@gd32-dmx.org
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

#include "network.h"

#include "display.h"
#include "display7segment.h"

namespace network {
static constexpr auto LINE_IP = 2U;

void display_emac_start() {
	Display::Get()->ClearLine(LINE_IP);
	Display::Get()->PutString("Ethernet start");
}

void display_ip() {
	Display::Get()->ClearLine(LINE_IP);
	Display::Get()->Printf(LINE_IP, "" IPSTR "/%d %c", IP2STR(Network::Get()->GetIp()), Network::Get()->GetNetmaskCIDR(), Network::Get()->GetAddressingMode());
}

void display_netmask() {
	display_ip();
}

void display_gateway() {
}

void display_hostname() {
}

void display_emac_shutdown() {
	Display::Get()->ClearLine(LINE_IP);
	Display::Get()->PutString("Ethernet shutdown");
}

void display_dhcp_status(network::dhcp::ClientStatus nStatus) {
	Display::Get()->ClearLine(LINE_IP);

	switch (nStatus) {
	case network::dhcp::ClientStatus::IDLE:
		break;
	case network::dhcp::ClientStatus::RENEW:
		Display::Get()->PutString("DHCP renewing");
		Display::Get()->Status(Display7SegmentMessage::INFO_DHCP);
		break;
	case network::dhcp::ClientStatus::GOT_IP:
		Display::Get()->PutString("Got IP");
		Display::Get()->Status(Display7SegmentMessage::INFO_NONE);
		break;
	case network::dhcp::ClientStatus::RETRYING:
		Display::Get()->PutString("DHCP retrying");
		Display::Get()->Status(Display7SegmentMessage::INFO_DHCP);
		break;
	case network::dhcp::ClientStatus::FAILED:
		Display::Get()->PutString("DHCP Error");
		Display::Get()->Status(Display7SegmentMessage::ERROR_DHCP);
		break;
	default:
		break;
	}
}
}  // namespace network
