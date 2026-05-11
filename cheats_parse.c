/* gameplaySP - Cheat format parsing and decryption (ported from mgba)
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 * Portions Copyright (C) 2013-2016 Jeffrey Pfau (mgba)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "cheats.h"
#include "gba_memory.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ================================================================
 *  GameShark / PARv3 shared TEA decryption
 * ================================================================ */

const u32 GSSeeds[4] = { 0x09F4FBBD, 0x9681884A, 0x352027E9, 0xF3DEE5A7 };
const u32 PAR3Seeds[4] = { 0x7AA9648F, 0x7FAE6994, 0xC0EFAAD5, 0x42712C57 };

/* GSAv1 lookup tables */
static const u8 _gsa1T1[256] = {
   0x31,0x1C,0x23,0xE5,0x89,0x8E,0xA1,0x37,0x74,0x6D,0x67,0xFC,0x1F,0xC0,0xB1,0x94,
   0x3B,0x05,0x56,0x86,0x00,0x24,0xF0,0x17,0x72,0xA2,0x3D,0x1B,0xE3,0x17,0xC5,0x0B,
   0xB9,0xE2,0xBD,0x58,0x71,0x1B,0x2C,0xFF,0xE4,0xC9,0x4C,0x5E,0xC9,0x55,0x33,0x45,
   0x7C,0x3F,0xB2,0x51,0xFE,0x10,0x7E,0x75,0x3C,0x90,0x8D,0xDA,0x94,0x38,0xC3,0xE9,
   0x95,0xEA,0xCE,0xA6,0x06,0xE0,0x4F,0x3F,0x2A,0xE3,0x3A,0xE4,0x43,0xBD,0x7F,0xDA,
   0x55,0xF0,0xEA,0xCB,0x2C,0xA8,0x47,0x61,0xA0,0xEF,0xCB,0x13,0x18,0x20,0xAF,0x3E,
   0x4D,0x9E,0x1E,0x77,0x51,0xC5,0x51,0x20,0xCF,0x21,0xF9,0x39,0x94,0xDE,0xDD,0x79,
   0x4E,0x80,0xC4,0x9D,0x94,0xD5,0x95,0x01,0x27,0x27,0xBD,0x6D,0x78,0xB5,0xD1,0x31,
   0x6A,0x65,0x74,0x74,0x58,0xB3,0x7C,0xC9,0x5A,0xED,0x50,0x03,0xC4,0xA2,0x94,0x4B,
   0xF0,0x58,0x09,0x6F,0x3E,0x7D,0xAE,0x7D,0x58,0xA0,0x2C,0x91,0xBB,0xE1,0x70,0xEB,
   0x73,0xA6,0x9A,0x44,0x25,0x90,0x16,0x62,0x53,0xAE,0x08,0xEB,0xDC,0xF0,0xEE,0x77,
   0xC2,0xDE,0x81,0xE8,0x30,0x89,0xDB,0xFE,0xBC,0xC2,0xDF,0x26,0xE9,0x8B,0xD6,0x93,
   0xF0,0xCB,0x56,0x90,0xC0,0x46,0x68,0x15,0x43,0xCB,0xE9,0x98,0xE3,0xAF,0x31,0x25,
   0x4D,0x7B,0xF3,0xB1,0x74,0xE2,0x64,0xAC,0xD9,0xF6,0xA0,0xD5,0x0B,0x9B,0x49,0x52,
   0x69,0x3B,0x71,0x00,0x2F,0xBB,0xBA,0x08,0xB1,0xAE,0xBB,0xB3,0xE1,0xC9,0xA6,0x7F,
   0x17,0x97,0x28,0x72,0x12,0x6E,0x91,0xAE,0x3A,0xA2,0x35,0x46,0x27,0xF8,0x12,0x50
};

static const u8 _gsa1T2[256] = {
   0xD8,0x65,0x04,0xC2,0x65,0xD5,0xB0,0x0C,0xDF,0x9D,0xF0,0xC3,0x9A,0x17,0xC9,0xA6,
   0xE1,0xAC,0x0D,0x14,0x2F,0x3C,0x2C,0x87,0xA2,0xBF,0x4D,0x5F,0xAC,0x2D,0x9D,0xE1,
   0x0C,0x9C,0xE7,0x7F,0xFC,0xA8,0x66,0x59,0xAC,0x18,0xD7,0x05,0xF0,0xBF,0xD1,0x8B,
   0x35,0x9F,0x59,0xB4,0xBA,0x55,0xB2,0x85,0xFD,0xB1,0x72,0x06,0x73,0xA4,0xDB,0x48,
   0x7B,0x5F,0x67,0xA5,0x95,0xB9,0xA5,0x4A,0xCF,0xD1,0x44,0xF3,0x81,0xF5,0x6D,0xF6,
   0x3A,0xC3,0x57,0x83,0xFA,0x8E,0x15,0x2A,0xA2,0x04,0xB2,0x9D,0xA8,0x0D,0x7F,0xB8,
   0x0F,0xF6,0xAC,0xBE,0x97,0xCE,0x16,0xE6,0x31,0x10,0x60,0x16,0xB5,0x83,0x45,0xEE,
   0xD7,0x5F,0x2C,0x08,0x58,0xB1,0xFD,0x7E,0x79,0x00,0x34,0xAD,0xB5,0x31,0x34,0x39,
   0xAF,0xA8,0xDD,0x52,0x6A,0xB0,0x60,0x35,0xB8,0x1D,0x52,0xF5,0xF5,0x30,0x00,0x7B,
   0xF4,0xBA,0x03,0xCB,0x3A,0x84,0x14,0x8A,0x6A,0xEF,0x21,0xBD,0x01,0xD8,0xA0,0xD4,
   0x43,0xBE,0x23,0xE7,0x76,0x27,0x2C,0x3F,0x4D,0x3F,0x43,0x18,0xA7,0xC3,0x47,0xA5,
   0x7A,0x1D,0x02,0x55,0x09,0xD1,0xFF,0x55,0x5E,0x17,0xA0,0x56,0xF4,0xC9,0x6B,0x90,
   0xB4,0x80,0xA5,0x07,0x22,0xFB,0x22,0x0D,0xD9,0xC0,0x5B,0x08,0x35,0x05,0xC1,0x75,
   0x4F,0xD0,0x51,0x2D,0x2E,0x5E,0x69,0xE7,0x3B,0xC2,0xDA,0xFF,0xF6,0xCE,0x3E,0x76,
   0xE8,0x36,0x8C,0x39,0xD8,0xF3,0xE9,0xA6,0x42,0xE6,0xC1,0x4C,0x05,0xBE,0x17,0xF2,
   0x5C,0x1B,0x19,0xDB,0x0F,0xF3,0xF8,0x49,0xEB,0x36,0xF6,0x40,0x6F,0xAD,0xC1,0x8C
};

