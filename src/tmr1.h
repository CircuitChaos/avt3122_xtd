#pragma once

#include <stdbool.h>
#include <inttypes.h>

// timer 1 tick frequency is 100 kHz (period is 10 us)
// it's used in ds and dht timing

void tmr1_init(void);
void tmr1_start(void);
void tmr1_stop(void);
bool tmr1_tick(void);
