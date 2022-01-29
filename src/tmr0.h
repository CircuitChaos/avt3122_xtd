#pragma once

#include <stdbool.h>

// timer resolution: 8e6 / 256 / 64 = 488.28125 Hz (2.048 ms)
//
// display is refreshed in every tick, so full cycle (4 digits)
// takes 8.192 ms (122.0703125 Hz)
//
// remember that timer changes ports to update the display, so
// all other non-atomic operations on ports have to be in atomic blocks

void tmr0_init(void);

// at TMR_REFRESH_RATE [Hz], defined in tmr.c, tmr_need_refresh()
// returns true and main loop switches display and rereads data
bool tmr0_need_refresh(void);

// enables interrupts due to ATOMIC_FORCEON; change to ATOMIC_RESTORESTATE
// if this is not desirable
void tmr0_update_dpy(const uint8_t digits[4]);

// blocks and unblocks tim0 refresh to minimize delays in dht handling
void tmr0_block(void);
void tmr0_unblock(void);
