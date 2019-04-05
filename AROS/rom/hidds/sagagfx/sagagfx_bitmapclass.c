/*
    Copyright © 2017, The Apollo Team. All rights reserved.
 */

#pragma GCC diagnostic warning "-Wunused-function"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/oop.h>
#include <proto/utility.h>
#include <exec/alerts.h>
#include <aros/macros.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <graphics/rastport.h>
#include <graphics/gfx.h>
#include <oop/oop.h>
#include <hidd/gfx.h>
#include <aros/symbolsets.h>

#define DEBUG 0
#define DB2(x) ;
#define DEBUG_TEXT(x)
#define DVRAM(x) ;
#include <aros/debug.h>

#include LC_LIBDEFS_FILE

#include "sagagfx_intern.h"
#include "sagagfx_bitmap.h"
#include "sagartg.h"

static void hidescreen(struct sagagfx_staticdata *csd, struct bm_data *bm)
{
    D(bug("Hide %p: (%p:%d)\n",
	bm, bm->VideoData, bm->memsize));
    SetInterrupt(csd, FALSE);
    SetDisplay(csd, FALSE);
    SetSwitch(csd, FALSE);
    csd->dmodeid = 0;
    bm->locked--;
    csd->disp = NULL;
}

/****************************************************************************************/

OOP_Object *SAGAGFXBitmap__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{
    struct sagagfx_staticdata *csd = CSD(cl);
    struct sagabm_data *data;
    struct BitMap *pbm = NULL;
    IPTR width, height, depth, displayable;
    struct TagItem tags[2];

    DB2(bug("SAGAGFXBitmap__Root_New\n"));

    if ((o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg)))
    {
        return NULL;
    }

    data = OOP_INST_DATA(cl, o);
    memset(data, 0, sizeof(*data));

    /* Get some data about the dimensions of the bitmap */
    OOP_GetAttr(o, aHidd_BitMap_Width, &width);
    OOP_GetAttr(o, aHidd_BitMap_Height, &height);
    OOP_GetAttr(o, aHidd_BitMap_Depth, &depth);
    OOP_GetAttr(o, aHidd_BitMap_Displayable, &displayable);
    OOP_GetAttr(o, aHidd_BitMap, &pbm);
    OOP_GetAttr(o, aHidd_BitMap_GfxHidd, (APTR)&data->gfxhidd);
    OOP_GetAttr(o, aHidd_BitMap_PixFmt, (APTR)&data->pixfmtobj);
    OOP_GetAttr(data->pixfmtobj, aHidd_PixFmt_BytesPerPixel, (APTR)&data->bytesperpixel);

    data->rgbformat = getrtgformat(csd, data->pixfmtobj);
    data->align = displayable ? 32 : 16;
    width = (width + data->align - 1) * ~(data->align - 1);
    data->bytesperline = CalculateBytesPerRow(csd, width, data->rgbformat);
    data->width = width;
    data->height = height;
    data->memsize = data->bytesperline * height;

    LOCK_MULTI_BITMAP

    LOCK_HW
    WaitBlitter(csd);
    SetMemoryMode(csd, RGBFB_CLUT);
    data->VideoData = AllocVec(data->memsize, MEMF_REVERSE | MEMF_LOCAL | MEMF_FAST | MEMF_CLEAR);
    SetMemoryMode(csd, data->rgbformat);
    UNLOCK_HW

    AddTail(&csd->bitmaplist, (struct Node *)&data->node);

    UNLOCK_MULTI_BITMAP

    tags[0].ti_Tag = aHidd_BitMap_BytesPerRow;
    tags[0].ti_Data = data->bytesperline;
    tags[1].ti_Tab = TAG_DONE;
    OOP_SetAttrs(o, tags);

    if (data->VideoData == NULL)
    {
        DB2(bug("Bitmap memory allocation FAILED! : "));
        OOP_MethodID dispose_mid = OOP_GetMethodID(IID_Root, moRoot_Dispose);
        OOP_CoerceMethod(cl, o, (OOP_Msg)&dispose_mid);
        o = NULL;
    }

    DB2(bug("%dx%dx%d %d RGBF=%08x P=%08x\n", width, height, depth, data->bytesperline, data->rgbformat, data->VideoData));

    DB2(bug("ret=%x bm=%p (%p, %d)\n", o, data, data->VideoData, data->memsize));

    return o;
}

