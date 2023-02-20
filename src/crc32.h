#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

uint32_t crc32(uint32_t crc, const uint8_t *buf,uint32_t len);

#endif 