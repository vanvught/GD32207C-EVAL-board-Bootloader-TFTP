/**
 * @file flashcodeinstall.cpp
 *
 */
/* Copyright (C) 2021-2022 by Arjan van Vught mailto:info@gd32-dmx.org
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

#if defined(__clang__)
# pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

#include <cassert>

#include "flashcodeinstall.h"

#include "display.h"

#include "debug.h"

FlashCodeInstall *FlashCodeInstall::s_pThis = nullptr;

FlashCodeInstall::FlashCodeInstall() {
	DEBUG_ENTRY

	assert(s_pThis == 0);
	s_pThis = this;

	Display::Get()->Cls();

	m_nFlashSize = FlashCode::GetSize();

	printf("%s, sector size %d, %d bytes [%d kB]\n", FlashCode::GetName(), FlashCode::GetSectorSize(), m_nFlashSize, m_nFlashSize / 1024U);
	Display::Get()->Write(1, FlashCode::GetName());

	DEBUG_EXIT
}

FlashCodeInstall::~FlashCodeInstall() {
	DEBUG_ENTRY
	DEBUG_EXIT
}

void FlashCodeInstall::Process(__attribute__((unused)) const char *pFileName, __attribute__((unused)) uint32_t nOffset) {
	DEBUG_ENTRY
	assert(0);
	DEBUG_EXIT
}

bool FlashCodeInstall::Open(__attribute__((unused)) const char *pFileName) {
	DEBUG_ENTRY
	assert(0);
	DEBUG_EXIT
	return false;
}

void FlashCodeInstall::Close() {
	DEBUG_ENTRY
	assert(0);
	DEBUG_EXIT
}

bool FlashCodeInstall::BuffersCompare(__attribute__((unused)) uint32_t nSize) {
	DEBUG_ENTRY
	assert(0);
	DEBUG_EXIT
	return false;
}

bool FlashCodeInstall::Diff(__attribute__((unused)) uint32_t nOffset) {
	DEBUG_ENTRY
	assert(0);
	DEBUG_EXIT
	return false;
}

void FlashCodeInstall::Write(__attribute__((unused)) uint32_t nOffset) {
	DEBUG_ENTRY
	assert(0);
	DEBUG_EXIT
}

