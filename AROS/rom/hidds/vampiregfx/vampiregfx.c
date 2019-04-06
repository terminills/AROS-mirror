/*
    Copyright © 2016, Michael Ness. All rights reserved.
 */

#include <exec/libraries.h>
#include <exec/rawfmt.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/displayinfo.h>
#include <intuition/intuitionbase.h>
#include <aros/libcall.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/kernel.h>
#include <proto/oop.h>
#include <proto/utility.h>
#include <oop/oop.h>

#include <hidd/hidd.h>
#include <hidd/graphics.h>

#include <aros/symbolsets.h>

#include LC_LIBDEFS_FILE

#include "vampiregfx.h"
#include "vampiregfxbitmap.h"

#define SDEBUG 0
#define DEBUG 1
#define DRTG(x) x

#include <aros/debug.h>

HIDDT_ModeID *VampireGFXCl__Hidd_Gfx_QueryModeIDs(OOP_Class *cl, OOP_Object *o, stuct pHidd_Gfx_QueryModeIDs *msg)
{

}

VOID VampireGFXCl__Hidd_Gfx__ReleaseModeIDs(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_ReleaseModeIDs *msg)
{

}

HIDDT_ModeID VampireGFXCl__Hidd_Gfx__NextModeID(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_NextModeID *msg)
{

}

BOOL VampireGFXCl__Hidd_Gfx__GetMode(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_GetMode *msg)
{

}

OOP_Object *VampireGFXCl__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{

}

/********** GfxHidd::Dispose()  ******************************/
OOP_Object *VampireGFXCl__Hidd_Gfx__CreateObject(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CreateObject *msg)
{

}

VOID VampireGFXCl__Root__Get(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{

}

VOID VampireGFXCl__Root__Set(OOP_Class *cl, OOP_Object *obj, struct pRoot_Set *msg)
{

}

static void doshow(struct uaegfx_staticdata *csd, OOP_Object *bm, struct ViewPort *vp, BOOL offonly)
{

}

OOP_Object *VampireGFXCl__Hidd_Gfx__Show(OOP_Class *cl, OOP_Object *c, struct pHidd_Gfx_Show *msg)
{

}

ULONG VampireGFXCl__Hidd_Gfx__PrepareViewPorts(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_ShowViewPorts *msg)
{

}

ULONG VampireGFXCl__Hidd_Gfx__ShowViewPorts(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_ShowViewPorts *msg)
{

}

VOID VampireGFXCl__Hidd_Gfx__CopyBox(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CopyBox *msg)
{

}

BOOL VampireGFXCl__Hidd_Gfx__CopyBoxMasked(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CopyBoxMasked *msg)
{

}

BOOL VampireGFXCl__Hidd_Gfx__SetCursorShape(OOP_Class *cl, OOP_Object *shape, struct pHidd_Gfx_SetCursorShape *msg)
{

}

BOOL VampireGFXCl__Hidd_Gfx__SetCursorPos(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorPos *msg)
{

}

VOID VampireGFXCl__Hidd_Gfx__SetCursorVisible(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_SetCursorVisible *msg)
{

}

BOOL VampireGFXCl__Hidd_Gfx__CheckMode(OOP_Class *cl, OOP_Object *o, struct pHidd_Gfx_CheckMode *msg)
{

}

static void freeattrbases(LIBBASETYPEPTR LIBBASE, struct vampiregfx_staticdata *csd)
{

}

AROS_INTP(rtg_vblank);

/* real RTG only */
static BOOL PopulateModeInfo(struct vampiregfx_staticdata *csd, struct LibResolution *res, const struct P96RTGmode *mode)
{

}

static void PopulateResolutionList(struct vampiregfx_staticdata *csd)
{

}

static int openall(struct vampiregfx_staticdata *csd)
{

}

static void freeall(struct vampiregfx_staticdata *csd)
{

}

BOOL Init_VampireGFXClass(LIBBASETYPEPTR LIBBASE)
{

}

static int Expunge_VampireGFXClass(LIBBASETYPEPTR LIBBASE)
{

}

ADD2EXPUNGELIB(Expunge_VampireGFXClass, 1)

#undef SysBase

AROS_INTH1(rtg_vblank, APTR, boardinfo)
{
    AROS_INTFUNC_INIT

    return 0;

    AROS_INTFUNC_EXIT
}
