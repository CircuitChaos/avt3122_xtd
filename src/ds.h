#pragma once

#include <inttypes.h>
#include "err.h"

// - ds_temp is in DS format
// - first read is meaningless
err_t ds_read(uint16_t *ds_temp);
