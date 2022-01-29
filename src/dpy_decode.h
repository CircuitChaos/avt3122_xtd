#pragma once

#include <inttypes.h>
#include "err.h"

void dpy_decode_err(uint8_t *segs, err_t err);
err_t dpy_decode_dht_temp(uint8_t *segs, uint16_t dht_temp);
err_t dpy_decode_dht_rh(uint8_t *segs, uint16_t dht_rh);
err_t dpy_decode_ds_temp(uint8_t *segs, uint16_t ds_temp);
