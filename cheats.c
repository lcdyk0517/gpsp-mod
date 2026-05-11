/* gameplaySP - Extended cheat system execution engine (ported from mgba)
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

/* GBA memory base addresses */
#define GBA_BASE_ROM  0x08000000
#define GBA_BASE_IO   0x04000000

cheat_type cheats[MAX_CHEATS];
u32 max_cheat = 0;
u32 cheat_hook_addresses[MAX_HOOK_ADDRS * MAX_CHEATS];
u32 cheat_hook_count = 0;

/* ================================================================
 *  Memory read/write helpers (by width)
 * ================================================================ */

static u32 _read_mem(u32 address, u8 width)
{
   switch (width) {
   case 1:  return read_memory8(address);
   case 2:  return read_memory16(address);
   case 4:  return read_memory32(address);
   default: return 0;
   }
}

static void _write_mem(u32 address, u8 width, u32 value)
{
   switch (width) {
   case 1: write_memory8(address, value);  break;
   case 2: write_memory16(address, value); break;
   case 4: write_memory32(address, value); break;
   }
}

/* ================================================================
 *  ROM patching (reference-counted)
 * ================================================================ */

static u32 _patch_make_key(u32 address, u8 width)
{
   if (width == 2) address >>= 1;
   else if (width == 4) address >>= 2;
   return address;
}

static void _apply_patches(cheat_type *ch)
{
   unsigned i;
   for (i = 0; i < ch->patch_count; i++) {
      cheat_rom_patch *p = &ch->patches[i];
      if (p->applied) continue;

      u32 key = _patch_make_key(p->address, p->width);
      unsigned j;
      bool found = false;
      for (j = 0; j < ch->cheat_count; j++) {
         cheat_code *c = &ch->codes[j];
         if (c->type == CHEAT_TYPE_HOOK) continue;
      }
      /* Check if already patched via unpatched table */
      for (j = 0; j < ch->patch_count; j++) {
         cheat_rom_patch *other = &ch->patches[j];
         if (other->applied && other->address == p->address && other->width == p->width) {
            found = true;
            break;
         }
      }

      if (!found) {
         /* Save original value */
         p->originalValue = _read_mem(p->address, p->width);
      }
      _write_mem(p->address, p->width, p->value);
      p->applied = 1;
   }
}

static void _restore_patches(cheat_type *ch)
{
   unsigned i;
   for (i = 0; i < ch->patch_count; i++) {
      cheat_rom_patch *p = &ch->patches[i];
      if (!p->applied) continue;

      /* Check if any other cheat set patches this same address */
      unsigned otherIdx;
      bool otherUses = false;
      for (otherIdx = 0; otherIdx <= max_cheat; otherIdx++) {
         cheat_type *other = &cheats[otherIdx];
         if (other == ch || !other->cheat_active) continue;
         unsigned j;
         for (j = 0; j < other->patch_count; j++) {
            cheat_rom_patch *op = &other->patches[j];
            if (op->applied && op->address == p->address && op->width == p->width) {
               otherUses = true;
               break;
            }
         }
         if (otherUses) break;
      }

      if (!otherUses) {
         _write_mem(p->address, p->width, p->originalValue);
      }
      p->applied = 0;
   }
}

/* ================================================================
 *  Abstract cheat execution engine
 * ================================================================ */

