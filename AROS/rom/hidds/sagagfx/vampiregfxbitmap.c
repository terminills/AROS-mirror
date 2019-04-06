/*
    Copyright © 2016, Michael Ness. All rights reserved.
 */

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
#include <hidd/graphics.h>
#include <aros/symbolsets.h>

#include "sagagfx.h"
#include "sagagfxbitmap.h"

static APTR allocrtgvrambitmap(struct sagagfx_staticdata *csd, struct bm_data *bm)
{

}

static void freertgbitmap(struct sagagfx_staticdata *csd, struct bm_data *bm)
{

}

static BOOL movebitmaptofram(struct sagagfx_staticdata *csd, struct bm_data *bm)
{

}

static BOOL allocrtgbitmap(struct sagagfx_staticdata *csd, struct bm_data *bm, BOOL usevram)
{

}

static BOOL movethisbitmaptovram(struct sagagfx_staticdata *csd, struct bm_data *bm)
{

}

static BOOL movebitmaptovram(struct sagagfx_staticdata *csd, struct bm_data *bm)
{

}

static void hidescreen(struct sagagfx_staticdata *csd, struct bm_data *bm)
{

}

/****************************************************************************************/

OOP_Object *SAGAGFXBitmap__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{

}

VOID SAGAGFXBitmap__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{

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

}

/****************************************************************************************/

static int SAGAGFXBitmap_Expunge(LIBBASETYPEPTR LIBBASE)
{

}

/****************************************************************************************/

ADD2INITLIB(SAGAGFXBitmap_Init, 0);
ADD2EXPUNGELIB(SAGAGFXBitmap_Expunge, 0);

BOOL SAGAGFXBitmap__Hidd_BitMap__ObtainDirectAccess(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_ObtainDirectAccess *msg)
{

}

