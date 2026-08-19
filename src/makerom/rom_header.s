#include "rom_header.h"
#include "ultra64/version.h"
#include "region.h"
#include "versions.h"

/* 0x00 */ ENDIAN_IDENTIFIER
/* 0x01 */ PI_DOMAIN_1_CFG(64, 18, 7, 3)
/* 0x04 */ SYSTEM_CLOCK_RATE_SETTING(0xF)
/* 0x08 */ ENTRYPOINT(entrypoint)
/* 0x0C */ OS_VERSION(2, 0, LIBULTRA_VERSION)
/* 0x10 */ CHECKSUM()

#if !PLATFORM_IQUE

/* 0x18 */ PADDING(8)
/* 0x20 */ ROM_NAME("THE LEGEND OF ZELDA")
/* 0x34 */ PADDING(7)
#if OOT_NTSC
/* 0x3B */ MEDIUM(CARTRIDGE_EXPANDABLE)
#else
/* 0x3B */ MEDIUM(CARTRIDGE)
#endif
// START: MODIFIED
/* 0x3C */ GAME_ID("ZL")
/* 0x3E */ REGION(US)
/* 0x3F */ GAME_REVISION(OOT_REVISION)
// END: MODIFIED; Removed Non-US headers because of performance issues

#else

    .fill 0x40 - 0x18

#endif
