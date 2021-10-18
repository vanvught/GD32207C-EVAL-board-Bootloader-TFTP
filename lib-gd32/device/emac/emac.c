/**
 * @file emac.c
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

#include "gd32f20x_enet.h"
#include "gd32f20x_enet_eval.h"

#include "debug.h"

/* ENET RxDMA/TxDMA descriptor */
extern enet_descriptors_struct  rxdesc_tab[ENET_RXBUF_NUM];
#ifdef CHECKSUM_BY_HARDWARE
extern enet_descriptors_struct  txdesc_tab[ENET_TXBUF_NUM];
#endif

/*global transmit and receive descriptors pointers */
extern enet_descriptors_struct  *dma_current_rxdesc;

int emac_start(uint8_t mac_address[]) {
	DEBUG_ENTRY

	enet_system_setup();

	DEBUG_PUTS("enet_system_setup done");

	/* initialize MAC address in Ethernet MAC */

	const uint32_t mac_lo = *(volatile uint32_t *)(0x1FFFF7EC);
	const uint32_t mac_hi = *(volatile uint32_t *)(0x1FFFF7F0);

	mac_address[0] = 2;
	mac_address[1] = (mac_lo >> 8) & 0xff;
	mac_address[2] = (mac_lo >> 16) & 0xff;
	mac_address[3] = (mac_lo >> 24) & 0xff;
	mac_address[4] = (mac_hi >> 0) & 0xff;
	mac_address[5] = (mac_hi >> 8) & 0xff;

#ifndef NDEBUG
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
#endif

	enet_mac_address_set(ENET_MAC_ADDRESS0, mac_address);

	/* initialize descriptors list: chain/ring mode */
	enet_descriptors_chain_init(ENET_DMA_TX);
	enet_descriptors_chain_init(ENET_DMA_RX);
	/* enable ethernet Rx interrrupt */
	{
		int i;
		for (i = 0; i < ENET_RXBUF_NUM; i++) {
			enet_desc_receive_complete_bit_enable(&rxdesc_tab[i]);
		}
	}
#ifdef CHECKSUM_BY_HARDWARE
    /* enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
    for(i=0; i < ENET_TXBUF_NUM; i++){
        enet_transmit_checksum_config(&txdesc_tab[i], ENET_CHECKSUM_TCPUDPICMP_FULL);
    }
#endif /* CHECKSUM_BY_HARDWARE */
    /* note: TCP, UDP, ICMP checksum checking for received frame are enabled in DMA config */
    /* enable MAC and DMA transmission and reception */
    enet_enable();

    DEBUG_EXIT
	return 1;
}

int emac_eth_recv(uint8_t **packet) {
	const uint32_t size = enet_rxframe_size_get();

	if (size > 0) {
		*packet = (uint8_t*) (enet_desc_information_get(dma_current_rxdesc, RXDESC_BUFFER_1_ADDR));
		return size;

	}

	return -1;
}

void emac_free_pkt(void) {
	ENET_NOCOPY_FRAME_RECEIVE();
}

void emac_eth_send(void *packet, int len) {
	enet_frame_transmit (packet, len);
}