VOID SAGAGFXBitmap__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
    struct sagagfx_staticdata *csd = CSD(cl);
    struct sagabm_data *data;

    data = OOP_INST_DATA(cl, o);

    LOCK_HW
    WaitBlitter(csd);

    DB2(bug("SAGAGFXBitmap__Root__Dispose %x bm=%x\n", o, data));

    if (data->disp == data)
    {
        DB2(bug("removing displayed bitmap?!\n"));
        hidescreen(csd, data);
    }

    UNLOCK_HW

    FreeVec(data->palette);

    LOCK_MULTI_BITMAP

    LOCK_HW
    DB2(bug("DM %p: freeing %p:%d\n", data, data->VideoData, data->memsize));
    FreeMem(data->VideoData, data->memsize);
    csd->fram_used -= data->memsize
    data->VideoData = NULL;
    UNLOCK_HW

    Remove((struct Node *)&data->node);

    OOP_DoSuperMethod(cl, o, msg);
}

VOID SAGAGFXBitmap__Root__Set(OOP_Class *cl, OOP_Object *o, struct pRoot_Set *msg)
{

}

VOID SAGAGFXBitmap__Root__Get(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{

}

/****************************************************************************************/

static int SAGAGFXBitmap_Init(LIBBASETYPEPTR LIBBASE)
{
    D(bug("SAGAGFXBitmap_Init\n"));
    return TRUE;
}

/****************************************************************************************/

static int SAGAGFXBitmap_Expunge(LIBBASETYPEPTR LIBBASE)
{
    D(bug("SAGAGFXBitmap_Expunge\n"));
    return TRUE;
}

/****************************************************************************************/

ADD2INITLIB(SAGAGFXBitmap_Init, 0);
ADD2EXPUNGELIB(SAGAGFXBitmap_Expunge, 0);

BOOL SAGAGFXBitmap__Hidd_BitMap__ObtainDirectAccess(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_ObtainDirectAccess *msg)
{
    return TRUE;
}

VOID SAGAGFXBitmap__Hidd_BitMap__ReleaseDirectAccess(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_ReleaseDirectAccess *msg)
{

}

BOOL SAGAGFXBitmap__Hidd_BitMap__SetColors(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_SetColors *msg)
{
	return FALSE;
}

VOID SAGAGFXBitmap__Hidd_BitMap__PutPixel(OOP_Class *cl, OOP_Object *o,
				struct pHidd_BitMap_PutPixel *msg)
{

}

/****************************************************************************************/

ULONG SAGAGFXBitmap__Hidd_BitMap__GetPixel(OOP_Class *cl, OOP_Object *o,
				 struct pHidd_BitMap_GetPixel *msg)
{
	return 0;
}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__DrawLine(OOP_Class *cl, OOP_Object *o,
				struct pHidd_BitMap_DrawLine *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__GetImage(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_GetImage *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__PutImage(OOP_Class *cl, OOP_Object *o,
				struct pHidd_BitMap_PutImage *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__PutImageLUT(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_BitMap_PutImageLUT *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__GetImageLUT(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_BitMap_GetImageLUT *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__FillRect(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_DrawRect *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__PutPattern(OOP_Class *cl, OOP_Object *o,
				 struct pHidd_BitMap_PutPattern *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__PutTemplate(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_PutTemplate *msg)
{

}

/****************************************************************************************/

VOID SAGAGFXBitmap__Hidd_BitMap__UpdateRect(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_UpdateRect *msg)
{

}

/****************************************************************************************/

BOOL SAGAGFXBitmap__Hidd_PlanarBM__SetBitMap(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_PlanarBM_SetBitMap *msg)
{
	return FALSE;
}

/****************************************************************************************/

BOOL SAGAGFXBitmap__Hidd_PlanarBM__GetBitMap(OOP_Class *cl, OOP_Object *o,
				   struct pHidd_PlanarBM_GetBitMap *msg)
{
	return FALSE;
}
