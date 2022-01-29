#include <util/atomic.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "tmr0.h"
#include "io.h"

// in Hz, remember that dht can be read only once per two seconds
#define TMR0_REFRESH_RATE	0.5

// in timer ticks
#define TMR0_REFRESH_TICKS	(uint16_t) (8000000L / 256 / 64 / TMR0_REFRESH_RATE)

static volatile uint16_t g_ticks;
static volatile uint8_t g_digits[4];
static volatile bool g_blocked = false;

ISR(TIMER0_OVF_vect)
{
	static uint8_t dpy_index = 0;

	if (!g_blocked)
	{
		io_dpy_set_com(4);
		io_dpy_set_seg(g_digits[dpy_index]);
		io_dpy_set_com(dpy_index);

		++dpy_index;
		dpy_index &= 3;
	}

	if (g_ticks < TMR0_REFRESH_TICKS)
		++g_ticks;
}

void tmr0_init(void)
{
	// normal mode, prescale by 64
	TCCR0A	= 0;
	TCCR0B	= _BV(CS01) | _BV(CS00);
	TIMSK	|= _BV(TOIE0);
}

bool tmr0_need_refresh(void)
{
	// synchronization not needed, because this operation doesn't need to
	// be atomic and compare-and-increment in ISR will be atomic

	if (g_ticks != TMR0_REFRESH_TICKS)
		return false;

	g_ticks = 0;
	return true;
}

void tmr0_update_dpy(const uint8_t digits[4])
{
	// synchronization needed, because memcpy can be interrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		uint8_t i;

		for (i = 0; i < 4; ++i)
			g_digits[i] = digits[i];
	}
}

void tmr0_block(void)
{
	g_blocked = true;
}

void tmr0_unblock(void)
{
	g_blocked = false;
}
