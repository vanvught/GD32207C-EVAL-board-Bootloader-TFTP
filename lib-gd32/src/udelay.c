#include <stdint.h>

#include "gd32f20x_timer.h"

void udelay(uint16_t delay) {
	TIMER_CNT(TIMER5) = 0;
	while (TIMER_CNT(TIMER5) < delay)
		;
}