/* PARv3 lookup tables */
static const u8 _par3T1[256] = {
   0xD0,0xFF,0xBA,0xE5,0xC1,0xC7,0xDB,0x5B,0x16,0xE3,0x6E,0x26,0x62,0x31,0x2E,0x2A,
   0xD1,0xBB,0x4A,0xE6,0xAE,0x2F,0x0A,0x90,0x29,0x90,0xB6,0x67,0x58,0x2A,0xB4,0x45,
   0x7B,0xCB,0xF0,0x73,0x84,0x30,0x81,0xC2,0xD7,0xBE,0x89,0xD7,0x4E,0x73,0x5C,0xC7,
   0x80,0x1B,0xE5,0xE4,0x43,0xC7,0x46,0xD6,0x6F,0x7B,0xBF,0xED,0xE5,0x27,0xD1,0xB5,
   0xD0,0xD8,0xA3,0xCB,0x2B,0x30,0xA4,0xF0,0x84,0x14,0x72,0x5C,0xFF,0xA4,0xFB,0x54,
   0x9D,0x70,0xE2,0xFF,0xBE,0xE8,0x24,0x76,0xE5,0x15,0xFB,0x1A,0xBC,0x87,0x02,0x2A,
   0x58,0x8F,0x9A,0x95,0xBD,0xAE,0x8D,0x0C,0xA5,0x4C,0xF2,0x5C,0x7D,0xAD,0x51,0xFB,
   0xB1,0x22,0x07,0xE0,0x29,0x7C,0xEB,0x98,0x14,0xC6,0x31,0x97,0xE4,0x34,0x8F,0xCC,
   0x99,0x56,0x9F,0x78,0x43,0x91,0x85,0x3F,0xC2,0xD0,0xD1,0x80,0xD1,0x77,0xA7,0xE2,
   0x43,0x99,0x1D,0x2F,0x8B,0x6A,0xE4,0x66,0x82,0xF7,0x2B,0x0B,0x65,0x14,0xC0,0xC2,
   0x1D,0x96,0x78,0x1C,0xC4,0xC3,0xD2,0xB1,0x64,0x07,0xD7,0x6F,0x02,0xE9,0x44,0x31,
   0xDB,0x3C,0xEB,0x93,0xED,0x9A,0x57,0x05,0xB9,0x0E,0xAF,0x1F,0x48,0x11,0xDC,0x35,
   0x6C,0xB8,0xEE,0x2A,0x48,0x2B,0xBC,0x89,0x12,0x59,0xCB,0xD1,0x18,0xEA,0x72,0x11,
   0x01,0x75,0x3B,0xB5,0x56,0xF4,0x8B,0xA0,0x41,0x75,0x86,0x7B,0x94,0x12,0x2D,0x4C,
   0x0C,0x22,0xC9,0x4A,0xD8,0xB1,0x8D,0xF0,0x55,0x2E,0x77,0x50,0x1C,0x64,0x77,0xAA,
   0x3E,0xAC,0xD3,0x3D,0xCE,0x60,0xCA,0x5D,0xA0,0x92,0x78,0xC6,0x51,0xFE,0xF9,0x30
};

static const u8 _par3T2[256] = {
   0xAA,0xAF,0xF0,0x72,0x90,0xF7,0x71,0x27,0x06,0x11,0xEB,0x9C,0x37,0x12,0x72,0xAA,
   0x65,0xBC,0x0D,0x4A,0x76,0xF6,0x5C,0xAA,0xB0,0x7A,0x7D,0x81,0xC1,0xCE,0x2F,0x9F,
   0x02,0x75,0x38,0xC8,0xFC,0x66,0x05,0xC2,0x2C,0xBD,0x91,0xAD,0x03,0xB1,0x88,0x93,
   0x31,0xC6,0xAB,0x40,0x23,0x43,0x76,0x54,0xCA,0xE7,0x00,0x96,0x9F,0xD8,0x24,0x8B,
   0xE4,0xDC,0xDE,0x48,0x2C,0xCB,0xF7,0x84,0x1D,0x45,0xE5,0xF1,0x75,0xA0,0xED,0xCD,
   0x4B,0x24,0x8A,0xB3,0x98,0x7B,0x12,0xB8,0xF5,0x63,0x97,0xB3,0xA6,0xA6,0x0B,0xDC,
   0xD8,0x4C,0xA8,0x99,0x27,0x0F,0x8F,0x94,0x63,0x0F,0xB0,0x11,0x94,0xC7,0xE9,0x7F,
   0x3B,0x40,0x72,0x4C,0xDB,0x84,0x78,0xFE,0xB8,0x56,0x08,0x80,0xDF,0x20,0x2F,0xB9,
   0x66,0x2D,0x60,0x63,0xF5,0x18,0x15,0x1B,0x86,0x85,0xB9,0xB4,0x68,0x0E,0xC6,0xD1,
   0x8A,0x81,0x2B,0xB3,0xF6,0x48,0xF0,0x4F,0x9C,0x28,0x1C,0xA4,0x51,0x2F,0xD7,0x4B,
   0x17,0xE7,0xCC,0x50,0x9F,0xD0,0xD1,0x40,0x0C,0x0D,0xCA,0x83,0xFA,0x5E,0xCA,0xEC,
   0xBF,0x4E,0x7C,0x8F,0xF0,0xAE,0xC2,0xD3,0x28,0x41,0x9B,0xC8,0x04,0xB9,0x4A,0xBA,
   0x72,0xE2,0xB5,0x06,0x2C,0x1E,0x0B,0x2C,0x7F,0x11,0xA9,0x26,0x51,0x9D,0x3F,0xF8,
   0x62,0x11,0x2E,0x89,0xD2,0x9D,0x35,0xB1,0xE4,0x0A,0x4D,0x93,0x01,0xA7,0xD1,0x2D,
   0x00,0x87,0xE2,0x2D,0xA4,0xE9,0x0A,0x06,0x66,0xF8,0x1F,0x44,0x75,0xB5,0x6B,0x1C,
   0xFC,0x31,0x09,0x48,0xA3,0xFF,0x92,0x12,0x58,0xE9,0xFA,0xAE,0x4F,0xE2,0xB4,0xCC
};

