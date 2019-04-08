/*
    Copyright © 2017-2019, The Apollo Team. All rights reserved.
*/

#define DEBUG 1

#include <aros/debug.h>
#include <aros/symbolsets.h>
#include <graphics/driver.h>
#include <graphics/gfxbase.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/oop.h>

#include "vampiregfx_video.h"
#include "vampiregfx_intern.h"
#include "vampiregfx_bitmap.h"

#include LC_LIBDEFS_FILE

UBYTE BOARDID = 0;
BOOL Init_VampireGFXClass(LIBBASETYPEPTR LIBBASE);

#undef SysBase
#undef OOPBase

static int VampireGFX_Init(LIBBASETYPEPTR LIBBASE)
{
    ULONG err = 0;
    int result = FALSE;

    struct ExecBase *SysBase = (LIBBASE)->csd.cs_SysBase;
    struct Library  *OOPBase;
    LIBBASE->csd.cs_GfxBase = TaggedOpenLibrary(TAGGEDOPEN_GRAPHICS);
    struct Library  *GfxBase = LIBBASE->csd.cs_GfxBase; 
    struct IORequest io;

    D(bug("**************************** VampireGFX_Init ******************************\n"));

    if (GfxBase)
    {
        OOPBase = OpenLibrary("oop.library", 0);
        if (OOPBase)
        {
            LIBBASE->csd.basebm = OOP_FindClass(CLID_Hidd_BitMap);
            CloseLibrary(OOPBase);
        }

        if (Init_VampireGFXClass(LIBBASE))
        {
            LIBBASE->library.lib_OpenCnt = 1;

            BOARDID = SAGA_BOARD_GET();

            if (BOARDID != SAGA_BOARD_Unknown &&
                BOARDID < SAGA_BOARD_Future)
            {
                //////////////////////////////////////////////////////
                // If the user is holding down SHIFT, then the driver don't load.
                //////////////////////////////////////////////////////

                if (0 == OpenDevice("input.device", 0, &io, 0))
                {
                    struct Library *InputBase = (struct Library *)io.io_Device;
                    UWORD qual = PeekQualifier();

                    CloseDevice(&io);

                    if (!(qual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)))
                    {
                        D(bug("[HiddVampireGfx] Init: Looks good, Installing driver\n"));
                        err = AddDisplayDriver(LIBBASE->csd.gfxclass, NULL,
		                               DDRV_KeepBootMode, TRUE,
	                                       DDRV_IDMask      , 0xF0000000,
			                       TAG_DONE);

                        D(bug("[HiddVampireGfx] AddDisplayDriver() result: %u\n", err));

                        if (!err)
                        {
                            LIBBASE->library.lib_OpenCnt = 1;
                            result = TRUE;
                        }
                    }
                }
            }
        }

        CloseLibrary(GfxBase);
    }
    else
        D(bug("[HiddVampireGfx] Failed to open graphics.library!\n"));

    return result;
}

ADD2INITLIB(VampireGFX_Init, 0)
