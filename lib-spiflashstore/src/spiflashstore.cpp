/**
 * @file spiflashstore.cpp
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

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "spiflashstore.h"

#include "spi_flash.h"

#ifndef NDEBUG
# include "hardware.h"
#endif

#include "debug.h"

using namespace spiflashstore;

static constexpr uint8_t s_aSignature[] = {'A', 'v', 'V', 0x10};
static constexpr auto OFFSET_STORES	= ((((sizeof(s_aSignature) + 15) / 16) * 16) + 16); // +16 is reserved for UUID
static constexpr uint32_t s_aStorSize[static_cast<uint32_t>(Store::LAST)]  = {96,        144,       32,    64,       96,      64,     32,     32,         480,           64,        32,        96,           48,        32,      944,          48,        64,            32,        96,         32,      1024,     32,     32,       64,            96,               32,    32,          32};
#ifndef NDEBUG
static constexpr char s_aStoreName[static_cast<uint32_t>(Store::LAST)][16] = {"Network", "Art-Net3", "DMX", "WS28xx", "E1.31", "LTC", "MIDI", "not-used", "OSC Server", "TLC59711", "USB Pro", "RDM Device", "RConfig", "TCNet", "OSC Client", "Display", "LTC Display", "Monitor", "SparkFun", "Slush", "Motors", "Show", "Serial", "RDM Sensors", "RDM SubDevices", "GPS", "RGB Panel", "DDP Display"};
#endif

bool SpiFlashStore::s_bHaveFlashChip;
bool SpiFlashStore::s_bIsNew;

State SpiFlashStore::s_State;

uint32_t SpiFlashStore::s_nStartAddress;
uint32_t SpiFlashStore::s_nSpiFlashStoreSize;

uint8_t SpiFlashStore::s_SpiFlashData[FlashStore::SIZE];

SpiFlashStore *SpiFlashStore::s_pThis = nullptr;

SpiFlashStore::SpiFlashStore() {
	DEBUG_ENTRY

	assert(s_pThis == nullptr);
	s_pThis = this;

	if (spi_flash_probe(0, 0, 0) < 0) {
		DEBUG_PUTS("No SPI flash chip");
	} else {
		printf("Detected %s with sector size %d total %d bytes\n", spi_flash_get_name(), spi_flash_get_sector_size(), spi_flash_get_size());
		s_bHaveFlashChip = Init();
	}

	if (s_bHaveFlashChip) {
		s_nSpiFlashStoreSize = OFFSET_STORES;

		for (uint32_t j = 0; j < static_cast<uint32_t>(Store::LAST); j++) {
			s_nSpiFlashStoreSize += s_aStorSize[j];
		}

		DEBUG_PRINTF("OFFSET_STORES=%d, m_nSpiFlashStoreSize=%d", static_cast<int>(OFFSET_STORES), s_nSpiFlashStoreSize);

		assert(s_nSpiFlashStoreSize <= FlashStore::SIZE);
	}

	DEBUG_EXIT
}

bool SpiFlashStore::Init() {
	const auto nEraseSize = spi_flash_get_sector_size();
	assert(FlashStore::SIZE == nEraseSize);

	if (FlashStore::SIZE != nEraseSize) {
		return false;
	}

	s_nStartAddress = spi_flash_get_size() - nEraseSize;
	assert(!(s_nStartAddress % nEraseSize));

	if (s_nStartAddress % nEraseSize) {
		return false;
	}

	spi_flash_cmd_read_fast(s_nStartAddress, FlashStore::SIZE, &s_SpiFlashData);

	bool bSignatureOK = true;

	for (uint32_t i = 0; i < sizeof(s_aSignature); i++) {
		if (s_aSignature[i] != s_SpiFlashData[i]) {
			s_SpiFlashData[i] = s_aSignature[i];
			bSignatureOK = false;
		}
	}

	if (__builtin_expect(!bSignatureOK, 0)) {
		DEBUG_PUTS("No signature");

		s_bIsNew = true;

		// Clear nSetList
		for (uint32_t j = 0; j < static_cast<uint32_t>(Store::LAST); j++) {
			const auto nOffset = GetStoreOffset(static_cast<Store>(j));
			auto k = nOffset;
			s_SpiFlashData[k++] = 0x00;
			s_SpiFlashData[k++] = 0x00;
			s_SpiFlashData[k++] = 0x00;
			s_SpiFlashData[k++] = 0x00;

			// Clear rest of data
			for (; k < nOffset + s_aStorSize[j]; k++) {
				s_SpiFlashData[k] = 0xFF;
			}
		}

		s_State = State::CHANGED;

		return true;
	}

	for (uint32_t j = 0; j < static_cast<uint32_t>(Store::LAST); j++) {
		auto *pbSetList = &s_SpiFlashData[GetStoreOffset(static_cast<Store>(j))];
		if ((pbSetList[0] == 0xFF) && (pbSetList[1] == 0xFF) && (pbSetList[2] == 0xFF) && (pbSetList[3] == 0xFF)) {
			DEBUG_PRINTF("[%s]: nSetList \'FF...FF\'", s_aStoreName[j]);
			// Clear bSetList
			*pbSetList++ = 0x00;
			*pbSetList++ = 0x00;
			*pbSetList++ = 0x00;
			*pbSetList = 0x00;

			s_State = State::CHANGED;
		}
	}

	return true;
}

uint32_t SpiFlashStore::GetStoreOffset(Store tStore) {
	assert(tStore < Store::LAST);

	uint32_t nOffset = OFFSET_STORES;

	for (uint32_t i = 0; i < static_cast<uint32_t>(tStore); i++) {
		nOffset += s_aStorSize[i];
	}

	DEBUG_PRINTF("nOffset=%d", nOffset);

	return nOffset;
}

void SpiFlashStore::ResetSetList(Store tStore) {
	assert(tStore < Store::LAST);

	auto *pbSetList = &s_SpiFlashData[GetStoreOffset(tStore)];

	// Clear bSetList
	*pbSetList++ = 0x00;
	*pbSetList++ = 0x00;
	*pbSetList++ = 0x00;
	*pbSetList = 0x00;

	s_State = State::CHANGED;
}

void SpiFlashStore::Update(Store tStore, uint32_t nOffset, const void *pData, uint32_t nDataLength, uint32_t nSetList, uint32_t nOffsetSetList) {
	DEBUG_ENTRY

	if (__builtin_expect((!s_bHaveFlashChip),0)) {
		return;
	}

	DEBUG_PRINTF("[%s]:%u:%p, nOffset=%d, nDataLength=%d-%u, bSetList=0x%x, nOffsetSetList=%d", s_aStoreName[static_cast<uint32_t>(tStore)], static_cast<uint32_t>(tStore), pData, nOffset, nDataLength, static_cast<uint32_t>(s_State), nSetList, nOffsetSetList);

	assert(tStore < Store::LAST);
	assert(pData != nullptr);
	assert((nOffset + nDataLength) <= s_aStorSize[static_cast<uint32_t>(tStore)]);

	debug_dump(const_cast<void*>(pData), static_cast<uint16_t>(nDataLength));

	auto bIsChanged = false;

	const auto nBase = nOffset + GetStoreOffset(tStore);

	const auto *pSrc = static_cast<const uint8_t*>(pData);
	auto *pDst = &s_SpiFlashData[nBase];

	for (uint32_t i = 0; i < nDataLength; i++) {
		if (*pSrc != *pDst) {
			bIsChanged = true;
			*pDst = *pSrc;
		}
		pDst++;
		pSrc++;
	}

	if (bIsChanged && (s_State != State::ERASED)) {
		s_State = State::CHANGED;
	}

	if ((0 != nOffset) && (bIsChanged) && (nSetList != 0)) {
		auto *pSet = reinterpret_cast<uint32_t*>((&s_SpiFlashData[GetStoreOffset(tStore)] + nOffsetSetList));

		*pSet |= nSetList;
	}

	DEBUG_PRINTF("m_tState=%u", static_cast<uint32_t>(s_State));
	DEBUG_EXIT
}

void SpiFlashStore::Copy(Store tStore, void *pData, uint32_t nDataLength, uint32_t nOffset) {
	DEBUG_ENTRY

	if (__builtin_expect((!s_bHaveFlashChip), 0)) {
		DEBUG_EXIT
		return;
	}

	assert(tStore < Store::LAST);
	assert(pData != nullptr);
	assert((nDataLength + nOffset) <= s_aStorSize[static_cast<uint32_t>(tStore)]);

	const auto *pSet = reinterpret_cast<uint32_t*>((&s_SpiFlashData[GetStoreOffset(tStore)] + nOffset));

	DEBUG_PRINTF("*pSet=0x%x", reinterpret_cast<uint32_t>(*pSet));

	if ((__builtin_expect((s_bIsNew), 0)) || (__builtin_expect((*pSet == 0), 0))) {
		Update(tStore, nOffset, pData, nDataLength);
		DEBUG_EXIT
		return;
	}

	const auto *pSrc = const_cast<const uint8_t*>(&s_SpiFlashData[GetStoreOffset(tStore)]) + nOffset;
	auto *pDst = static_cast<uint8_t*>(pData);

	for (uint32_t i = 0; i < nDataLength; i++) {
		*pDst++ = *pSrc++;
	}

	DEBUG_EXIT
}

void SpiFlashStore::CopyTo(Store tStore, void* pData, uint32_t& nDataLength) {
	DEBUG_ENTRY

	if (__builtin_expect((tStore >= Store::LAST), 0)) {
		nDataLength = 0;
		return;
	}

	nDataLength = static_cast<uint16_t>(s_aStorSize[static_cast<uint32_t>(tStore)]);

	const auto *pSrc = const_cast<const uint8_t*>(&s_SpiFlashData[GetStoreOffset(tStore)]);
	auto *pDst = static_cast<uint8_t*>(pData);

	for (uint32_t i = 0; i < nDataLength; i++) {
		*pDst++ = *pSrc++;
	}

	DEBUG_EXIT
}

bool SpiFlashStore::Flash() {
	if (__builtin_expect((s_State == State::IDLE), 1)) {
		return false;
	}

	DEBUG_PRINTF("m_tState=%d", static_cast<uint32_t>(s_State));

	assert(s_nStartAddress != 0);

	if (s_nStartAddress == 0) {
		printf("!*! m_nStartAddress == 0 !*!\n");
		return false;
	}

	switch (s_State) {
		case State::CHANGED:
			spi_flash_cmd_erase(s_nStartAddress, FlashStore::SIZE);
			s_State = State::ERASED;
			return true;
			break;
		case State::ERASED:
			spi_flash_cmd_write_multi(s_nStartAddress, s_nSpiFlashStoreSize, &s_SpiFlashData);
			s_State = State::IDLE;
			break;
		default:
			break;
	}

//	Dump();

	return false;
}

void SpiFlashStore::Dump() {
#ifndef NDEBUG
	if (__builtin_expect((!s_bHaveFlashChip), 0)) {
		return;
	}

	const auto IsWatchDog = Hardware::Get()->IsWatchdog();

	if (IsWatchDog) {
		Hardware::Get()->WatchdogStop();
	}

	debug_dump(s_SpiFlashData, OFFSET_STORES);
	printf("\n");

	for (uint32_t j = 0; j < static_cast<uint32_t>(Store::LAST); j++) {
		printf("Store [%s]:%d\n", s_aStoreName[j], j);

		auto *p = &s_SpiFlashData[GetStoreOffset(static_cast<Store>(j))];
		debug_dump(p, static_cast<uint16_t>(s_aStorSize[j]));

		printf("\n");
	}

	if (IsWatchDog) {
		Hardware::Get()->WatchdogInit();
	}

	printf("m_tState=%d\n", static_cast<uint32_t>(s_State));
#endif
}