static void _process_cheat_set(cheat_type *ch, u16 pad)
{
   if (!ch->cheat_active) return;

   /* ROM patches: apply if enabled */
   if (ch->patch_count > 0) {
      _apply_patches(ch);
   }

   unsigned i;

   /* Track pending fill parameters */
   s32 fillRepeat = 0;
   s32 fillAddrOffset = 0;
   s32 fillOpOffset = 0;
   u32 fillAddress = 0;
   u32 fillValue = 0;
   u8  fillWidth = 0;

   /* Block-level else/endif tracking */
   s16 elseLoc = 0;
   s16 endLoc = 0;

   for (i = 0; i < ch->cheat_count; i++) {
      cheat_code *c = &ch->codes[i];

      /* Handle pending fill */
      if (fillRepeat > 0) {
         u32 addr = fillAddress;
         u32 val  = fillValue;
         s32 remaining = fillRepeat;
         for (; remaining > 0; --remaining) {
            _write_mem(addr, fillWidth, val);
            addr += fillAddrOffset;
            val  += fillOpOffset;
         }
         fillRepeat = 0;
         continue;
      }

      u32 value = 0;
      bool performAssignment = false;
      bool condition = true;
      s16 conditionRemaining = 0;
      s16 negativeConditionRemaining = 0;
      u32 operationsRemaining = c->repeat > 0 ? c->repeat : 1;
      u32 address = c->address;
      u32 operand = c->operand;

      s32 opIter;
      for (opIter = operationsRemaining; opIter > 0; --opIter) {
         switch (c->type) {
         case CHEAT_ASSIGN:
            value = operand;
            performAssignment = true;
            break;
         case CHEAT_ASSIGN_INDIRECT:
            value = operand;
            address = _read_mem(address, 4) + c->addressOffset;
            performAssignment = true;
            break;
         case CHEAT_AND:
            value = _read_mem(address, c->width) & operand;
            performAssignment = true;
            break;
         case CHEAT_ADD:
            value = _read_mem(address, c->width) + operand;
            performAssignment = true;
            break;
         case CHEAT_OR:
            value = _read_mem(address, c->width) | operand;
            performAssignment = true;
            break;
         case CHEAT_IF_EQ:
            condition = (_read_mem(address, c->width) == operand);
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_NE:
            condition = (_read_mem(address, c->width) != operand);
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_LT:
            condition = ((s32)_read_mem(address, c->width) < (s32)operand);
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_GT:
            condition = ((s32)_read_mem(address, c->width) > (s32)operand);
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_ULT:
            condition = (_read_mem(address, c->width) < operand);
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_UGT:
            condition = (_read_mem(address, c->width) > operand);
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_AND:
            condition = (_read_mem(address, c->width) & operand) != 0;
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_LAND:
            condition = _read_mem(address, c->width) && operand;
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_NAND:
            condition = !(_read_mem(address, c->width) & operand);
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_IF_BUTTON:
            /* Check if any button in operand is pressed */
            condition = ((~pad) & operand & 0x3FF) == operand;
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_NEVER:
            condition = false;
            conditionRemaining = c->repeat;
            negativeConditionRemaining = c->negativeRepeat;
            opIter = 0;
            break;
         case CHEAT_TYPE_HOOK:
            break;
         }

         if (performAssignment) {
            _write_mem(address, c->width, value);
         }

         address += c->addressOffset;
         operand += c->operandOffset;
      }

      /* Handle else/endif block jumping */
      if (elseLoc && (s16)i == elseLoc) {
         i = endLoc;
         endLoc = 0;
         elseLoc = 0;
      }

      /* Handle condition skip: jump over conditionRemaining codes on false */
      if (conditionRemaining > 0 && !condition) {
         i += conditionRemaining;
      } else if (negativeConditionRemaining > 0) {
         elseLoc = (s16)i + conditionRemaining;
         endLoc = elseLoc + negativeConditionRemaining;
      }
   }

   /* Restore ROM patches if cheat set should not have them applied permanently */
   /* (patches are restored when cheat is disabled via cheat_clear) */
}

/* ================================================================
 *  Main entry points (called from main loop / libretro)
 * ================================================================ */

void process_cheats(void)
{
   u16 pad = 0x3FF ^ read_ioreg(REG_P1);
   unsigned i;
   for (i = 0; i <= max_cheat; i++) {
      if (!cheats[i].cheat_active)
         continue;
      _process_cheat_set(&cheats[i], pad);
   }
}

void cheat_clear(void)
{
   unsigned i;
   for (i = 0; i < MAX_CHEATS; i++) {
      cheats[i].cheat_count = 0;
      cheats[i].patch_count = 0;
      cheats[i].cheat_active = false;
      cheats[i].hookCount = 0;
      cheats[i].gsaVersion = 0;
      cheats[i].cbMaster = 0;
      cheats[i].currentBlock = 0;
      cheats[i].incompleteCheat = -1;
      cheats[i].remainingAddresses = 0;
   }
   max_cheat = 0;
   cheat_hook_count = 0;
   memset(cheat_hook_addresses, 0xFF, sizeof(cheat_hook_addresses));
}

void cheat_disable(unsigned index)
{
   if (index >= MAX_CHEATS)
      return;
   cheats[index].cheat_active = false;
}

void cheats_update_hooks(void)
{
   unsigned i;
   cheat_hook_count = 0;
   for (i = 0; i <= max_cheat; i++) {
      if (!cheats[i].cheat_active) continue;
      unsigned j;
      for (j = 0; j < cheats[i].hookCount; j++) {
         if (cheat_hook_count < MAX_HOOK_ADDRS * MAX_CHEATS) {
            cheat_hook_addresses[cheat_hook_count++] = cheats[i].hookAddresses[j];
         }
      }
   }
}

