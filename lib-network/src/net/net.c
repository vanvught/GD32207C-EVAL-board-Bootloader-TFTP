/**
 * @file net.c
 *
 */
/* Copyright (C) 2018-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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
#include <string.h>
#include <stdbool.h>

#include "net.h"
#include "net_packets.h"
#include "net_debug.h"

extern int emac_eth_recv(uint8_t **);
extern void emac_free_pkt(void);

extern void net_timers_run(void);

extern void arp_init(const uint8_t *, const struct ip_info  *);
extern void arp_handle(struct t_arp *);

extern void ip_init(const uint8_t *, const struct ip_info  *);
extern void ip_set_ip(const struct ip_info  *);
extern void ip_handle(struct t_ip4 *);
extern void ip_shutdown(void);

extern void tcp_init(void);

extern int dhcp_client(const uint8_t *mac_address, struct ip_info  *p_ip_info, const uint8_t *hostname);
extern void dhcp_client_release(void);

extern void rfc3927_init(const uint8_t *mac_address);
extern bool rfc3927(struct ip_info *p_ip_info);

struct ip_info g_ip_info  __attribute__ ((aligned (4)));
uint8_t g_mac_address[ETH_ADDR_LEN] __attribute__ ((aligned (4)));

static uint8_t *s_p;
static bool s_is_dhcp = false;

void __attribute__((cold)) net_init(const uint8_t *mac_address, struct ip_info *p_ip_info, const char *hostname, bool *use_dhcp, bool *is_zeroconf_used) {
	uint32_t i;

	for (i = 0; i < ETH_ADDR_LEN; i++) {
		g_mac_address[i] = mac_address[i];
	}

	const uint8_t *src = (const uint8_t *) p_ip_info;
	uint8_t *dst = (uint8_t *) &g_ip_info;

	for (i = 0; i < sizeof(struct ip_info); i++) {
		*dst++ = *src++;
	}

	ip_init(g_mac_address, &g_ip_info);
	rfc3927_init(g_mac_address);

	*is_zeroconf_used = false;

	if (*use_dhcp) {
		if (dhcp_client(g_mac_address, &g_ip_info, (const uint8_t *)hostname) < 0) {
			*use_dhcp = false;
			DEBUG_PUTS("DHCP Client failed");
			*is_zeroconf_used = rfc3927(&g_ip_info);
		}
	}

	arp_init(g_mac_address, &g_ip_info);
	ip_set_ip(&g_ip_info);
	tcp_init();

	src = (const uint8_t*) &g_ip_info;
	dst = (uint8_t*) p_ip_info;

	for (i = 0; i < sizeof(struct ip_info); i++) {
		*dst++ = *src++;
	}

	s_is_dhcp = *use_dhcp;
}

void __attribute__((cold)) net_shutdown(void) {
	ip_shutdown();

	if (s_is_dhcp) {
		dhcp_client_release();
	}
}

void net_set_ip(uint32_t ip) {
	g_ip_info.ip.addr = ip;

	arp_init(g_mac_address, &g_ip_info);
	ip_set_ip(&g_ip_info);
}

void net_set_gw(uint32_t gw) {
	g_ip_info.gw.addr = gw;

	ip_set_ip(&g_ip_info);
}

bool net_set_dhcp(struct ip_info *p_ip_info, const char *hostname, bool *is_zeroconf_used) {
	bool is_dhcp = false;
	*is_zeroconf_used = false;

	if (dhcp_client(g_mac_address, &g_ip_info, (const uint8_t *)hostname) < 0) {
		DEBUG_PUTS("DHCP Client failed");
		*is_zeroconf_used = rfc3927(&g_ip_info);
	} else {
		is_dhcp = true;
	}

	arp_init(g_mac_address, &g_ip_info);
	ip_set_ip(&g_ip_info);

	const uint8_t *src = (const uint8_t*) &g_ip_info;
	uint8_t *dst = (uint8_t*) p_ip_info;
	uint32_t i;

	for (i = 0; i < sizeof(struct ip_info); i++) {
		*dst++ = *src++;
	}

	s_is_dhcp = is_dhcp;
	return is_dhcp;
}

void net_dhcp_release(void) {
	dhcp_client_release();
	s_is_dhcp = false;
}

bool net_set_zeroconf(struct ip_info *p_ip_info) {
	const bool b = rfc3927(&g_ip_info);

	if (b) {
		arp_init(g_mac_address, &g_ip_info);
		ip_set_ip(&g_ip_info);

		const uint8_t *src = (const uint8_t *) &g_ip_info;
		uint8_t *dst = (uint8_t *) p_ip_info;
		uint32_t i;

		for (i = 0; i < sizeof(struct ip_info); i++) {
			*dst++ = *src++;
		}

		s_is_dhcp = false;
		return true;
	}

	DEBUG_PUTS("Zeroconf failed");
	return false;
}

__attribute__((hot)) void net_handle(void) {
	const int length = emac_eth_recv(&s_p);

	if (__builtin_expect((length > 0), 0)) {
		const struct ether_header *eth = (struct ether_header *) s_p;

		if (eth->type == __builtin_bswap16(ETHER_TYPE_IPv4)) {
			ip_handle((struct t_ip4*) s_p);
		} else if (eth->type == __builtin_bswap16(ETHER_TYPE_ARP)) {
			arp_handle((struct t_arp*) s_p);
		} else {
			DEBUG_PRINTF("type %04x is not implemented", __builtin_bswap16(eth->type));
		}

		emac_free_pkt();
	}

	net_timers_run();
}
