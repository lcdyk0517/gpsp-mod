#ifndef LIBRETRO_CORE_OPTIONS_INTL_H__
#define LIBRETRO_CORE_OPTIONS_INTL_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* https://support.microsoft.com/en-us/kb/980263 */
#pragma execution_character_set("utf-8")
#pragma warning(disable:4566)
#endif

#include <libretro.h>

/*
 ********************************
 * VERSION: 1.3
 ********************************
 *
 * - 1.3: Move translations to libretro_core_options_intl.h
 *        - libretro_core_options_intl.h includes BOM and utf-8
 *          fix for MSVC 2010-2013
 *        - Added HAVE_NO_LANGEXTRA flag to disable translations
 *          on platforms/compilers without BOM support
 * - 1.2: Use core options v1 interface when
 *        RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION is >= 1
 *        (previously required RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION == 1)
 * - 1.1: Support generation of core options v0 retro_core_option_value
 *        arrays containing options with a single value
 * - 1.0: First commit
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_JAPANESE */

/* RETRO_LANGUAGE_FRENCH */

/* RETRO_LANGUAGE_SPANISH */

/* RETRO_LANGUAGE_GERMAN */

/* RETRO_LANGUAGE_ITALIAN */

/* RETRO_LANGUAGE_DUTCH */

/* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */

/* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */

/* RETRO_LANGUAGE_RUSSIAN */

/* RETRO_LANGUAGE_KOREAN */

/* RETRO_LANGUAGE_CHINESE_TRADITIONAL */

