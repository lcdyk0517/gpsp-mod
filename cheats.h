/* gameplaySP - Extended cheat system (ported from mgba)
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 * Portions Copyright (C) 2013-2016 Jeffrey Pfau (mgba)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#ifndef __GPSP_CHEATS_H__
#define __GPSP_CHEATS_H__

#include "common.h"

#define MAX_CHEATS        100
#define MAX_CHEAT_CODES   256
#define MAX_HOOK_ADDRS    8

/* Cheat format types */
typedef enum {
   CHEAT_FORMAT_AUTODETECT,
   CHEAT_FORMAT_CODEBREAKER,
   CHEAT_FORMAT_GAMESHARK,
   CHEAT_FORMAT_PARV3,
   CHEAT_FORMAT_VBA
} cheat_format_type;

/* GameShark version */
typedef enum {
   GSA_VERSION_GSAV1 = 1,
   GSA_VERSION_GSAV1_RAW = 2,
   GSA_VERSION_PARV3 = 3,
   GSA_VERSION_PARV3_RAW = 4
} gsa_version_type;

/* Abstract cheat operation types */
typedef enum {
   CHEAT_ASSIGN,
   CHEAT_ASSIGN_INDIRECT,
   CHEAT_AND,
   CHEAT_ADD,
   CHEAT_OR,
   CHEAT_IF_EQ,
   CHEAT_IF_NE,
   CHEAT_IF_LT,
   CHEAT_IF_GT,
   CHEAT_IF_ULT,
   CHEAT_IF_UGT,
   CHEAT_IF_AND,
   CHEAT_IF_LAND,
   CHEAT_IF_NAND,
   CHEAT_IF_BUTTON,
   CHEAT_NEVER,
   /* Internal marker: used for hook registration, not a real cheat type */
   CHEAT_TYPE_HOOK = 255
} cheat_op_type;

typedef struct {
   cheat_op_type type;
   u32 address;
   u32 operand;
   u16 repeat;
   u16 negativeRepeat;
   s32 addressOffset;
   s32 operandOffset;
   u8 width;       /* 1, 2 or 4 */
   u8 applied;     /* For ROM patches: is it currently applied */
} cheat_code;

/* ROM patch entry */
typedef struct {
   u32 address;
   u32 value;
   u32 originalValue;
   u8 width;
   u8 applied;
   u8 checkActive;
   u32 checkValue;
} cheat_rom_patch;

/* Per-cheat-set state */
typedef struct {
   bool cheat_active;
   cheat_code codes[MAX_CHEAT_CODES];
   struct {
      u32 originalValue;
      u32 key;
   } unpatched[MAX_CHEAT_CODES];
   unsigned cheat_count;

   /* ROM patches */
   cheat_rom_patch patches[MAX_CHEAT_CODES];
   unsigned patch_count;

   /* Format detection state */
   cheat_format_type format;
   gsa_version_type gsaVersion;
   u32 gsaSeeds[4];

   /* CodeBreaker state */
   u32 cbRngState;
   u32 cbMaster;
   u8  cbTable[0x30];
   u32 cbSeeds[4];

   /* Multi-hook support */
   u32 hookAddresses[MAX_HOOK_ADDRS];
   u8  hookCount;

   /* PARv3 block tracking */
   u16 currentBlock;
   s16 incompleteCheat;
   s16 remainingAddresses;
} cheat_type;

extern cheat_type cheats[MAX_CHEATS];
extern u32 max_cheat;
extern u32 cheat_hook_addresses[MAX_HOOK_ADDRS * MAX_CHEATS];
extern u32 cheat_hook_count;

/* Error codes */
typedef enum {
   CheatNoError = 0,
   CheatErrorTooMany,
   CheatErrorTooBig,
   CheatErrorEncrypted,
   CheatErrorNotSupported
} cheat_error;

void process_cheats(void);
cheat_error cheat_parse(unsigned index, const char *code);
void cheat_clear(void);
void cheat_disable(unsigned index);

/* Hook management */
void cheats_update_hooks(void);
bool cheats_is_hook_address(u32 pc);
void cheats_process_hook(u32 pc);

/* Format-specific parsers (in cheats_parse.c) */
cheat_error cheats_parse_gameshark(cheat_type *ch, u32 op1, u32 op2);
cheat_error cheats_parse_parv3(cheat_type *ch, u32 op1, u32 op2);
cheat_error cheats_parse_codebreaker_raw(cheat_type *ch, u32 op1, u16 op2);
cheat_error cheats_parse_vba_line(cheat_type *ch, const char *line);
cheat_error cheats_parse_autodetect(cheat_type *ch, u32 op1, u32 op2);

/* Decryption utilities (in cheats_parse.c) */
void cheats_decrypt_gameshark(u32 *op1, u32 *op2, const u32 *seeds);
void cheats_reseed_gameshark(u32 *seeds, u16 params, const u8 *t1, const u8 *t2);
void cheats_decrypt_codebreaker(cheat_type *ch, u32 *op1, u16 *op2);
void cheats_reseed_codebreaker(cheat_type *ch, u32 op1, u16 op2);

#endif
