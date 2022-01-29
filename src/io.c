#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "io.h"

void io_init(void)
{
	PORTA	= 0xfc;
	DDRA	= 0x00;
	PORTB	= 0xe1;
	DDRB	= 0x1f;
	PORTD	= 0xff;
	DDRD	= 0x7f;
}

void io_dpy_set_seg(uint8_t seg)
{
	static const uint8_t tbl[] PROGMEM = { ~_BV(6), ~_BV(3), ~_BV(1), ~_BV(0), ~_BV(2), ~_BV(5), ~_BV(4) };
	uint8_t pd = PORTD | (_BV(0) | _BV(1) | _BV(2) | _BV(4) | _BV(5) | _BV(6) | _BV(3));
	uint8_t i;

	for (i = 0; i < 7; ++i)
		if (seg & _BV(i))
			pd &= pgm_read_byte_near(tbl + i);

	if (seg & 0x80)
		PORTB &= ~_BV(0);
	else
		PORTB |= _BV(0);

	PORTD = pd;
}

void io_dpy_set_com(uint8_t com)
{
	static const uint8_t tbl[] PROGMEM = { ~_BV(3), ~_BV(4), ~_BV(1), ~_BV(2), 0xff };
	PORTB = (PORTB | _BV(1) | _BV(2) | _BV(3) | _BV(4)) & pgm_read_byte_near(tbl + com);
}
