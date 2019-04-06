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

#include "vampiregfx.h"
#include "vampiregfxbitmap.h"

static APTR allocrtgvrambitmap(struct vampiregfx_staticdata *csd, struct bm_data *bm)
{

}

static void freertgbitmap(struct vampiregfx_staticdata *csd, struct bm_data *bm)
{

}

static BOOL movebitmaptofram(struct vampiregfx_staticdata *csd, struct bm_data *bm)
{

}

static BOOL allocrtgbitmap(struct vampiregfx_staticdata *csd, struct bm_data *bm, BOOL usevram)
{

}

static BOOL movethisbitmaptovram(struct vampiregfx_staticdata *csd, struct bm_data *bm)
{

}

static BOOL movebitmaptovram(struct vampiregfx_staticdata *csd, struct bm_data *bm)
{

}

static void hidescreen(struct vampiregfx_staticdata *csd, struct bm_data *bm)
{

}

/****************************************************************************************/

OOP_Object *VampireGFXBitmap__Root__New(OOP_Class *cl, OOP_Object *o, struct pRoot_New *msg)
{

}

VOID VampireGFXBitmap__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{

}

VOID VampireGFXBitmap__Root__Set(OOP_Class *cl, OOP_Object *o, struct pRoot_Set *msg)
{

}

VOID VampireGFXBitmap__Root__Get(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{

}

/****************************************************************************************/

static int VampireGFXBitmap_Init(LIBBASETYPEPTR LIBBASE)
{

}

/****************************************************************************************/

static int VampireGFXBitmap_Expunge(LIBBASETYPEPTR LIBBASE)
{

}

/****************************************************************************************/

ADD2INITLIB(VampireGFXBitmap_Init, 0);
ADD2EXPUNGELIB(VampireGFXBitmap_Expunge, 0);

BOOL VampireGFXBitmap__Hidd_BitMap__ObtainDirectAccess(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_ObtainDirectAccess *msg)
{

}

