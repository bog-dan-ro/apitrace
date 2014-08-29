/*********************************************************************
 *
 * Copyright (c) 2011-2013 Stephan Brumme. All rights reserved.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *********************************************************************/

#ifndef CRC32C_HPP
#define CRC32C_HPP


#include <stdlib.h>
#include <stdint.h>

#ifndef _WIN32
#include <endian.h>
#endif


extern const uint32_t crc32c_tables[8][256];


// swap endianess
static inline uint32_t
swap(uint32_t x)
{
#if defined(__GNUC__) || defined(__clang__)
  return __builtin_bswap32(x);
#else
  return (x >> 24) |
        ((x >>  8) & 0x0000FF00) |
        ((x <<  8) & 0x00FF0000) |
         (x << 24);
#endif
}


// compute CRC32C (Slicing-by-4 algorithm)
static inline uint32_t
crc32c_4bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t  crc = ~previousCrc32;
  const uint32_t* current = (const uint32_t*) data;

  // process four bytes at once (Slicing-by-4)
  while (length >= 4)
  {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    crc  = crc32c_tables[0][ one      & 0xFF] ^
           crc32c_tables[1][(one>> 8) & 0xFF] ^
           crc32c_tables[2][(one>>16) & 0xFF] ^
           crc32c_tables[3][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    crc  = crc32c_tables[0][(one>>24) & 0xFF] ^
           crc32c_tables[1][(one>>16) & 0xFF] ^
           crc32c_tables[2][(one>> 8) & 0xFF] ^
           crc32c_tables[3][ one      & 0xFF];
#endif

    length -= 4;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 3 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ crc32c_tables[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc;
}


// compute CRC32C (Slicing-by-8 algorithm)
static inline uint32_t
crc32c_8bytes(const void* data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32;
  const uint32_t* current = (const uint32_t*) data;

  // process eight bytes at once (Slicing-by-8)
  while (length >= 8)
  {
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t one = *current++ ^ swap(crc);
    uint32_t two = *current++;
    crc  = crc32c_tables[0][ two      & 0xFF] ^
           crc32c_tables[1][(two>> 8) & 0xFF] ^
           crc32c_tables[2][(two>>16) & 0xFF] ^
           crc32c_tables[3][(two>>24) & 0xFF] ^
           crc32c_tables[4][ one      & 0xFF] ^
           crc32c_tables[5][(one>> 8) & 0xFF] ^
           crc32c_tables[6][(one>>16) & 0xFF] ^
           crc32c_tables[7][(one>>24) & 0xFF];
#else
    uint32_t one = *current++ ^ crc;
    uint32_t two = *current++;
    crc  = crc32c_tables[0][(two>>24) & 0xFF] ^
           crc32c_tables[1][(two>>16) & 0xFF] ^
           crc32c_tables[2][(two>> 8) & 0xFF] ^
           crc32c_tables[3][ two      & 0xFF] ^
           crc32c_tables[4][(one>>24) & 0xFF] ^
           crc32c_tables[5][(one>>16) & 0xFF] ^
           crc32c_tables[6][(one>> 8) & 0xFF] ^
           crc32c_tables[7][ one      & 0xFF];
#endif

    length -= 8;
  }

  const uint8_t* currentChar = (const uint8_t*) current;
  // remaining 1 to 7 bytes (standard algorithm)
  while (length-- > 0)
    crc = (crc >> 8) ^ crc32c_tables[0][(crc & 0xFF) ^ *currentChar++];

  return ~crc;
}


#endif // CRC32C_HPP
