/*
**  CRC.H - header file for SNIPPETS CRC and checksum functions
*/

#pragma once

#include <stdint.h> 

#define UPDC32(octet,crc) (crc_32_tab[((crc)\
     ^ ((uint8_t)octet)) & 0xff] ^ ((crc) >> 8))

uint32_t updateCRC32(unsigned char ch, uint32_t crc);
uint32_t crc32buf(char* buf, size_t len);

