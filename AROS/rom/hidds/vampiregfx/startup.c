/*
    Copyright © 2017, The Apollo Team. All rights reserved.
*/

#define DEBUG 1

#include <aros/debug.h>
#include <aros/symbolsets.h>
#include <graphics/driver.h>
#include <graphics/gfxbase.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/oop.h>

#include "vampiregfx_intern.h"
#include "vampiregfx_bitmap.h"

#include LC_LIBDEFS_FILE

BOOL Init_VampireGFXClass(LIBBASETYPEPTR LIBBASE);

#undef SysBase
#undef OOPBase

static int VampireGFX_Init(LIBBASETYPEPTR LIBBASE)
{
    ULONG err;
    struct ExecBase *SysBase = (LIBBASE)->csd.cs_SysBase;
    struct Library  *GfxBase = TaggedOpenLibrary(TAGGEDOPEN_GRAPHICS);
    struct Library  *OOPBase = OpenLibrary("oop.library", 0);

    D(bug("**************************** VampireGFX_Init ******************************\n"));

    if (!GfxBase)
        return FALSE;

    LIBBASE->csd.basebm = OOP_FindClass(CLID_Hidd_BitMap);
    CloseLibrary(OOPBase);

    if (!Init_VampireGFXClass(LIBBASE)) {
        CloseLibrary(GfxBase);
        return FALSE;
    }

    LIBBASE->library.lib_OpenCnt = 1;

    err = AddDisplayDriver(LIBBASE->csd.gfxclass, NULL,
			   DDRV_KeepBootMode, TRUE,
			   DDRV_IDMask      , 0xF0000000,
			   TAG_DONE);

    CloseLibrary(GfxBase);

    D(bug("VampireHIDD AddDisplayDriver() result: %u\n", err));
    return err ? FALSE : TRUE;
}

ADD2INITLIB(VampireGFX_Init, 0)
