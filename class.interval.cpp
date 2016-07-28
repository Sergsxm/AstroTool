/*
 * class.interval.cpp
 * Interval class source file
 *
 * Project: AstroTool
 * Created on: 28.07.2016
 * Author: SergSXM (sergsxm@embedded.by)
 */

#include "class.interval.h"

#define	INTERVAL_TCNT	((256 - (F_CPU / INTERVAL_FREQ / 64)) & 0xFF)

Interval * Interval::instance;

Interval::Interval(void) {
	instance = this;
	for (uint8_t i = 0; i < INTERVAL_LISTENERS_COUNT; i++) {
		listenerInterval[i] = 0;
	}
	TCCR2 = 0x04;
	TCNT2 = INTERVAL_TCNT;
	TIMSK |= 1 << 6;
	sei();
}

void Interval::InterruptHandler(void) {
	TCNT2 = INTERVAL_TCNT;
	for (uint8_t i = 0; i < INTERVAL_LISTENERS_COUNT; i++) {
		if (listenerInterval[i] == 0) {
			continue;
		}
		listenerCounter[i]++;
		if (listenerCounter[i] >= listenerInterval[i]) {
			listenerCounter[i] = 0;
			listeners[i]->IntervalHandler();
		}
	}
}

void Interval::addListener(IntervalListener * listener, uint16_t frequency) {
	uint8_t i = 0;
	for (; i < INTERVAL_LISTENERS_COUNT; i++) {
		if (listenerInterval[i] == 0) {
			break;
		}
	}
	listenerCounter[i] = 0;
	listeners[i] = listener;
	listenerInterval[i] = INTERVAL_FREQ / frequency;
	if (listenerInterval[i] < 1) {
		listenerInterval[i] = 1;
	}
}


ISR(TIMER2_OVF_vect) {
	Interval::instance->InterruptHandler();
}


