/**
 * @file network.h
 *
 */
/* Copyright (C) 2017-2024 by Arjan van Vught mailto:info@gd32-dmx.org
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

#ifndef EMAC_NETWORK_H_
#define EMAC_NETWORK_H_

#if defined (NO_EMAC) || defined (ESP8266)
# error This file should not be included
#endif

#if !defined (HAVE_NET_HANDLE)
# define HAVE_NET_HANDLE
#endif

namespace net {
#if defined (CONFIG_ENET_ENABLE_PTP)
void ptp_run();
#endif
}  // namespace net

#include <cstdint>
#include <cstring>
#include <cassert>
#include <net/if.h>

#include "networkparams.h"

#include "net.h"
#include "netif.h"

#include "emac/net_link_check.h"

#define HAVE_NET_HANDLE

class Network {
public:
	Network();
	~Network() = default;

	void Start(const net::Link link);
	void Print();

	void Shutdown() {
		network::display_emac_shutdown();
		network::mdns_shutdown();
		net::net_shutdown();
	}

	void MacAddressCopyTo(uint8_t *pMacAddress) {
		memcpy(pMacAddress, net::netif_hwaddr(), NETIF_MAX_HWADDR_LEN);
	}

	uint32_t GetSecondaryIp() {
		return net::netif_secondary_ipaddr();
	}

	void SetIp(uint32_t nIp);
	uint32_t GetIp() {
		return net::netif_ipaddr();
	}

	void SetNetmask(uint32_t nNetmask);
	uint32_t GetNetmask() {
		return net::netif_netmask();
	}

	void SetGatewayIp(uint32_t nGatewayIp);
	uint32_t GetGatewayIp()  {
		return net::netif_gw();
	}

	uint32_t GetBroadcastIp()  {
		return net::netif_broadcast_ipaddr();
	}

	/*
	 * DHCP
	 */

	bool IsDhcpCapable() const {
		return m_IsDhcpCapable;
	}

	bool EnableDhcp();

	bool IsDhcpUsed() const {
		return m_IsDhcpUsed;
	}

	bool IsDhcpKnown() const {
		return true;
	}

	network::dhcp::Mode GetDhcpMode() const {
		if (m_IsDhcpUsed) {
			return network::dhcp::Mode::ACTIVE;
		}

		return network::dhcp::Mode::INACTIVE;
	}

	/*
	 * Zeroconf
	 */

	bool IsZeroconfCapable() const {
		return m_IsZeroconfCapable;
	}
	bool SetZeroconf();
	bool IsZeroconfUsed() const {
		return m_IsZeroconfUsed;
	}

	void SetHostName(const char *pHostName);
	const char *GetHostName() const {
		return m_aHostName;
	}

	void SetDomainName(const char *pDomainName) {
		strncpy(m_aDomainName, pDomainName, network::DOMAINNAME_SIZE - 1);
		m_aDomainName[network::DOMAINNAME_SIZE - 1] = '\0';
	}
	const char *GetDomainName() const {
		return m_aDomainName;
	}

	/*
	 * UDP/IP
	 */

	int32_t Begin(uint16_t nPort) {
		const auto nIndex = net::udp_begin(nPort);
		assert(nIndex != -1);
		return nIndex;
	}

	int32_t End(uint16_t nPort) {
		const auto nIndex = net::udp_end(nPort);
		assert(nIndex == 0);
		return nIndex;
	}

	uint16_t RecvFrom(int32_t nHandle, void *pBuffer, uint16_t nLength, uint32_t *from_ip, uint16_t *from_port) {
		return net::udp_recv1(nHandle, reinterpret_cast<uint8_t *>(pBuffer), nLength, from_ip, from_port);
	}

	uint16_t RecvFrom(int32_t nHandle, const void **ppBuffer, uint32_t *pFromIp, uint16_t *pFromPort) {
		return net::udp_recv2(nHandle, reinterpret_cast<const uint8_t **>(ppBuffer), pFromIp, pFromPort);
	}

	void SendTo(int32_t nHandle, const void *pBuffer, uint16_t nLength, uint32_t to_ip, uint16_t remote_port) {
		net::udp_send(nHandle, reinterpret_cast<const uint8_t *>(pBuffer), nLength, to_ip, remote_port);
	}

	void SendToTimestamp(int32_t nHandle, const void *pBuffer, uint16_t nLength, uint32_t to_ip, uint16_t remote_port) {
		net::udp_send_timestamp(nHandle, reinterpret_cast<const uint8_t *>(pBuffer), nLength, to_ip, remote_port);
	}

	/*
	 * TCP/IP
	 */

	int32_t TcpBegin(const uint16_t nLocalPort) {
		return net::tcp_begin(nLocalPort);
	}

	int32_t TcpEnd(const int32_t nHandle);

	uint16_t TcpRead(const int32_t nHandleListen, const uint8_t **ppBuffer, uint32_t &HandleConnection) {
		return net::tcp_read(nHandleListen, ppBuffer, HandleConnection);
	}

	void TcpWrite(const int32_t nHandleListen, const uint8_t *pBuffer, uint16_t nLength, const uint32_t HandleConnection) {
		net::tcp_write(nHandleListen, pBuffer, nLength, HandleConnection);
	}

	/*
	 * IGMP
	 */

	void JoinGroup([[maybe_unused]] int32_t nHandle, uint32_t nIp) {
		net::igmp_join(nIp);
	}

	void LeaveGroup([[maybe_unused]] int32_t nHandle, uint32_t nIp) {
		net::igmp_leave(nIp);
	}

	void SetQueuedStaticIp(uint32_t nLocalIp = 0, uint32_t nNetmask = 0);
	void SetQueuedDhcp() {
		m_QueuedConfig.nMask |= QueuedConfig::DHCP;
	}
	void SetQueuedZeroconf() {
		m_QueuedConfig.nMask |= QueuedConfig::ZEROCONF;
	}

	bool ApplyQueuedConfig();

	uint32_t GetNetmaskCIDR() {
		return static_cast<uint32_t>(__builtin_popcount(GetNetmask()));
	}

	char GetAddressingMode() {
		if (Network::Get()->IsZeroconfUsed()) {
			return  'Z';
		} else if (Network::Get()->IsDhcpKnown()) {
			if (Network::Get()->IsDhcpUsed()) {
				return 'D';
			} else {
				return 'S';
			}
		}

		return 'U';
	}

	const char *GetIfName() const {
		return m_aIfName;
	}

	uint32_t GetIfIndex() const {
		return m_nIfIndex;
	}

	bool IsValidIp(const uint32_t nIp) {
		return (GetIp() & GetNetmask()) == (nIp & GetNetmask());
	}

	void Run() {
		net::net_handle();
#if defined (CONFIG_ENET_ENABLE_PTP)
		net::ptp_run();
#endif
#if defined (ENET_LINK_CHECK_USE_PIN_POLL)
		net::link_pin_poll();
#elif defined (ENET_LINK_CHECK_REG_POLL)
		const net::Link link_state = net::link_status_read();

		if (link_state != s_lastState) {
			s_lastState = link_state;
			net::link_handle_change(link_state);
		}
#endif
	}

	static Network *Get() {
		return s_pThis;
	}