struct retro_core_option_definition option_defs_tw[] = {
   {
      "gpsp_bios",
      "BIOS",
      "選擇使用的 BIOS 映像檔。官方 BIOS 需由使用者自行提供。使用非官方 (或內建) BIOS 可能導致部分遊戲相容性問題。使用官方任天堂 BIOS 可獲得最佳效果。",
      {
         { "auto",      "自動選擇" },
         { "builtin",   "內建 BIOS" },
         { "official",  "官方 BIOS" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "gpsp_boot_mode",
      "啟動模式",
      "選擇在啟動遊戲前是否先啟動 BIOS。兩種模式差異不大。",
      {
         { "game",      "直接啟動遊戲" },
         { "bios",      "先啟動 BIOS" },
         { NULL, NULL },
      },
      "game"
   },
   {
      "gpsp_rumble",
      "震動支援",
      "啟用或停用卡匣震動模擬 (GPIO、EZ-Flash 及 GB Player)。",
      {
         { "enabled",  "啟用" },
         { "disabled", "停用" },
         { NULL, NULL },
      },
      "enabled"
   },
#if defined(HAVE_DYNAREC)
   {
      "gpsp_drc",
      "動態重編譯器",
      "將 CPU 指令動態重編譯為原生指令。大幅提升效能，但可能降低精確度。",
      {
         { "enabled",  "啟用" },
         { "disabled", "停用" },
         { NULL, NULL },
      },
      "enabled"
   },
#endif
   {
      "gpsp_sprlim",
      "無精靈限制",
      "移除原始硬體每行掃描線的精靈數量限制。部分遊戲可能依賴此限制來正常顯示 (例如隱藏精靈)。",
      {
         { "enabled",  "啟用" },
         { "disabled", "停用" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_rtc",
      "RTC 支援",
      "設定模擬卡匣的 RTC (即時時鐘) 支援。自動偵測使用 ROM 資料庫，適用於大多數商業遊戲。使用自製程式或 ROM Hack 時可能需要強制啟用 RTC。",
      {
         { "auto",     "自動" },
         { "enabled",  "啟用" },
         { "disabled", "停用" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "gpsp_serial",
      "連線對戰",
      "設定序列埠 (連線線) 的連接模式。自動選項會為大多數已知的遊戲選擇合適的支援。",
      {
         { "auto",     "自動" },
         { "disabled", "停用" },
         { "rfu",      "GBA 無線適配器" },
         { "mul_poke", "連線線 - 寶可夢三代模式" },
         { "mul_aw1",  "連線線 - 大戰 1 模式" },
         { "mul_aw2",  "連線線 - 大戰 2 模式" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "gpsp_frameskip",
      "跳幀",
      "跳過幀以避免音訊緩衝區不足 (爆音)。以視覺流暢度為代價提升效能。'自動' 在前端建議時跳幀。'自動 (閾值)' 使用 '跳幀閾值 (%)' 設定。'固定間隔' 使用 '跳幀間隔' 設定。",
      {
         { "disabled",       "停用" },
         { "auto",           "自動" },
         { "auto_threshold", "自動 (閾值)" },
         { "fixed_interval", "固定間隔" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_frameskip_threshold",
      "跳幀閾值 (%)",
      "當 '跳幀' 設為 '自動 (閾值)' 時，指定音訊緩衝區佔用率低於此百分比將觸發跳幀。數值越高越能減少爆音，但幀率下降更頻繁。",
      {
         { "15", NULL },  { "18", NULL },  { "21", NULL },
         { "24", NULL },  { "27", NULL },  { "30", NULL },
         { "33", NULL },  { "36", NULL },  { "39", NULL },
         { "42", NULL },  { "45", NULL },  { "48", NULL },
         { "51", NULL },  { "54", NULL },  { "57", NULL },
         { "60", NULL },
         { NULL, NULL },
      },
      "33"
   },
   {
      "gpsp_frameskip_interval",
      "跳幀間隔",
      "當 '跳幀' 設為 '固定間隔' 時，此值為渲染一幀後略過的幀數 - 即 '0' = 60fps, '1' = 30fps, '2' = 15fps 等。",
      {
         { "0", NULL }, { "1", NULL }, { "2", NULL }, { "3", NULL },
         { "4", NULL }, { "5", NULL }, { "6", NULL }, { "7", NULL },
         { "8", NULL }, { "9", NULL }, { "10", NULL },
         { NULL, NULL },
      },
      "1"
   },
   {
      "gpsp_color_correction",
      "色彩校正",
      "調整輸出色彩以符合真實 GBA 硬體的顯示效果。",
      {
         { "enabled",  "啟用" },
         { "disabled", "停用" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_frame_mixing",
      "幀混合",
      "透過將當前幀與前一幀 50:50 混合來模擬 LCD 殘影效果。遊玩利用此效果實現透明效果 (如 F-Zero、我們的太陽系列等) 的遊戲時需要啟用。",
      {
         { "enabled",  "啟用" },
         { "disabled", "停用" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_turbo_period",
      "連發按鈕間隔",
      "設定按住 Turbo A/B 按鈕時的重複間隔 (幀數)。",
      {
         { "4", NULL },   { "5", NULL },  { "6", NULL },
         { "7", NULL },   { "8", NULL },  { "9", NULL },
         { "10", NULL },  { "11", NULL }, { "12", NULL },
         { "13", NULL },  { "14", NULL }, { "15", NULL },
         { "16", NULL },  { "17", NULL }, { "18", NULL },
         { "19", NULL },  { "20", NULL }, { "21", NULL },
         { "22", NULL },  { "23", NULL }, { "24", NULL },
         { "25", NULL },  { "26", NULL }, { "27", NULL },
         { "28", NULL },  { "29", NULL }, { "30", NULL },
         { NULL, NULL },
      },
       "4"
    },
    {
       "gpsp_solar_sensor",
       "太陽能感應器等級",
       "設定《我們的太陽》系列遊戲的環境光線強度。遊戲中可使用 L3/R3 按鈕調整。",
       {
          { "disabled", "停用" },
          { "0",  "最亮" },
          { "1",  NULL },
          { "2",  NULL },
          { "3",  NULL },
          { "4",  NULL },
          { "5",  "中等" },
          { "6",  NULL },
          { "7",  NULL },
          { "8",  NULL },
          { "9",  NULL },
          { "10", "最暗" },
          { NULL, NULL },
       },
       "disabled"
    },
    { NULL, NULL, NULL, {{0}}, NULL },
};

/* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */

struct retro_core_option_definition option_defs_cn[] = {
   {
      "gpsp_bios",
      "BIOS",
      "选择使用的 BIOS 映像文件。官方 BIOS 需由用户自行提供。使用非官方 (或内置) BIOS 可能导致部分游戏兼容性问题。使用官方任天堂 BIOS 可获得最佳效果。",
      {
         { "auto",      "自动选择" },
         { "builtin",   "内置 BIOS" },
         { "official",  "官方 BIOS" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "gpsp_boot_mode",
      "启动模式",
      "选择在启动游戏前是否先启动 BIOS。两种模式差异不大。",
      {
         { "game",      "直接启动游戏" },
         { "bios",      "先启动 BIOS" },
         { NULL, NULL },
      },
      "game"
   },
   {
      "gpsp_rumble",
      "震动支持",
      "启用或禁用卡匣震动模拟 (GPIO、EZ-Flash 及 GB Player)。",
      {
         { "enabled",  "启用" },
         { "disabled", "禁用" },
         { NULL, NULL },
      },
      "enabled"
   },
#if defined(HAVE_DYNAREC)
   {
      "gpsp_drc",
      "动态重编译器",
      "将 CPU 指令动态重编译为原生指令。大幅提升性能，但可能降低精确度。",
      {
         { "enabled",  "启用" },
         { "disabled", "禁用" },
         { NULL, NULL },
      },
      "enabled"
   },
#endif
   {
      "gpsp_sprlim",
      "无精灵限制",
      "移除原始硬件每行扫描线的精灵数量限制。部分游戏可能依赖此限制来正常显示 (例如隐藏精灵)。",
      {
         { "enabled",  "启用" },
         { "disabled", "禁用" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_rtc",
      "RTC 支持",
      "设定模拟卡匣的 RTC (实时时钟) 支持。自动检测使用 ROM 数据库，适用于大多数商业游戏。使用自制程序或 ROM Hack 时可能需要强制启用 RTC。",
      {
         { "auto",     "自动" },
         { "enabled",  "启用" },
         { "disabled", "禁用" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "gpsp_serial",
      "联机对战",
      "设定串行端口 (联机线) 的连接模式。自动选项会为大多数已知的游戏选择合适的支持。",
      {
         { "auto",     "自动" },
         { "disabled", "禁用" },
         { "rfu",      "GBA 无线适配器" },
         { "mul_poke", "联机线 - 宝可梦三代模式" },
         { "mul_aw1",  "联机线 - 大战争 1 模式" },
         { "mul_aw2",  "联机线 - 大战争 2 模式" },
         { NULL, NULL },
      },
      "auto"
   },
   {
      "gpsp_frameskip",
      "跳帧",
      "跳过帧以避免音频缓冲区不足 (爆音)。以视觉流畅度为代价提升性能。'自动' 在前端建议时跳帧。'自动 (阈值)' 使用 '跳帧阈值 (%)' 设置。'固定间隔' 使用 '跳帧间隔' 设置。",
      {
         { "disabled",       "禁用" },
         { "auto",           "自动" },
         { "auto_threshold", "自动 (阈值)" },
         { "fixed_interval", "固定间隔" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_frameskip_threshold",
      "跳帧阈值 (%)",
      "当 '跳帧' 设为 '自动 (阈值)' 时，指定音频缓冲区占用率低于此百分比将触发跳帧。数值越高越能减少爆音，但帧率下降更频繁。",
      {
         { "15", NULL },  { "18", NULL },  { "21", NULL },
         { "24", NULL },  { "27", NULL },  { "30", NULL },
         { "33", NULL },  { "36", NULL },  { "39", NULL },
         { "42", NULL },  { "45", NULL },  { "48", NULL },
         { "51", NULL },  { "54", NULL },  { "57", NULL },
         { "60", NULL },
         { NULL, NULL },
      },
      "33"
   },
   {
      "gpsp_frameskip_interval",
      "跳帧间隔",
      "当 '跳帧' 设为 '固定间隔' 时，此值为渲染一帧后跳过的帧数 - 即 '0' = 60fps, '1' = 30fps, '2' = 15fps 等。",
      {
         { "0", NULL }, { "1", NULL }, { "2", NULL }, { "3", NULL },
         { "4", NULL }, { "5", NULL }, { "6", NULL }, { "7", NULL },
         { "8", NULL }, { "9", NULL }, { "10", NULL },
         { NULL, NULL },
      },
      "1"
   },
   {
      "gpsp_color_correction",
      "色彩校正",
      "调整输出色彩以符合真实 GBA 硬件的显示效果。",
      {
         { "enabled",  "启用" },
         { "disabled", "禁用" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_frame_mixing",
      "帧混合",
      "通过将当前帧与前一帧 50:50 混合来模拟 LCD 残影效果。游玩利用此效果实现透明效果 (如 F-Zero、我们的太阳系列等) 的游戏时需要启用。",
      {
         { "enabled",  "启用" },
         { "disabled", "禁用" },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "gpsp_turbo_period",
      "连发按钮间隔",
      "设置按住 Turbo A/B 按钮时的重复间隔 (帧数)。",
      {
         { "4", NULL },   { "5", NULL },  { "6", NULL },
         { "7", NULL },   { "8", NULL },  { "9", NULL },
         { "10", NULL },  { "11", NULL }, { "12", NULL },
         { "13", NULL },  { "14", NULL }, { "15", NULL },
         { "16", NULL },  { "17", NULL }, { "18", NULL },
         { "19", NULL },  { "20", NULL }, { "21", NULL },
         { "22", NULL },  { "23", NULL }, { "24", NULL },
         { "25", NULL },  { "26", NULL }, { "27", NULL },
         { "28", NULL },  { "29", NULL }, { "30", NULL },
         { NULL, NULL },
      },
       "4"
    },
    {
       "gpsp_solar_sensor",
       "太阳能传感器等级",
       "设置《我们的太阳》系列游戏的环境光线强度。游戏中可使用 L3/R3 按钮调整。",
       {
          { "disabled", "禁用" },
          { "0",  "最亮" },
          { "1",  NULL },
          { "2",  NULL },
          { "3",  NULL },
          { "4",  NULL },
          { "5",  "中等" },
          { "6",  NULL },
          { "7",  NULL },
          { "8",  NULL },
          { "9",  NULL },
          { "10", "最暗" },
          { NULL, NULL },
       },
       "disabled"
    },
    { NULL, NULL, NULL, {{0}}, NULL },
};

/* RETRO_LANGUAGE_ESPERANTO */

/* RETRO_LANGUAGE_POLISH */

/* RETRO_LANGUAGE_VIETNAMESE */

/* RETRO_LANGUAGE_ARABIC */

/* RETRO_LANGUAGE_GREEK */

/* RETRO_LANGUAGE_TURKISH */

/* RETRO_LANGUAGE_SLOVAK */

/* RETRO_LANGUAGE_PERSIAN */

/* RETRO_LANGUAGE_HEBREW */

/* RETRO_LANGUAGE_ASTURIAN */

#ifdef __cplusplus
}
#endif

#endif
