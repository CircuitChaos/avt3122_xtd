#include <stdbool.h>
#include <util/atomic.h>
#include "tmr1.h"
#include "ds.h"
#include "io.h"

/*
 * tslot	= 60...120 us
 * trec		= 1 us ...
 * tlow0	= 60 ... 120 us
 * tlow1	= 1 ... 15 us
 * trdv		= ... 15 us
 * trsth	= 480 us ...
 * trstl	= 480 us ...
 * tpdhigh	= 15 ... 60 us
 * tpdlow	= 60 ... 240 us
 */

static const uint8_t CMD_READ_ROM	= 0x33;
static const uint8_t CMD_MATCH		= 0x55;
static const uint8_t CMD_SEARCH		= 0xf0;
static const uint8_t CMD_SKIP		= 0xcc;
static const uint8_t CMD_CONVERT	= 0x44; // strong before 10us, wait till conv complete
static const uint8_t CMD_COPY		= 0x48; // strong before 10us, wait 10ms
static const uint8_t CMD_WRITE		= 0x4e;
static const uint8_t CMD_READ		= 0xbe;

static bool g_was_conv = false;

static void wait(uint8_t units)
{
	tmr1_start();
	while (units--)
		while (!tmr1_tick()) ;
	tmr1_stop();
}

static void wr0(void)
{
	// 90 us: low (tlow1)
	// 10 us: fl (trec)

	io_ds_lo();
	wait(9);

	io_ds_pu();
	wait(1);
}

static bool wr1(void)
{
	// 10 us: low (tlow1)
	// 10 us: pu
	// sample
	// 70 us: recovery

	bool rs;

	io_ds_lo();
	wait(1);

	io_ds_pu();
	wait(1);

	rs = io_ds_get();
	wait(7);

	return rs;
}

static bool rst(void)
{
	bool presence;

	io_ds_lo();
	wait(55);	// 550 us

	io_ds_pu();
	wait(9);	// wait 90 us (tpdhigh)

	presence = io_ds_get();
	wait(46);	// wait another 460 us

	return !presence;
}

static void wr(uint8_t byte)
{
	uint8_t i;

	for (i = 0; i < 8; ++i)
		if (byte & _BV(i))
			wr1();
		else
			wr0();
}

static uint8_t rd(void)
{
	uint8_t rs = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i)
		if (wr1())
			rs |= _BV(i);

	return rs;
}

/*
static uint8_t crc(uint8_t crc, uint8_t data)
{
	uint8_t i;

	crc = crc ^ data;

	for (i = 0; i < 8; ++i)
		if (crc & 1)
			crc = (crc >> 1) ^ 0x8c;
		else
			crc >>= 1;

	return crc;
}
*/

err_t ds_read(uint16_t *ds_temp)
{
	uint8_t scratchpad[9];
	uint8_t i;
	// uint8_t calc_crc = 0;
	bool valid = false;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		io_ds_weak();
	}

	if (!rst())
		return ERR_DS_RST_1;

	wr(CMD_SKIP);
	wr(CMD_READ);

	for (i = 0; i < 9; ++i)
		scratchpad[i] = rd();

	if (!rst())
		return ERR_DS_RST_2;

	// skip crc calculation, because one of my sensors keeps returning nonsense in
	// crc field, but otherwise accurate data...
	//
	// 0x0a, 0x00, 0x4b, 0x46, 0x7f, 0xff, 0x46, 0x10, 0x83
	// 0x0c, 0x00, 0x4b, 0x46, 0x7f, 0xff, 0x44, 0x10, 0x81
	// 0x0d, 0x00, 0x4b, 0x46, 0x7f, 0xff, 0x43, 0x10, 0xac
	// 0x0e, 0x00, 0x4b, 0x46, 0x7f, 0x00, 0x42, 0x10, 0xad	// actually, here [5] is 00...
	//
	// for (i = 0; i < 8; ++i)
	//	calc_crc = crc(calc_crc, scratchpad[i]);
	//
	// if (calc_crc != scratchpad[8])
	//	return ERR_DS_CRC;

	if (scratchpad[4] != 0x7f)
	{
		wr(CMD_SKIP);
		wr(CMD_WRITE);
		wr(0);
		wr(0);
		wr(0x7f);

		if (!rst())
			return ERR_DS_RST_3;
	}
	else if (g_was_conv)
		valid = true;

	wr(CMD_SKIP);
	wr(CMD_CONVERT);
	g_was_conv = true;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		io_ds_strong();
	}

	if (!valid)
		return ERR_DS_NOTYET;

	*ds_temp = scratchpad[0] | (scratchpad[1] << 8);
	return ERR_OK;
}
