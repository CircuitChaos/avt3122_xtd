#pragma once

// use hex, but only 0...9 (display doesn't display a...f due to memory limitation)

typedef enum
{
	ERR_OK		= 0,

	// DS reader
	ERR_DS_NOTYET		= 0x10,
	ERR_DS_RST_1		= 0x11,
	ERR_DS_RST_2		= 0x12,
	ERR_DS_RST_3		= 0x13,
	ERR_DS_CRC		= 0x14,

	// DS to 7seg decoder
	ERR_DS_TOO_LOW		= 0x20,
	ERR_DS_TOO_HIGH		= 0x21,

	// DHT reader
	ERR_DHT_FIRST_LOW	= 0x30,
	ERR_DHT_FIRST_HIGH	= 0x31,
	ERR_DHT_SECOND_LOW	= 0x32,
	ERR_DHT_DATA_HIGH	= 0x33,
	ERR_DHT_DATA_LOW	= 0x34,
	ERR_DHT_FINAL_HIGH	= 0x35,
	ERR_DHT_CHECKSUM	= 0x36,

	// DHT to 7seg decoder
	ERR_DHT_RH_TOO_HIGH	= 0x40,
	ERR_DHT_TEMP_TOO_LOW	= 0x41,
	ERR_DHT_TEMP_TOO_HIGH	= 0x42,
	// ...
} err_t;
