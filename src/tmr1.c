#include <avr/interrupt.h>
#include <avr/io.h>
#include "tmr1.h"

static volatile bool g_tick;

ISR(TIMER1_COMPA_vect)
{
	g_tick = true;
}

void tmr1_init(void)
{
	// WGM is CTC (0100)
	// CS is clk/1 (001)

	OCR1A	= 80;	// 8 MHz / 100 kHz
	TCCR1A	= 0;
	TCCR1B	= _BV(WGM12) | _BV(CS10);
	TCCR1C	= 0;
}

void tmr1_start(void)
{
	TCNT1	= 0;
	TIMSK	|= _BV(OCIE1A);
}

void tmr1_stop(void)
{
	TIMSK	&= ~_BV(OCIE1A);
}

bool tmr1_tick(void)
{
	if (!g_tick)
		return false;

	g_tick = false;
	return true;
}