/* TEA decryption - https://en.wikipedia.org/wiki/Tiny_Encryption_Algorithm */
void cheats_decrypt_gameshark(u32 *op1, u32 *op2, const u32 *seeds)
{
   u32 sum = 0xC6EF3720;
   int i;
   for (i = 0; i < 32; ++i) {
      *op2 -= ((*op1 << 4) + seeds[2]) ^ (*op1 + sum) ^ ((*op1 >> 5) + seeds[3]);
      *op1 -= ((*op2 << 4) + seeds[0]) ^ (*op2 + sum) ^ ((*op2 >> 5) + seeds[1]);
      sum -= 0x9E3779B9;
   }
}

void cheats_reseed_gameshark(u32 *seeds, u16 params, const u8 *t1, const u8 *t2)
{
   int x, y;
   int s0 = params >> 8;
   int s1 = params & 0xFF;
   for (y = 0; y < 4; ++y) {
      for (x = 0; x < 4; ++x) {
         u8 z = t1[(s0 + x) & 0xFF] + t2[(s1 + y) & 0xFF];
         seeds[y] <<= 8;
         seeds[y] |= z;
      }
   }
}

static void _set_gsa_version(cheat_type *ch, gsa_version_type version)
{
   ch->gsaVersion = version;
   if (version == GSA_VERSION_GSAV1 || version == GSA_VERSION_GSAV1_RAW)
      memcpy(ch->gsaSeeds, GSSeeds, 4 * sizeof(u32));
   else
      memcpy(ch->gsaSeeds, PAR3Seeds, 4 * sizeof(u32));
}

/* ================================================================
 *  CodeBreaker decryption
 * ================================================================ */

static void _cbLoadByteswap(u8 *buffer, u32 op1, u16 op2)
{
   buffer[0] = op1 >> 24;
   buffer[1] = op1 >> 16;
   buffer[2] = op1 >> 8;
   buffer[3] = op1;
   buffer[4] = op2 >> 8;
   buffer[5] = op2;
}

static void _cbStoreByteswap(u8 *buffer, u32 *op1, u16 *op2)
{
   *op1 = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
   *op2 = (buffer[4] << 8) | buffer[5];
}

static void _cbDecrypt(cheat_type *ch, u32 *op1, u16 *op2)
{
   u8 buffer[6];
   int i;

   _cbLoadByteswap(buffer, *op1, *op2);
   for (i = sizeof(ch->cbTable) - 1; i >= 0; --i) {
      int offsetX = i >> 3;
      int offsetY = ch->cbTable[i] >> 3;
      int bitX = i & 7;
      int bitY = ch->cbTable[i] & 7;

      u8 x = (buffer[offsetX] >> bitX) & 1;
      u8 y = (buffer[offsetY] >> bitY) & 1;
      u8 x2 = buffer[offsetX] & ~(1 << bitX);
      if (y) x2 |= 1 << bitX;
      buffer[offsetX] = x2;

      u8 y2 = buffer[offsetY] & ~(1 << bitY);
      if (x) y2 |= 1 << bitY;
      buffer[offsetY] = y2;
   }
   _cbStoreByteswap(buffer, op1, op2);

   *op1 ^= ch->cbSeeds[0];
   *op2 ^= ch->cbSeeds[1];

   _cbLoadByteswap(buffer, *op1, *op2);
   u32 master = ch->cbMaster;
   for (i = 0; i < 5; ++i)
      buffer[i] ^= (master >> 8) ^ buffer[i + 1];
   buffer[5] ^= master >> 8;

   for (i = 5; i > 0; --i)
      buffer[i] ^= master ^ buffer[i - 1];
   buffer[0] ^= master;
   _cbStoreByteswap(buffer, op1, op2);

   *op1 ^= ch->cbSeeds[2];
   *op2 ^= ch->cbSeeds[3];
}

static u32 _cbRand(cheat_type *ch)
{
   u32 roll  = ch->cbRngState * 0x41C64E6D + 0x3039;
   u32 roll2 = roll * 0x41C64E6D + 0x3039;
   u32 roll3 = roll2 * 0x41C64E6D + 0x3039;
   u32 mix   = (roll  << 14) & 0xC0000000;
   mix      |= (roll2 >>  1) & 0x3FFF8000;
   mix      |= (roll3 >> 16) & 0x7FFF;
   ch->cbRngState = roll3;
   return mix;
}

static u32 ROR32(u32 val, int n) { return (val >> n) | (val << (32 - n)); }

