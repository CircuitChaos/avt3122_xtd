#include "dht.h"
#include "tmr1.h"
#include "tmr0.h"
#include "io.h"

#define US_PER_TICK	10
#define WAIT_TIMEOUT	255

// shortcut to use us instead of ticks, but remember that
// it still returns ticks, not us
#define wait(state, us) wait2(state, (us) / US_PER_TICK)

static uint8_t wait2(bool state, uint8_t max_ticks)
{
	uint8_t ticks;

	tmr1_start();

	for (ticks = 0; ticks < max_ticks; ++ticks)
	{
		while (!tmr1_tick()) ;

		if (io_dht_get() == state)
			break;
	}

	tmr1_stop();

	return (ticks != max_ticks) ? ticks : WAIT_TIMEOUT;
}

static uint8_t read2(uint16_t *dht_temp, uint16_t *dht_rh)
{
	uint8_t i;
	uint8_t data[5] = { 0, 0, 0, 0, 0 };
	uint8_t ofs = 0;
	uint8_t mask = 0x80;

	// dht:
	// - waits 20-40 us
	// - sets low
	// - waits 80 us
	// - sets high
	// - waits 80 us
	// - starts sending data:
	//   - sets low
	//   - waits 50 us
	//   - sets high
	//   - waits 26-28 us for 0 or 70 us for 1

	// let's wait up to 60 us for output to go low
	// (expected in 20-40 us)
	if (wait(false, 60) == WAIT_TIMEOUT)
		return ERR_DHT_FIRST_LOW;

	// let's wait up to 100 us for output to go high
	// (expected in 80 us)
	if (wait(true, 100) == WAIT_TIMEOUT)
		return ERR_DHT_FIRST_HIGH;

	// let's wait up to 100 us for output to go low
	// (expected in 80 us)
	if (wait(false, 100) == WAIT_TIMEOUT)
		return ERR_DHT_SECOND_LOW;

	for (i = 0; i < 40; ++i)
	{
		uint8_t rs;

		// let's wait up to 100 us for output to go high
		// (expected in 50 us)
		if (wait(true, 100) == WAIT_TIMEOUT)
			return ERR_DHT_DATA_HIGH;

		// let's wait up to 100 us for output to go low,
		// but monitoring the time
		//
		// expected in 26-28 us or 70 us
		rs = wait(false, 100);

		// 10-40 us -- we assume 0
		// 50-100 us -- we assume 1
		if (rs == WAIT_TIMEOUT)
			return ERR_DHT_DATA_LOW;

		if (rs >= (40 / US_PER_TICK))
			data[ofs] |= mask;

		mask >>= 1;
		if (mask == 0)
		{
			++ofs;
			mask = 0x80;
		}
	}

	// wait for data to go high, up to 100 us
	if (wait(true, 100) == WAIT_TIMEOUT)
		return ERR_DHT_FINAL_HIGH;

	if (((data[0] + data[1] + data[2] + data[3]) & 0xff) != data[4])
		return ERR_DHT_CHECKSUM;

	*dht_temp = (data[2] << 8) | data[3];
	*dht_rh = (data[0] << 8) | data[1];
	return ERR_OK;
}

err_t dht_read(uint16_t *dht_temp, uint16_t *dht_rh)
{
	err_t err;

	io_dht_lo();
	wait(true, 2500);
	tmr0_block();
	io_dht_pu();

	err = read2(dht_temp, dht_rh);
	tmr0_unblock();

	return err;
}
