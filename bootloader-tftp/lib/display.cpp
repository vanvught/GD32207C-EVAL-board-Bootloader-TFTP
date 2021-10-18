/**
 * @file display.cpp
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

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "displayset.h"
#include "display.h"

#include "display7segment.h"
#include "hardware.h"
#include "console.h"

#define UNUSED __attribute__((unused))

using namespace display;

Display *Display::s_pThis = nullptr;

Display::Display() : m_nMillis(0) {
	assert(s_pThis == nullptr);
	s_pThis = this;
}

Display::Display(uint8_t nCols, uint8_t nRows) : m_nMillis(0) {
	assert(s_pThis == nullptr);
	s_pThis = this;
}

Display::Display(DisplayType tDisplayType) : m_tType(tDisplayType), m_nMillis(0) {
	assert(s_pThis == nullptr);
	s_pThis = this;
}

void Display::Detect(DisplayType tDisplayType) {
}

void Display::Detect( uint8_t nCols, uint8_t nRows) {
}

int Display::Printf(uint8_t nLine, const char *format, ...) {
	return 0;
}

int Display::Write(uint8_t nLine, const char *pText) {
	return 0;
}

void Display::SetCursorPos(uint8_t nCol, uint8_t nRow) {
}

#if defined(ENABLE_CURSOR_MODE)
# define UNUSED
#else
# define UNUSED __attribute__((unused))
#endif

void Display::SetCursor(UNUSED uint32_t nMode) {
#if defined(ENABLE_CURSOR_MODE)
	if (m_LcdDisplay == nullptr) {
		return;
	}

	m_LcdDisplay->SetCursor(nMode);
#endif
}

void Display::TextStatus(const char *pText) {
}

void Display::TextStatus(const char *pText, Display7SegmentMessage n7SegmentData, __attribute__((unused)) uint32_t nConsoleColor) {
	TextStatus(pText);
	m_Display7Segment.Status(n7SegmentData);

	if (nConsoleColor == UINT32_MAX) {
		return;
	}

	console_status(nConsoleColor, pText);
}

void Display::TextStatus(const char *pText, uint8_t nValue7Segment, bool bHex) {
	TextStatus(pText);
	m_Display7Segment.Status(nValue7Segment, bHex);
}

void Display::PrintInfo() {
	puts("No display found");
}

void Display::SetSleep(bool bSleep) {
}

void Display::Run() {
}
