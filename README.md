New Features (this fork)
========================

### Unicode ROM Path & 中文支持
 - need_fullpath=false + VFS 临时文件，绕过中文 zip 路径编码问题
 - load_gamepak_from_data 优先从内存加载 ROM，避免文件路径乱码
 - 简/繁体中文核心选项翻译 (libretro_core_options_intl.h)

### 统一震动系统 (GPIO / GBP / EZ-Flash)
 - 震动默认启用，运行时即时生效，去掉"自动"选项
 - EZ-Flash 7 态状态机（对齐 mgba），200ms 脉冲/持续模式
 - GBP 震动默认支持（rumble_enabled 时自动响应）
 - rumble_enabled 统一控制 GPIO/GBP/EZ 三条路径，关闭时强制熄火
 - 核心选项列表中震动置顶

### 太阳能传感器 (Boktai 系列)
 - 移植 mgba 的 _lightReadPins 计数/阈值逻辑
 - 核心选项设置初始亮度 (0=最亮, 10=最暗)
 - L3/R3 运行时手动调整 (与 mgba 一致)
 - 支持《我们的太阳》系列、《暗影之屋》等使用 GPIO 光感的游戏

### 金手指系统重写
 - 移植 mgba 的 GameShark / PARv3 / CodeBreaker 解密引擎
 - TEA / GSAv1 / CodeBreaker 解密 + LCG 随机数
 - 格式解析器：支持 GameShark / PARv3 / CodeBreaker / VBA 四种格式
 - 自动检测：概率评分识别 4 种金手指变体
 - 多钩子地址支持（cheats_is_hook_address / cheats_process_hook）
 - extern 声明整理，libretro 构建集成

-----------------------------------------------------------------------

gpSP for libretro
=================

This is a fork of notaz's gpSP (https://github.com/notaz/gpsp) for libretro
frontends (like Retroarch). Check the original_readme.txt file for more info.

The current maintainer/main contributor is davidgfnet (check out the repo at
https://github.com/davidgfnet/gpsp). This version has a bunch of fixes and
features.

Feature list
===========

gpSP features a dynamic recompiler that makes it quite fast (compared to other
emulators at least). It supports x86/x64, ARMv6/7 and ARMv8 and MIPS (32 and 64
bits), for other platforms an interpreter is available (albeit slower). Both
little and big endian systems are supported. Some supported platforms are PSP,
PS2, GameCube/Wii, Nintendo 3DS and Switch, Dingux/OpenDingux and of course
PC and Android.

At the moment this emulator lacks a native UI and must be played using some
libretro frontend (we recommend Retroarch). A list of available frontends can
be found at https://docs.libretro.com/development/frontends/

Many new features (compared to the original release) are:

 - Wireless Adapter networked multiplayer!
 - Rumble support (including Gameboy Player emulation)
 - New video renderer, fixes many graphical bugs & adds many effects (mosaic).
 - Many long standing issues have been fixed.
 - Slightly better performance (for some games at least!)
 - Better audio (fixed many audio related bugs).
 - Ships an opensource BIOS replacement,we recommend using the original though.

Planned features (aka the TODO list)
====================================

Some features I'd like to see (in loose priority order):

 - GBA link emulation (for some games, perhaps with patches).
 - Improve RFU (Wireless Adapter) emulation through research.
 - Bringing back the native UI for PC, PSP and perhaps PS2/3DS/Wii.
 - A native UI with Multiplayer support for portable devices with wifi support.
 - A better BIOS emulation and perhaps a newer better open BIOS.
 - Dynarec rewrite: make it easier to add new drcs and share more code.
 - Adding some funny DRCs like PowerPC or SH4.


