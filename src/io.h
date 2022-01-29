#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>

#define DS_PIN		PINA
#define DS_PORT		PORTA
#define DS_DDR		DDRA
#define DS_MASK		_BV(0)

#define DHT_PIN		PINA
#define DHT_DDR		DDRA
#define DHT_MASK	_BV(1)

void io_init(void);

void io_dpy_set_com(uint8_t com);	// 4 to disable all
void io_dpy_set_seg(uint8_t seg);

// default pa0 (DS) settings: p=0 d=0

// ds hi (during conversion):
// - call only after io_ds_pu()
// - not atomic
// - port is 0, ddr is 0
// - port 0 -> 1 (switch to pullup or high)
// - ddr 0 -> 1 (force high)
static inline void io_ds_strong(void)	{ DS_PORT |= DS_MASK; DS_DDR |= DS_MASK; }

// ds back to pullup (after conversion)
// - call only after io_ds_strong() (preferably)
// - not atomic
// - port is 1, ddr is 1
// - ddr 1 -> 0 (switch to pullup)
// - port 1 -> 0 (switch to float)
static inline void io_ds_weak(void)	{ DS_DDR &= ~DS_MASK; DS_PORT &= ~DS_MASK; }

// ds lo:
// - call only after io_ds_weak() and io_ds_pu()
// - atomic
// - port is 0
// - ddr 0 -> 1 (float -> low)
static inline void io_ds_lo(void)	{ DS_DDR |= DS_MASK; }

// ds pu (with external pullup):
// - call only after io_ds_weak() and io_ds_lo()
// - atomic
// - port is 0
// - ddr 1 -> 0 (low -> float)
static inline void io_ds_pu(void)	{ DS_DDR &= ~DS_MASK; }

// ds get:
// - call only after io_ds_pu()
// - atomic
// - false if ds line is low
// - true if ds line is high (pulled up)
static inline bool io_ds_get(void)	{ return DS_PIN & DS_MASK; }

static inline void io_dht_lo(void)	{ DHT_DDR |= DHT_MASK; }
static inline void io_dht_pu(void)	{ DHT_DDR &= ~DHT_MASK; }
static inline bool io_dht_get(void)	{ return DHT_PIN & DHT_MASK; }