private:
	net::Link s_lastState { net::Link::STATE_DOWN };
	bool m_IsDhcpCapable { true };
	bool m_IsDhcpUsed { false };
	bool m_IsZeroconfCapable { true };
	bool m_IsZeroconfUsed { false };
	uint32_t m_nIfIndex { 1 };
	uint8_t m_nDhcpRetryTime { 0 };

	char m_aHostName[network::HOSTNAME_SIZE];
	char m_aDomainName[network::DOMAINNAME_SIZE];
	char m_aIfName[IFNAMSIZ];

	struct QueuedConfig {
		static constexpr uint32_t NONE = 0;
		static constexpr uint32_t STATIC_IP = (1U << 0);
		static constexpr uint32_t NET_MASK = (1U << 1);
		static constexpr uint32_t DHCP = (1U << 2);
		static constexpr uint32_t ZEROCONF = (1U << 3);
		uint32_t nMask = QueuedConfig::NONE;
		uint32_t nLocalIp = 0;
		uint32_t nNetmask = 0;
	};

	QueuedConfig m_QueuedConfig;

    bool isQueuedMaskSet(const uint32_t nMask) {
    	return (m_QueuedConfig.nMask & nMask) == nMask;
    }

	static Network *s_pThis;
};

#endif /* EMAC_NETWORK_H_ */
