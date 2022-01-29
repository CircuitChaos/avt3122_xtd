#include <avr/pgmspace.h>
#include "dpy_decode.h"

static const uint8_t CODE_R	= 0x50;
static const uint8_t CODE_H_LC	= 0x74;
static const uint8_t CODE_H_UC	= 0x76;
static const uint8_t CODE_E	= 0x79;
static const uint8_t CODE_SP	= 0x00;
static const uint8_t CODE_DASH	= 0x40;
static const uint8_t CODE_DOT	= 0x80;	// can be OR-ed with other codes

static uint8_t nibble(uint8_t value)
{
	static const uint8_t tbl[] PROGMEM =
	{
		0x3f,	// 0
		0x06,	// 1
		0x5b,	// 2
		0x4f,	// 3
		0x66,	// 4
		0x6d,	// 5
		0x7d,	// 6
		0x07,	// 7
		0x7f,	// 8
		0x6f,	// 9
//		0x77,	// a
//		0x7c,	// b
//		0x39,	// c
//		0x5e,	// d
//		0x79,	// e
//		0x71,	// f
	};

	return pgm_read_byte_near(tbl + value);
}

static void copy_2digit_decimal(uint8_t *buf, uint8_t dec)
{
	buf[0] = nibble(dec / 10);
	buf[1] = nibble(dec % 10);
}

void dpy_decode_err(uint8_t *segs, err_t err)
{
	if (err == ERR_DS_NOTYET)
	{
		// special code, because it occurs during startup
		// and is normal
		segs[0] = segs[1] = segs[2] = segs[3] = CODE_DOT;
	}
	else
	{
		segs[0] = CODE_E;
		segs[1] = CODE_R;
		segs[2] = nibble(err >> 4);
		segs[3] = nibble(err & 0x0f);
	}
}

err_t dpy_decode_dht_temp(uint8_t *segs, uint16_t dht_temp)
{
	// example: 0x015f & 0x7fff = 351 = 35.1
	// example: 0x8065 & 0x7fff = 101 = -10.1
	uint8_t dec, frac;

	// we don't support minus at the moment due to lack of flash space
	if (dht_temp & 0x8000)
		return ERR_DHT_TEMP_TOO_LOW;
	else if (dht_temp > 999)
		return ERR_DHT_TEMP_TOO_HIGH;

	dec = dht_temp / 10;
	frac = dht_temp % 10;

	segs[0] = CODE_H_UC;

	if (dec < 10)
	{
		segs[1] = CODE_SP;
		segs[2] = nibble(dec);
	}
	else
		copy_2digit_decimal(segs + 1, dec);

	segs[2] |= CODE_DOT;
	segs[3] = nibble(frac);

	return ERR_OK;
}

err_t dpy_decode_dht_rh(uint8_t *segs, uint16_t dht_rh)
{
	// example: 0x028c = 652 = 65.2%

	uint8_t dec;

	dec = dht_rh / 10;
	if (dec > 100)
		return ERR_DHT_RH_TOO_HIGH;
	else if (dec == 100)
		dec = 99;

	if (dec < 10)
	{
		segs[0] = nibble(dec);
		segs[1] = CODE_SP;
	}
	else
		copy_2digit_decimal(segs, dec);

	segs[2] = CODE_R;
	segs[3] = CODE_H_LC;

	return ERR_OK;
}

err_t dpy_decode_ds_temp(uint8_t *segs, uint16_t ds_temp)
{
	static const uint8_t fractbl[] PROGMEM =
	{
		0,	// 0
		1,	// 0.625
		1,	// 1.25
		2,	// 1.875
		3,	// 2.5
		3,	// 3.125
		4,	// 3.75
		4,	// 4.375
		5,	// 5
		6,	// 5.625
		6,	// 6.250
		7,	// 6.875
		8,	// 7.5
		8,	// 8.125
		9,	// 8.75
		9	// 9.375
	};

	uint8_t integral;
	uint8_t sign = CODE_SP;

	if (ds_temp & 0x8000)
	{
		ds_temp = (ds_temp ^ 0xffff) + 1;
		sign = CODE_DASH;
	}

	// obtain integral part
	integral = ds_temp >> 4;

	// integral part normal range:
	// neg	0...55
	// !neg	0...125, but we'll stick to 0...99 for smaller code
	if (sign == CODE_DASH)
	{
		if (integral > 55)
			return ERR_DS_TOO_LOW;
	}
	else
	{
		if (integral > 99)
			return ERR_DS_TOO_HIGH;
	}

	if (integral < 10)
	{
		// format: <space> <sign> <integral>. <fractional>
		segs[0] = CODE_SP;
		segs[1] = sign;
		segs[2] = nibble(integral);
	}
	else
	{
		// format: <sign> <integral / 10> <integral % 10>. <fractional>
		segs[0] = sign;
		copy_2digit_decimal(segs + 1, integral);
	}

	segs[2] |= CODE_DOT;
	segs[3] = nibble(pgm_read_byte_near(fractbl + (ds_temp & 0x0f)));
	return ERR_OK;
}