static u32 _cbSwapIndex(cheat_type *ch)
{
   u32 roll  = _cbRand(ch);
   u32 count = sizeof(ch->cbTable);

   if (roll == count) roll = 0;
   if (roll < count) return roll;

   u32 bit = 1;
   while (count < 0x10000000 && count < roll) { count <<= 4; bit <<= 4; }
   while (count < 0x80000000 && count < roll) { count <<= 1; bit <<= 1; }

   u32 mask;
   while (1) {
      mask = 0;
      if (roll >= count)      { roll -= count; }
      if (roll >= count >> 1) { roll -= count >> 1; mask |= ROR32(bit, 1); }
      if (roll >= count >> 2) { roll -= count >> 2; mask |= ROR32(bit, 2); }
      if (roll >= count >> 3) { roll -= count >> 3; mask |= ROR32(bit, 3); }
      if (!roll || !(bit >> 4)) break;
      bit >>= 4;
      count >>= 4;
   }

   mask &= 0xE0000000;
   if (!mask || !(bit & 7)) return roll;

   if (mask & ROR32(bit, 3)) roll += count >> 3;
   if (mask & ROR32(bit, 2)) roll += count >> 2;
   if (mask & ROR32(bit, 1)) roll += count >> 1;
   return roll;
}

void cheats_reseed_codebreaker(cheat_type *ch, u32 op1, u16 op2)
{
   ch->cbRngState = (op2 & 0xFF) ^ 0x1111;
   int i;
   for (i = 0; i < (int)sizeof(ch->cbTable); ++i)
      ch->cbTable[i] = i;
   for (i = 0; i < 0x50; ++i) {
      u32 x = _cbSwapIndex(ch);
      u32 y = _cbSwapIndex(ch);
      u8 swap = ch->cbTable[x];
      ch->cbTable[x] = ch->cbTable[y];
      ch->cbTable[y] = swap;
   }

   ch->cbRngState = 0x4EFAD1C3;
   for (i = 0; i < ((op1 >> 24) & 0xF); ++i)
      ch->cbRngState = _cbRand(ch);

   ch->cbSeeds[2] = _cbRand(ch);
   ch->cbSeeds[3] = _cbRand(ch);

   ch->cbRngState = (op2 >> 8) ^ 0xF254;
   for (i = 0; i < (int)(op2 >> 8); ++i)
      ch->cbRngState = _cbRand(ch);

   ch->cbSeeds[0] = _cbRand(ch);
   ch->cbSeeds[1] = _cbRand(ch);

   ch->cbMaster = op1;
}

void cheats_decrypt_codebreaker(cheat_type *ch, u32 *op1, u16 *op2)
{
   if (ch->cbMaster)
      _cbDecrypt(ch, op1, op2);
}

/* ================================================================
 *  PARv3 helpers
 * ================================================================ */

static u32 _parAddr(u32 x)
{
   return (x & 0xFFFFF) | ((x << 4) & 0x0F000000);
}

#define PAR3_COND          0x38000000
#define PAR3_WIDTH_MASK    0x06000000
#define PAR3_WIDTH_BASE    25
#define PAR3_ACTION_MASK   0xC0000000
#define PAR3_BASE_MASK     0xC0000000
#define PAR3_OFFSET_MASK   0xFFFF

#define PAR3_ACTION_NEXT      0x00000000
#define PAR3_ACTION_NEXT_TWO  0x40000000
#define PAR3_ACTION_BLOCK     0x80000000
#define PAR3_ACTION_DISABLE   0xC0000000

#define PAR3_BASE_ASSIGN    0x00000000
#define PAR3_BASE_INDIRECT  0x40000000
#define PAR3_BASE_ADD       0x80000000
#define PAR3_BASE_OTHER     0xC0000000

#define PAR3_COND_EQ   0x08000000
#define PAR3_COND_NE   0x10000000
#define PAR3_COND_LT   0x18000000
#define PAR3_COND_GT   0x20000000
#define PAR3_COND_ULT  0x28000000
#define PAR3_COND_UGT  0x30000000
#define PAR3_COND_AND  0x38000000

#define PAR3_OTHER_ENDIF     0x40000000
#define PAR3_OTHER_ELSE      0x60000000
#define PAR3_OTHER_FILL_1    0x80000000
#define PAR3_OTHER_FILL_2    0x82000000
#define PAR3_OTHER_FILL_4    0x84000000
#define PAR3_OTHER_BUTTON_1  0x10000000
#define PAR3_OTHER_BUTTON_2  0x12000000
#define PAR3_OTHER_BUTTON_4  0x14000000
#define PAR3_OTHER_SLOWDOWN  0x08000000
#define PAR3_OTHER_PATCH_1   0x18000000
#define PAR3_OTHER_PATCH_2   0x1A000000
#define PAR3_OTHER_PATCH_3   0x1C000000
#define PAR3_OTHER_PATCH_4   0x1E000000

static void _add_cheat(cheat_type *ch, cheat_op_type type, u32 address,
                       u32 operand, u8 width, u16 repeat, u16 negRepeat)
{
   if (ch->cheat_count >= MAX_CHEAT_CODES) return;
   cheat_code *c = &ch->codes[ch->cheat_count++];
   c->type = type;
   c->address = address;
   c->operand = operand;
   c->width = width;
   c->repeat = repeat;
   c->negativeRepeat = negRepeat;
   c->addressOffset = 0;
   c->operandOffset = 0;
}

/* ================================================================
 *  GameShark parser
 * ================================================================ */

/* GBA memory base addresses */
#define GBA_BASE_ROM  0x08000000
#define GBA_BASE_IO   0x04000000