bool cheats_is_hook_address(u32 pc)
{
   unsigned i;
   for (i = 0; i < cheat_hook_count; i++) {
      if (cheat_hook_addresses[i] == pc)
         return true;
   }
   return false;
}

void cheats_process_hook(u32 pc)
{
   u16 pad = 0x3FF ^ read_ioreg(REG_P1);
   unsigned i;
   for (i = 0; i <= max_cheat; i++) {
      if (!cheats[i].cheat_active) continue;
      unsigned j;
      for (j = 0; j < cheats[i].hookCount; j++) {
         if (cheats[i].hookAddresses[j] == pc) {
            _process_cheat_set(&cheats[i], pad);
            break;
         }
      }
   }
}

cheat_error cheat_parse(unsigned index, const char *code)
{
   if (index >= MAX_CHEATS)
      return CheatErrorTooMany;

   cheat_type *ch = &cheats[index];
   unsigned codelen = strlen(code);
   char buf[2048];

   if (codelen >= sizeof(buf))
      return CheatErrorTooBig;

   memcpy(buf, code, codelen + 1);

   /* Init to a known good state */
   ch->cheat_count = 0;
   ch->patch_count = 0;
   ch->hookCount = 0;
   ch->gsaVersion = 0;
   ch->cbMaster = 0;
   ch->currentBlock = 0;
   ch->incompleteCheat = -1;
   ch->remainingAddresses = 0;
   ch->cheat_active = false;
   if (index > (int)max_cheat)
      max_cheat = index;

   /* ================================================================
    *  Try to auto-detect the cheat format
    *
    *  GameShark/PARv3:  "XXXXXXXX YYYYYYYY"  (two 32-bit hex words)
    *  CodeBreaker:      "XXXXXXXX YYYY"      (32-bit + 16-bit)
    *  VBA:              "XXXXXXXX:YY"        (address:value with colon)
    * ================================================================ */

   /* First, normalize: replace all non-hex (except colon/space/plus) to spaces */
   unsigned pos;
   bool hasColon = false;
   for (pos = 0; pos < codelen; pos++) {
      char chc = buf[pos];
      if (chc == ':') { hasColon = true; continue; }
      if (!((chc >= '0' && chc <= '9') ||
            (chc >= 'a' && chc <= 'f') ||
            (chc >= 'A' && chc <= 'F')))
         buf[pos] = ' ';
   }

   /* VBA format: has colon */
   if (hasColon) {
      pos = 0;
      while (pos < codelen) {
         /* Find next non-space */
         while (pos < codelen && buf[pos] == ' ') pos++;
         if (pos >= codelen) break;

         char *line = buf + pos;
         /* Find end of this line (next newline, or until the next VBA pattern) */
         while (pos < codelen && buf[pos] != '\n' && buf[pos] != '\r') pos++;
         buf[pos] = '\0';

         cheats_parse_vba_line(ch, line);
      }
      ch->cheat_active = true;
      cheats_update_hooks();
      return CheatNoError;
   }

   /* Try GameShark/PARv3 (two 32-bit words) or CodeBreaker (32-bit + 16-bit) */
   pos = 0;
   while (pos < codelen) {
      while (pos < codelen && buf[pos] == ' ') pos++;
      if (pos >= codelen) break;

      u32 op1 = 0, op2;
      u16 op2short;
      int chars;

      /* Try "XXXXXXXX YYYYYYYY" (GS/PARv3) */
      if (2 == sscanf(&buf[pos], "%08x %08x%n", &op1, &op2, &chars)) {
         cheats_parse_autodetect(ch, op1, op2);
         pos += chars;
      }
      /* Try "XXXXXXXX YYYY" (CodeBreaker) */
      else if (2 == sscanf(&buf[pos], "%08x %04hx%n", &op1, &op2short, &chars)) {
         cheats_parse_codebreaker_raw(ch, op1, op2short);
         pos += chars;
      } else {
         break;
      }

      /* Skip trailing whitespace */
      while (pos < codelen && buf[pos] == ' ') pos++;
   }

   if (pos >= codelen) {
      ch->cheat_active = true;
      cheats_update_hooks();
      return CheatNoError;
   }

   return CheatErrorNotSupported;
}
