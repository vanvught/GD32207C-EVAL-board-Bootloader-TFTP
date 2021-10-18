/**
 * @file storeslushdmx.h
 *
 */
/* Copyright (C) 2019-2021 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef STORESLUSHDMX_H_
#define STORESLUSHDMX_H_

#include "slushdmxparams.h"

class StoreSlushDmx final: public SlushDmxParamsStore {
public:
	StoreSlushDmx();

	void Update(const struct TSlushDmxParams *pSlushDmxParams) override;
	void Copy(struct TSlushDmxParams *pSlushDmxParams) override;

	void Update(uint32_t nMotorIndex, const struct TSlushDmxParams *ptSlushDmxParams) override;
	void Copy(uint32_t nMotorIndex, struct TSlushDmxParams *ptSlushDmxParams) override;

	static StoreSlushDmx *Get() {
		return s_pThis;
	}

private:
	static StoreSlushDmx *s_pThis;
};

#endif /* STORESLUSHDMX_H_ */