cheat_error cheats_parse_gameshark_raw(cheat_type *ch, u32 op1, u32 op2)
{
   u32 type = op1 >> 28;
   u32 address = op1 & 0x0FFFFFFF;

   /* Continue incomplete assign-list */
   if (ch->incompleteCheat >= 0) {
      cheat_code *incomplete = &ch->codes[ch->incompleteCheat];
      if (ch->remainingAddresses > 0) {
         _add_cheat(ch, CHEAT_ASSIGN, op1, incomplete->operand, 4, 1, 0);
         --ch->remainingAddresses;
      }
      if (ch->remainingAddresses > 0) {
         _add_cheat(ch, CHEAT_ASSIGN, op2, incomplete->operand, 4, 1, 0);
         --ch->remainingAddresses;
      }
      if (ch->remainingAddresses == 0)
         ch->incompleteCheat = -1;
      return CheatNoError;
   }

   switch (type) {
   case 0x0: /* GSA_ASSIGN_1 */
      _add_cheat(ch, CHEAT_ASSIGN, address, op2, 1, 1, 0);
      break;
   case 0x1: /* GSA_ASSIGN_2 */
      _add_cheat(ch, CHEAT_ASSIGN, address, op2, 2, 1, 0);
      break;
   case 0x2: /* GSA_ASSIGN_4 */
      _add_cheat(ch, CHEAT_ASSIGN, address, op2, 4, 1, 0);
      break;
   case 0x3: /* GSA_ASSIGN_LIST */
      ch->remainingAddresses = (op1 & 0xFFFF) - 1;
      _add_cheat(ch, CHEAT_ASSIGN, op2, 0, 4, 1, 0);
      ch->incompleteCheat = ch->cheat_count - 1;
      break;
   case 0x6: /* GSA_PATCH */
      if (ch->patch_count < MAX_CHEAT_CODES) {
         cheat_rom_patch *p = &ch->patches[ch->patch_count++];
         p->address = GBA_BASE_ROM | ((op1 & 0xFFFFFF) << 1);
         p->value = op2;
         p->width = 2;
         p->applied = 0;
         p->checkActive = 0;
      }
      break;
   case 0xD: /* GSA_IF_EQ */
      if (op1 == 0xDEADFACE) {
         cheats_reseed_gameshark(ch->gsaSeeds, op2, _gsa1T1, _gsa1T2);
      } else {
         _add_cheat(ch, CHEAT_IF_EQ, address, op2, 2, 1, 0);
      }
      break;
   case 0xE: /* GSA_IF_EQ_RANGE */
      _add_cheat(ch, CHEAT_IF_EQ, op2 & 0x0FFFFFFF, op1 & 0xFFFF,
                 2, (op1 >> 16) & 0xFF, 0);
      break;
   case 0xF: /* GSA_HOOK */
      if (ch->hookCount < MAX_HOOK_ADDRS) {
         ch->hookAddresses[ch->hookCount++] =
            GBA_BASE_ROM | (op1 & 0xFFFFFF);
      }
      break;
   case 0x8: /* GSA_BUTTON */
      _add_cheat(ch, CHEAT_IF_BUTTON, 0, 0, 1, 1, 0);
      switch (op1 & 0x00F00000) {
      case 0x00100000:
         _add_cheat(ch, CHEAT_ASSIGN, op1 & 0x0F0FFFFF, op2, 1, 1, 0);
         break;
      case 0x00200000:
         _add_cheat(ch, CHEAT_ASSIGN, op1 & 0x0F0FFFFF, op2, 2, 1, 0);
         break;
      default:
         break;
      }
      break;
   default:
      return CheatErrorNotSupported;
   }
   return CheatNoError;
}

cheat_error cheats_parse_gameshark(cheat_type *ch, u32 op1, u32 op2)
{
   u32 o1 = op1, o2 = op2;

   if (ch->gsaVersion == 0 || ch->gsaVersion == GSA_VERSION_GSAV1)
      _set_gsa_version(ch, GSA_VERSION_GSAV1);

   if (ch->gsaVersion == GSA_VERSION_GSAV1)
      cheats_decrypt_gameshark(&o1, &o2, ch->gsaSeeds);

   return cheats_parse_gameshark_raw(ch, o1, o2);
}

/* ================================================================
 *  PARv3 parser
 * ================================================================ */

static int _par_current_block(cheat_type *ch)
{
   return ch->currentBlock ? (int)(ch->currentBlock - 1) : -1;
}

static void _par_end_block(cheat_type *ch)
{
   int blockIdx = _par_current_block(ch);
   if (blockIdx < 0) return;
   cheat_code *block = &ch->codes[blockIdx];
   int size = (int)ch->cheat_count - blockIdx - 1;
   if (block->repeat)
      block->negativeRepeat = size - block->repeat;
   else
      block->repeat = size;
   ch->currentBlock = 0;
}

static void _par_else_block(cheat_type *ch)
{
   int blockIdx = _par_current_block(ch);
   if (blockIdx < 0) return;
   cheat_code *block = &ch->codes[blockIdx];
   block->repeat = (int)ch->cheat_count - blockIdx - 1;
}

static bool _par_add_cond(cheat_type *ch, u32 op1, u32 op2)
{
   u32 condition = op1 & PAR3_COND;
   int width = 1 << ((op1 & PAR3_WIDTH_MASK) >> PAR3_WIDTH_BASE);
   if ((op1 & PAR3_ACTION_MASK) == PAR3_ACTION_DISABLE)
      return false;

   cheat_op_type ct;
   switch (condition) {
   case PAR3_COND_EQ:  ct = CHEAT_IF_EQ;  break;
   case PAR3_COND_NE:  ct = CHEAT_IF_NE;  break;
   case PAR3_COND_LT:  ct = CHEAT_IF_LT;  break;
   case PAR3_COND_GT:  ct = CHEAT_IF_GT;  break;
   case PAR3_COND_ULT: ct = CHEAT_IF_ULT; break;
   case PAR3_COND_UGT: ct = CHEAT_IF_UGT; break;
   case PAR3_COND_AND: ct = CHEAT_IF_AND; break;
   default: return false;
   }

   if (width > 4) { ct = CHEAT_NEVER; width = 0; }

   u16 repeat = 1, negRepeat = 0;
   u32 action = op1 & PAR3_ACTION_MASK;
   if (action == PAR3_ACTION_NEXT_TWO) repeat = 2;
   else if (action == PAR3_ACTION_BLOCK) {
      repeat = 0;
      if (ch->currentBlock) _par_end_block(ch);
      /* Store block index + 1 (0 means no block) */
   }

   _add_cheat(ch, ct, _parAddr(op1),
              op2 & (0xFFFFFFFFU >> ((4 - width) * 8)),
              width, repeat, negRepeat);

   if (action == PAR3_ACTION_BLOCK)
      ch->currentBlock = ch->cheat_count; /* +1 implied by 0=no block */

   return true;
}

