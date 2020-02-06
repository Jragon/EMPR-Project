#pragma once

#define SENSOR_ADDRESS 0x29

#define SENSOR_CMD_REG(CMD, type, addr) CMD << 7 | type << 5 | addr

#define SENSOR_ENABLE_ADDRESS 0x00
#define SENSOR_ENABLE_REG(AIEN, WEN, AEN, PON) AIEN << 4 | WEN << 3 | AEN << 1 | PON

#define SENSOR_ATIME_ADDRESS 0x01
#define SESNOR_GAIN_ADDRESS 0x0F

// these data addresses are each 16 bits long
#define SENSOR_CDATA 0x14
#define SENSOR_RDATA 0x16
#define SENSOR_GDATA 0x18
#define SENSOR_BDATA 0x1A
