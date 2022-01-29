#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include "io.h"
#include "tmr0.h"
#include "tmr1.h"
#include "err.h"
#include "ds.h"
#include "dht.h"
#include "dpy_decode.h"

static void refresh(void)
{
	enum state_t
	{
		ST_DS,
		ST_DHT_TEMP,
		ST_DHT_RH,
	};

	static enum state_t state = ST_DS;
	static uint16_t dht_rh;
	uint8_t segs[4];
	err_t err = ERR_OK;

	switch (state)
	{
		case ST_DHT_TEMP:
		{
			uint16_t dht_temp;

			err = dht_read(&dht_temp, &dht_rh);
			if (err != ERR_OK)
			{
				state = ST_DS;
				break;
			}

			err = dpy_decode_dht_temp(segs, dht_temp);
			state = ST_DHT_RH;
			break;
		}

		case ST_DHT_RH:
			err = dpy_decode_dht_rh(segs, dht_rh);
			state = ST_DS;
			break;

		case ST_DS:
		{
			uint16_t ds_temp;

			err = ds_read(&ds_temp);
			if (err == ERR_OK)
				err = dpy_decode_ds_temp(segs, ds_temp);

			state = ST_DHT_TEMP;
			break;
		}
	}

	if (err != ERR_OK)
		dpy_decode_err(segs, err);

	tmr0_update_dpy(segs);
}

int main(void)
{
	io_init();
	tmr0_init();
	tmr1_init();
	wdt_enable(WDTO_15MS);
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei();

	for (;;)
	{
		wdt_reset();
		sleep_mode();

		if (!tmr0_need_refresh())
			continue;

		refresh();
	}

	/* NOTREACHED */
	return 0;
}