static bool _par_add_special(cheat_type *ch, u32 op2)
{
   u32 special = op2 & 0xFF000000;
   if (special == PAR3_OTHER_SLOWDOWN)
      return false; /* TODO */

   if (special >= PAR3_OTHER_BUTTON_1 && special <= PAR3_OTHER_BUTTON_4) {
      u8 width = (special == PAR3_OTHER_BUTTON_2) ? 2 :
                 (special == PAR3_OTHER_BUTTON_4) ? 4 : 1;
      _add_cheat(ch, CHEAT_IF_BUTTON, 0, 0, 1, 1, 0);
      _add_cheat(ch, CHEAT_ASSIGN, _parAddr(op2), 0, width, 1, 0);
      ch->incompleteCheat = ch->cheat_count - 1;
      return true;
   }

   if (special >= PAR3_OTHER_PATCH_1 && special <= PAR3_OTHER_PATCH_4) {
      if (ch->patch_count < MAX_CHEAT_CODES) {
         cheat_rom_patch *p = &ch->patches[ch->patch_count++];
         p->address = GBA_BASE_ROM | ((op2 & 0xFFFFFF) << 1);
         p->value = 0;
         p->width = 2;
         p->applied = 0;
         p->checkActive = 0;
      }
      return true;
   }

   if (special == PAR3_OTHER_ENDIF) {
      if (ch->currentBlock) { _par_end_block(ch); return true; }
      return false;
   }

   if (special == PAR3_OTHER_ELSE) {
      if (ch->currentBlock) { _par_else_block(ch); return true; }
      return false;
   }

   if (special >= PAR3_OTHER_FILL_1 && special <= PAR3_OTHER_FILL_4) {
      u8 width = (special == PAR3_OTHER_FILL_2) ? 2 :
                 (special == PAR3_OTHER_FILL_4) ? 4 : 1;
      _add_cheat(ch, CHEAT_ASSIGN, _parAddr(op2), 0, width, 1, 0);
      ch->incompleteCheat = ch->cheat_count - 1;
      return true;
   }

   return false;
}

cheat_error cheats_parse_parv3_raw(cheat_type *ch, u32 op1, u32 op2)
{
   /* Complete pending ROM patch */
   if (ch->patch_count > 0) {
      cheat_rom_patch *last = &ch->patches[ch->patch_count - 1];
      if (!last->value) { last->value = op1; return CheatNoError; }
   }

   /* Complete pending incomplete cheat (button/fill) */
   if (ch->incompleteCheat >= 0) {
      cheat_code *ic = &ch->codes[ch->incompleteCheat];
      if (ic->operand == 0) {
         ic->operand = op1 & (0xFFFFFFFFU >> ((4 - ic->width) * 8));
         if (ch->incompleteCheat > 0 &&
             ch->codes[ch->incompleteCheat - 1].type == CHEAT_IF_BUTTON) {
            ch->incompleteCheat = -1;
         } else {
            ic->operandOffset = op2 >> 24;
            ic->repeat = (op2 >> 16) & 0xFF;
            ic->addressOffset = (op2 & 0xFFFF) * ic->width;
            ch->incompleteCheat = -1;
         }
         return CheatNoError;
      }
   }

   if (op2 == 0x001DC0DE) return CheatNoError;

   if (op1 == 0x00000000)
      return _par_add_special(ch, op2) ? CheatNoError : CheatErrorNotSupported;

   if (op1 == 0xDEADFACE) {
      cheats_reseed_gameshark(ch->gsaSeeds, op2, _par3T1, _par3T2);
      return CheatNoError;
   }

   if (op1 >> 24 == 0xC4) {
      if (ch->hookCount < MAX_HOOK_ADDRS)
         ch->hookAddresses[ch->hookCount++] =
            GBA_BASE_ROM | (op1 & 0xFFFFFF);
      return CheatNoError;
   }

   if (op1 & PAR3_COND)
      return _par_add_cond(ch, op1, op2) ? CheatNoError : CheatErrorNotSupported;

   int width = 1 << ((op1 & PAR3_WIDTH_MASK) >> PAR3_WIDTH_BASE);
   cheat_op_type ct;

   switch (op1 & PAR3_BASE_MASK) {
   case PAR3_BASE_ASSIGN:
      ct = CHEAT_ASSIGN;
      break;
   case PAR3_BASE_INDIRECT:
      ct = CHEAT_ASSIGN_INDIRECT;
      break;
   case PAR3_BASE_ADD:
      ct = CHEAT_ADD;
      break;
   case PAR3_BASE_OTHER:
      width = ((op1 >> 24) & 1) + 1;
      _add_cheat(ch, CHEAT_ASSIGN, GBA_BASE_IO | (op1 & PAR3_OFFSET_MASK),
                 op2 & (0xFFFFFFFFU >> ((4 - width) * 8)), width, 1, 0);
      return CheatNoError;
   default:
      return CheatErrorNotSupported;
   }

   if (op1 & 0x01000000 && (op1 & 0xFE000000) != 0xC6000000)
      return CheatErrorNotSupported;

   _add_cheat(ch, ct, _parAddr(op1),
              op2 & (0xFFFFFFFFU >> ((4 - width) * 8)), width, 1, 0);

   /* Handle multi-write for assign with width<4 */
   if (ct == CHEAT_ASSIGN && width < 4) {
      cheat_code *last = &ch->codes[ch->cheat_count - 1];
      last->repeat = (op2 >> (width * 8)) + 1;
      last->addressOffset = width;
   }
   if (ct == CHEAT_ASSIGN_INDIRECT && width < 4) {
      cheat_code *last = &ch->codes[ch->cheat_count - 1];
      last->addressOffset = (op2 >> (width * 8)) * width;
   }

   return CheatNoError;
}

