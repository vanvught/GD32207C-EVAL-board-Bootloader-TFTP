/**
 * @file udp.c
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
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "dhcp_internal.h"
#include "tftp_internal.h"
#include "ntp_internal.h"

#include "net.h"
#include "net_memcpy.h"
#include "net_packets.h"
#include "net_debug.h"

#include "../../config/net_config.h"

extern int console_error(const char *);

#ifndef ALIGNED
# define ALIGNED __attribute__ ((aligned (4)))
#endif

#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

extern void emac_eth_send(void *, int);
extern uint32_t arp_cache_lookup(uint32_t, uint8_t *);
extern uint16_t net_chksum(void *, uint32_t);

#define UDP_RX_MAX_ENTRIES		(1U << 1) // Must always be a power of 2
#define UDP_RX_MAX_ENTRIES_MASK	(UDP_RX_MAX_ENTRIES - 1)

struct queue_entry {
	uint8_t data[UDP_DATA_SIZE];
	uint32_t from_ip;
	uint16_t from_port;
	uint16_t size;
}ALIGNED;

struct queue {
	uint16_t queue_head;
	uint16_t queue_tail;
	struct queue_entry entries[UDP_RX_MAX_ENTRIES] ALIGNED;
}ALIGNED;

typedef union pcast32 {
	uint32_t u32;
	uint8_t u8[4];
} _pcast32;

static uint32_t s_ports_allowed[UDP_MAX_PORTS_ALLOWED] ;
static struct queue s_recv_queue[UDP_MAX_PORTS_ALLOWED] ;
static struct t_udp s_send_packet ;
static uint16_t s_id ;
static uint32_t broadcast_mask;
static uint32_t on_network_mask;
static uint32_t gw_ip;
static uint8_t s_multicast_mac[ETH_ADDR_LEN] = {0x01, 0x00, 0x5E}; // Fixed part

void udp_set_ip(const struct ip_info *p_ip_info) {
	_pcast32 src;

	src.u32 = p_ip_info->ip.addr;
	memcpy(s_send_packet.ip4.src, src.u8, IPv4_ADDR_LEN);
	broadcast_mask = ~(p_ip_info->netmask.addr);
	on_network_mask = p_ip_info->ip.addr & p_ip_info->netmask.addr;
	gw_ip = p_ip_info->gw.addr;
}

void __attribute__((cold)) udp_init(const uint8_t *mac_address, const struct ip_info  *p_ip_info) {
	uint32_t i;

	for (i = 0; i < UDP_MAX_PORTS_ALLOWED; i++) {
		s_ports_allowed[i] = 0;
		s_recv_queue[i].queue_head = 0;
		s_recv_queue[i].queue_tail = 0;
	}

	s_id = 0;

	// Ethernet
	memcpy(s_send_packet.ether.src, mac_address, ETH_ADDR_LEN);
	s_send_packet.ether.type = __builtin_bswap16(ETHER_TYPE_IPv4);
	// IPv4
	s_send_packet.ip4.ver_ihl = 0x45;
	s_send_packet.ip4.tos = 0;
	s_send_packet.ip4.flags_froff = __builtin_bswap16(IPv4_FLAG_DF);
	s_send_packet.ip4.ttl = 64;
	s_send_packet.ip4.proto = IPv4_PROTO_UDP;
	udp_set_ip(p_ip_info);
	// UDP
	s_send_packet.udp.checksum = 0;
}

void __attribute__((cold)) udp_shutdown(void) {
	DEBUG1_ENTRY

	DEBUG1_EXIT
}

__attribute__((hot)) void udp_handle(struct t_udp *p_udp) {
	uint32_t port_index;
	_pcast32 src;
	uint16_t i;

	const uint16_t dest_port = __builtin_bswap16(p_udp->udp.destination_port);

	if ((dest_port != DHCP_PORT_CLIENT)
			&& (dest_port != TFTP_PORT_SERVER)
			&& (dest_port != NTP_PORT_SERVER)
			&& (dest_port < 1024)) { // There is no support for other UDP defined services
		DEBUG_PRINTF("Not supported -> " IPSTR ":%d", p_udp->ip4.src[0],p_udp->ip4.src[1],p_udp->ip4.src[2],p_udp->ip4.src[3], dest_port);
		return;
	}

	for (port_index = 0; port_index < UDP_MAX_PORTS_ALLOWED; port_index++) {
		if (s_ports_allowed[port_index] == dest_port) {
			break;
		}
	}

	if (__builtin_expect ((port_index == UDP_MAX_PORTS_ALLOWED), 0)) {
		DEBUG_PRINTF(IPSTR ":%d[%x]", p_udp->ip4.src[0],p_udp->ip4.src[1],p_udp->ip4.src[2],p_udp->ip4.src[3], dest_port, dest_port);
		return;
	}

	const uint32_t entry = s_recv_queue[port_index].queue_head;
	struct queue_entry *p_queue_entry = &s_recv_queue[port_index].entries[entry];

	const uint16_t data_length = (uint16_t)(__builtin_bswap16(p_udp->udp.len) - UDP_HEADER_SIZE);

	i = MIN(UDP_DATA_SIZE, data_length);

	memcpy(p_queue_entry->data, p_udp->udp.data, i);

	memcpy(src.u8, p_udp->ip4.src, IPv4_ADDR_LEN);
	p_queue_entry->from_ip = src.u32;
	p_queue_entry->from_port = __builtin_bswap16(p_udp->udp.source_port);
	p_queue_entry->size = i;

	s_recv_queue[port_index].queue_head = (s_recv_queue[port_index].queue_head + 1U) & UDP_RX_MAX_ENTRIES_MASK;

	DEBUG_PRINTF("port_index=%u, i=%u, queue_head=%u", port_index, i, s_recv_queue[port_index].queue_head);
}

// -->

int udp_bind(uint16_t local_port) {
	DEBUG_PRINTF("local_port=%u", local_port);

	int i;

	for (i = 0; i < UDP_MAX_PORTS_ALLOWED; i++) {
		if (s_ports_allowed[i] == local_port) {
			return i;
		}

		if (s_ports_allowed[i] == 0) {
			break;
		}
	}

	if (i == UDP_MAX_PORTS_ALLOWED) {
		console_error("bind");
		return -1;
	}

	s_ports_allowed[i] = local_port;

	DEBUG_PRINTF("i=%d, local_port=%d[%x]", i, local_port, local_port);
	return i;
}

int udp_unbind(uint16_t local_port) {
	DEBUG_PRINTF("local_port=%u[%x]", local_port, local_port);

	for (uint32_t i = 0; i < UDP_MAX_PORTS_ALLOWED; i++) {
		if (s_ports_allowed[i] == local_port) {
			s_ports_allowed[i] = 0;
			s_recv_queue[i].queue_head = 0;
			s_recv_queue[i].queue_tail = 0;
			return 0;
		}
	}

	console_error("unbind\n");
	return -1;
}

uint16_t udp_recv(uint8_t idx, uint8_t *packet, uint16_t size, uint32_t *from_ip, uint16_t *from_port) {
	assert(idx < UDP_MAX_PORTS_ALLOWED);

	if (s_recv_queue[idx].queue_head == s_recv_queue[idx].queue_tail) {
		return 0;
	}

	const uint32_t entry = s_recv_queue[idx].queue_tail;
	struct queue_entry *p_queue_entry = &s_recv_queue[idx].entries[entry];

	const uint32_t i = MIN(size, p_queue_entry->size);

	net_memcpy(packet, p_queue_entry->data, i);

	*from_ip = p_queue_entry->from_ip;
	*from_port = p_queue_entry->from_port;

	s_recv_queue[idx].queue_tail = (s_recv_queue[idx].queue_tail + 1U) & UDP_RX_MAX_ENTRIES_MASK;

	return (uint16_t) i;
}

uint16_t udp_recv2(uint8_t idx, const uint8_t **packet, uint32_t *from_ip, uint16_t *from_port) {
	assert(idx < UDP_MAX_PORTS_ALLOWED);

	if (s_recv_queue[idx].queue_head == s_recv_queue[idx].queue_tail) {
		return 0;
	}

	const uint32_t entry = s_recv_queue[idx].queue_tail;
	const struct queue_entry *p_queue_entry = &s_recv_queue[idx].entries[entry];

	*packet = p_queue_entry->data;
	*from_ip = p_queue_entry->from_ip;
	*from_port = p_queue_entry->from_port;

	s_recv_queue[idx].queue_tail = (s_recv_queue[idx].queue_tail + 1U) & UDP_RX_MAX_ENTRIES_MASK;

	return p_queue_entry->size;
}

int udp_send(uint8_t idx, const uint8_t *packet, uint16_t size, uint32_t to_ip, uint16_t remote_port) {
	assert(idx < UDP_MAX_PORTS_ALLOWED);

	_pcast32 dst;

	if (__builtin_expect ((s_ports_allowed[idx] == 0), 0)) {
		DEBUG_PUTS("ports_allowed[idx] == 0");
		return -1;
	}

	if (to_ip == IPv4_BROADCAST) {
		memset(s_send_packet.ether.dst, 0xFF, ETH_ADDR_LEN);
		memset(s_send_packet.ip4.dst, 0xFF, IPv4_ADDR_LEN);
	} else if ((to_ip & broadcast_mask) == broadcast_mask) {
		memset(s_send_packet.ether.dst, 0xFF, ETH_ADDR_LEN);
		dst.u32 = to_ip;
		memcpy(s_send_packet.ip4.dst, dst.u8, IPv4_ADDR_LEN);
	} else {
		if ((to_ip & 0xE0) == 0xE0) { // Multicast, we know the MAC Address
			_pcast32 multicast_ip;

			multicast_ip.u32 = to_ip;

			s_multicast_mac[3] = multicast_ip.u8[1] & 0x7F;
			s_multicast_mac[4] = multicast_ip.u8[2];
			s_multicast_mac[5] = multicast_ip.u8[3];

			memcpy(s_send_packet.ether.dst, s_multicast_mac, ETH_ADDR_LEN);

			dst.u32 = to_ip;
			memcpy(s_send_packet.ip4.dst, dst.u8, IPv4_ADDR_LEN);
		} else {
			if  (__builtin_expect((on_network_mask != (to_ip & on_network_mask)), 0)) {
				if (gw_ip == arp_cache_lookup(gw_ip, s_send_packet.ether.dst)) {
					dst.u32 = to_ip;
					memcpy(s_send_packet.ip4.dst, dst.u8, IPv4_ADDR_LEN);
				} else {
					console_error("ARP lookup failed -> default gateway\n");
					return -3;
				}
			} else {
				if (to_ip == arp_cache_lookup(to_ip, s_send_packet.ether.dst)) {
					dst.u32 = to_ip;
					memcpy(s_send_packet.ip4.dst, dst.u8, IPv4_ADDR_LEN);
				} else {
					console_error("ARP lookup failed\n");
					return -2;
				}
			}
		}
	}

	//IPv4
	s_send_packet.ip4.id = s_id;
	s_send_packet.ip4.len = __builtin_bswap16((uint16_t)(size + IPv4_UDP_HEADERS_SIZE));
	s_send_packet.ip4.chksum = 0;
	s_send_packet.ip4.chksum = net_chksum((void *) &s_send_packet.ip4, (uint32_t) sizeof(s_send_packet.ip4));

	//UDP
	s_send_packet.udp.source_port = __builtin_bswap16((uint16_t) s_ports_allowed[idx]);
	s_send_packet.udp.destination_port = __builtin_bswap16(remote_port);
	s_send_packet.udp.len = __builtin_bswap16((uint16_t)(size + UDP_HEADER_SIZE));

	net_memcpy(s_send_packet.udp.data, packet, MIN(UDP_DATA_SIZE, size));

	debug_dump( &s_send_packet, size + UDP_PACKET_HEADERS_SIZE);

	emac_eth_send((void *) &s_send_packet, (int) (size + UDP_PACKET_HEADERS_SIZE));

	s_id++;

	return 0;
}

// <---
