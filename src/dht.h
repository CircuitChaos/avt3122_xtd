#pragma once

#include <inttypes.h>
#include "err.h"

// blocks timer 0 internally
err_t dht_read(uint16_t *dht_temp, uint16_t *dht_rh);