cheat_error cheats_parse_parv3(cheat_type *ch, u32 op1, u32 op2)
{
   u32 o1 = op1, o2 = op2;

   if (ch->gsaVersion == 0 || ch->gsaVersion == GSA_VERSION_PARV3)
      _set_gsa_version(ch, GSA_VERSION_PARV3);

   if (ch->gsaVersion == GSA_VERSION_PARV3)
      cheats_decrypt_gameshark(&o1, &o2, ch->gsaSeeds);

   return cheats_parse_parv3_raw(ch, o1, o2);
}

/* ================================================================
 *  CodeBreaker raw parser
 * ================================================================ */

cheat_error cheats_parse_codebreaker_raw(cheat_type *ch, u32 op1, u16 op2)
{
   if (ch->cbMaster)
      _cbDecrypt(ch, &op1, &op2);

   u32 type = op1 >> 28;
   u32 address = op1 & 0x0FFFFFFF;

   /* Complete pending fill */
   if (ch->incompleteCheat >= 0) {
      cheat_code *ic = &ch->codes[ch->incompleteCheat];
      ic->repeat = op1 & 0xFFFF;
      ic->addressOffset = op2;
      ic->operandOffset = op1 >> 16;
      ch->incompleteCheat = -1;
      return CheatNoError;
   }

   switch (type) {
   case 0x0: /* Game CRC - ignored */
      break;
   case 0x1: /* Hook */
      if (ch->hookCount < MAX_HOOK_ADDRS)
         ch->hookAddresses[ch->hookCount++] =
            GBA_BASE_ROM | (op1 & 0x1FFFFFF);
      break;
   case 0x2: /* 16-bit OR */
      _add_cheat(ch, CHEAT_OR, address, op2, 2, 1, 0);
      break;
   case 0x3: /* 8-bit write */
      _add_cheat(ch, CHEAT_ASSIGN, address, op2, 1, 1, 0);
      break;
   case 0x4: /* Fill / slide code */
      _add_cheat(ch, CHEAT_ASSIGN, address, op2, 2, 0, 0);
      ch->incompleteCheat = ch->cheat_count - 1;
      break;
   case 0x5: /* Super code (FILL_8) - unsupported */
      return CheatErrorNotSupported;
   case 0x6: /* 16-bit AND */
      _add_cheat(ch, CHEAT_AND, address, op2, 2, 1, 0);
      break;
   case 0x7: /* IF != */
      _add_cheat(ch, CHEAT_IF_NE, address, op2, 2, 1, 0);
      break;
   case 0x8: /* 16-bit write */
      _add_cheat(ch, CHEAT_ASSIGN, address, op2, 2, 1, 0);
      break;
   case 0x9: /* Encrypt/reseed */
      cheats_reseed_codebreaker(ch, op1, op2);
      break;
   case 0xA: /* IF == (condition to execute next) */
      _add_cheat(ch, CHEAT_IF_NE, address, op2, 2, 1, 0);
      break;
   case 0xB: /* IF > (skip if <=) */
      _add_cheat(ch, CHEAT_IF_GT, address, op2, 2, 1, 0);
      break;
   case 0xC: /* IF < (skip if >=) */
      _add_cheat(ch, CHEAT_IF_LT, address, op2, 2, 1, 0);
      break;
   case 0xD: /* IF special (button check) */
      if (address == 0x20) {
         _add_cheat(ch, CHEAT_IF_NAND, GBA_BASE_IO | 0x130, op2, 2, 1, 0);
      }
      break;
   case 0xE: /* 16/32-bit ADD */
      if (address & 1) {
         _add_cheat(ch, CHEAT_ADD, address & ~1U, (u32)((s16)op2), 4, 1, 0);
      } else {
         _add_cheat(ch, CHEAT_ADD, address, op2, 2, 1, 0);
      }
      break;
   case 0xF: /* IF AND (skip if == 0) */
      _add_cheat(ch, CHEAT_IF_AND, address, op2, 2, 1, 0);
      break;
   }
   return CheatNoError;
}

/* ================================================================
 *  VBA format parser
 * ================================================================ */

cheat_error cheats_parse_vba_line(cheat_type *ch, const char *line)
{
   u32 address = 0;
   u32 value = 0;
   int width = 0;
   u8 byte;
   const char *p = line;
   int chars;

   /* Parse hex address */
   if (sscanf(p, "%08x:%n", &address, &chars) < 1)
      return CheatErrorNotSupported;
   p += chars;

   /* Parse hex bytes */
   while (sscanf(p, "%02hhx%n", &byte, &chars) == 1 && width < 4) {
      value <<= 8;
      value |= byte;
      width++;
      p += chars;
   }

   if (width == 0 || width == 3)
      return CheatErrorNotSupported;

   if (address >= 0x08000000 && address < 0x0E000000) {
      /* ROM patch */
      if (ch->patch_count < MAX_CHEAT_CODES) {
         cheat_rom_patch *pt = &ch->patches[ch->patch_count++];
         pt->address = address;
         pt->value = value;
         pt->width = width;
         pt->applied = 0;
         pt->checkActive = 0;
      }
   } else {
      _add_cheat(ch, CHEAT_ASSIGN, address, value, width, 1, 0);
   }
   return CheatNoError;
}

/* ================================================================
 *  Auto-detection
 * ================================================================ */

/* GBA address region validation for probability scoring */
static int _addr_is_real(u32 address)
{
   switch (address >> 24) {
   case 0x02: /* EWRAM */
      if ((address & 0x3FFFF) > 0x40000) return -0x40;
      return 0x20;
   case 0x03: /* IWRAM */
      if ((address & 0x7FFF) > 0x8000) return -0x40;
      return 0x20;
   case 0x04: /* IO */
      if ((address & 0x3FF) > 0x3FF) return -0x80;
      return 0x10;
   case 0x05: /* Palette */
   case 0x06: /* VRAM */
   case 0x07: /* OAM */
   case 0x0D: /* EEPROM */
   case 0x0E: /* SRAM */
   case 0x0F: /* SRAM */
      return -0x8;
   case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C:
      return -0x8; /* ROM */
   default:
      return -0xC0;
   }
}

static int _gs_probability(u32 op1, u32 op2)
{
   int prob = 0;
   if (op2 == 0x001DC0DE) return 0x100;
   u32 address = op1 & 0x0FFFFFFF;
   switch (op1 >> 28) {
   case 0x0: case 0x1: case 0x2:
      prob += 0x20;
      if ((op1 >> 28) == 0x0 && (op2 & 0xFFFFFF00)) prob -= 0x10;
      if ((op1 >> 28) == 0x1 && (op2 & 0xFFFF0000)) prob -= 0x10;
      prob += _addr_is_real(address);
      break;
   case 0x6:
      prob += 0x20;
      if (op2 & 0xCFFF0000) prob -= 0x10;
      break;
   case 0x8:
      prob += 0x10;
      break;
   case 0xD:
      prob += 0x20;
      if (op2 & 0xFFFF0000) prob -= 0x10;
      prob += _addr_is_real(address);
      break;
   case 0xE:
      prob += 0x20;
      prob += _addr_is_real(op2 & 0x0FFFFFFF);
      if (op1 & 0x0F000000) prob -= 0x10;
      break;
   case 0xF:
      prob += 0x20;
      if (op2 & 0xFFFF0000) prob -= 0x10;
      break;
   default:
      prob -= 0x40;
   }
   return prob;
}

static int _par3_probability(u32 op1, u32 op2)
{
   int prob = 0;
   if (op2 == 0x001DC0DE) return 0x100;
   if (op1 == 0xDEADFACE && !(op2 & 0xFFFF0000)) return 0x100;
   if (!op1) {
      prob += 0x40;
      u32 address = _parAddr(op2);
      switch (op2 & 0xFE000000) {
      case PAR3_OTHER_FILL_1: case PAR3_OTHER_FILL_2: case PAR3_OTHER_FILL_4:
         prob += _addr_is_real(address);
         break;
      case PAR3_OTHER_ENDIF: case PAR3_OTHER_ELSE: case PAR3_OTHER_SLOWDOWN:
      case PAR3_OTHER_BUTTON_1: case PAR3_OTHER_BUTTON_2: case PAR3_OTHER_BUTTON_4:
         if (op2 & 0x01000000) prob -= 0x40;
         break;
      default:
         prob -= 0x40;
      }
      return prob;
   }

   int width = ((op1 & PAR3_WIDTH_MASK) >> (PAR3_WIDTH_BASE - 3));
   if (op1 & PAR3_COND) {
      prob += 0x20;
      if (width >= 24) return 0;
      if (op2 & ~((1 << width) - 1)) prob -= 0x10;
   } else {
      u32 address = _parAddr(op1);
      prob += 0x20;
      switch (op1 & PAR3_BASE_MASK) {
      case PAR3_BASE_ADD:
         if (op2 & ~((1 << width) - 1)) prob -= 0x10;
         /* fallthrough */
      case PAR3_BASE_ASSIGN:
      case PAR3_BASE_INDIRECT:
         prob += _addr_is_real(address);
         if (op1 & 0x01000000) return 0;
         break;
      default:
         break;
      }
   }
   return prob;
}

cheat_error cheats_parse_autodetect(cheat_type *ch, u32 op1, u32 op2)
{
   u32 o1, o2;
   int bestProb = -10000;
   gsa_version_type bestVer = GSA_VERSION_GSAV1;
   bool isRaw = false;

   if (ch->gsaVersion == 0) {
      /* Try GSAv1 */
      o1 = op1; o2 = op2;
      cheats_decrypt_gameshark(&o1, &o2, GSSeeds);
      int prob = _gs_probability(o1, o2);
      if (prob > bestProb) { bestProb = prob; bestVer = GSA_VERSION_GSAV1; }

      /* Try PARv3 */
      o1 = op1; o2 = op2;
      cheats_decrypt_gameshark(&o1, &o2, PAR3Seeds);
      prob = _par3_probability(o1, o2);
      if (prob > bestProb) { bestProb = prob; bestVer = GSA_VERSION_PARV3; }

      /* Try RAW GSAv1 */
      prob = _gs_probability(op1, op2);
      if (prob > bestProb) { bestProb = prob; bestVer = GSA_VERSION_GSAV1_RAW; isRaw = true; }

      /* Try RAW PARv3 */
      prob = _par3_probability(op1, op2);
      if (prob > bestProb) { bestProb = prob; bestVer = GSA_VERSION_PARV3_RAW; isRaw = true; }

      _set_gsa_version(ch, bestVer);
   }

   if (ch->gsaVersion <= GSA_VERSION_GSAV1_RAW)
      return cheats_parse_gameshark(ch, op1, op2);
   else
      return cheats_parse_parv3(ch, op1, op2);
}
