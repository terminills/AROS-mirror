/*
    (C) 1997 AROS - The Amiga Replacement OS
    $Id$

    Desc: Basic help functions needed by gadtools.library.
    Lang: English.
*/
#include <string.h>
#include <proto/exec.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/graphics.h>
#include <graphics/rastport.h>
#include <graphics/rpattr.h>
#include <graphics/text.h>
#include <proto/utility.h>
#include <utility/tagitem.h>
#include <libraries/gadtools.h>
#include <aros/debug.h>
#include "gadtools_intern.h"



void freeitext(struct GadToolsBase_intern *GadToolsBase,
               struct IntuiText *itext)
{
    if (!itext)
        return;
    FreeVec(itext->ITextFont->ta_Name);
    FreeVec(itext->ITextFont);
    FreeVec(itext);
}

struct IntuiText *makeitext(struct GadToolsBase_intern *GadToolsBase,
			    struct NewGadget *ng)
{
    struct IntuiText *it;
    struct DrawInfo *dri = ((struct VisualInfo *)ng->ng_VisualInfo)->vi_dri;

    it = (struct IntuiText *)AllocVec(sizeof(struct IntuiText), MEMF_ANY);
    if (!it)
	return NULL;

    if (!(ng->ng_Flags & NG_HIGHLABEL))
	it->FrontPen = dri->dri_Pens[TEXTPEN];
    else
	it->FrontPen = dri->dri_Pens[HIGHLIGHTTEXTPEN];
    it->BackPen   = dri->dri_Pens[BACKGROUNDPEN];
    it->DrawMode  = JAM1;
    it->LeftEdge  = 0;
    it->TopEdge   = 0;
    it->ITextFont = AllocVec(sizeof(struct TextAttr), MEMF_ANY);
    if (!it->ITextFont)
    {
        FreeVec(it);
        return NULL;
    }
    if (ng->ng_TextAttr)
    {
        int len = strlen(ng->ng_TextAttr->ta_Name) + 1;

        it->ITextFont->ta_Name = AllocVec(len, MEMF_ANY);
        if (!it->ITextFont->ta_Name)
        {
            FreeVec(it->ITextFont);
            FreeVec(it);
            return NULL;
        }
        CopyMem(ng->ng_TextAttr->ta_Name, it->ITextFont->ta_Name, len);
        it->ITextFont->ta_YSize = ng->ng_TextAttr->ta_YSize;
        it->ITextFont->ta_Style = ng->ng_TextAttr->ta_Style;
        it->ITextFont->ta_Flags = ng->ng_TextAttr->ta_Flags;
    } else
    {
        int len = strlen(dri->dri_Font->tf_Message.mn_Node.ln_Name) + 1;

        it->ITextFont->ta_Name = AllocVec(len, MEMF_ANY);
        if (!it->ITextFont->ta_Name)
        {
            FreeVec(it->ITextFont);
            FreeVec(it);
            return NULL;
        }
        CopyMem(dri->dri_Font->tf_Message.mn_Node.ln_Name, it->ITextFont->ta_Name, len);
        it->ITextFont->ta_YSize = dri->dri_Font->tf_YSize;
        it->ITextFont->ta_Style = dri->dri_Font->tf_Style;
        it->ITextFont->ta_Flags = dri->dri_Font->tf_Flags;
    }
    it->IText     = ng->ng_GadgetText;
    it->NextText  = NULL;

    return it;
}


struct TextFont *preparefont(struct GadToolsBase_intern *GadToolsBase,
			     struct RastPort *rport, struct IntuiText *itext,
			     struct TextFont **oldfont)
{
    struct TextFont *font;

    if (itext->ITextFont)
    {
	*oldfont = rport->Font;
	font = OpenFont(itext->ITextFont);
	if (font)
	{
	    SetFont(rport, font);
	    /* SetSoftStyle(rport, itext->ITextFont->ta_Style, 0xffffffff) !!! */
	} else
	    font = rport->Font;
    } else
    {
	*oldfont = NULL;
	font = rport->Font;
    }
    SetABPenDrMd(rport, itext->FrontPen, itext->BackPen, itext->DrawMode);

    return font;
}

void closefont(struct GadToolsBase_intern *GadToolsBase,
	       struct RastPort *rport,
	       struct TextFont *font, struct TextFont *oldfont)
{
    if (oldfont)
    {
	SetFont(rport, oldfont);
	CloseFont(font);
    }
}

BOOL renderlabel(struct GadToolsBase_intern *GadToolsBase,
		 struct Gadget *gad, struct RastPort *rport, LONG labelplace)
{
    struct TextFont *font = NULL, *oldfont;
    struct TextExtent te;
    STRPTR text;
    int len = 0, x, y;
    UWORD width, height;

    if (gad->GadgetText)
    {
        /* Calculate offsets. */
        if ((gad->Flags & GFLG_LABELSTRING))
            text = (STRPTR)gad->GadgetText;
        else if ((gad->Flags & GFLG_LABELIMAGE))
            text = NULL;
        else
        {
            /* GFLG_LABELITEXT */
            text = gad->GadgetText->IText;
            font = preparefont(GadToolsBase,
                               rport, gad->GadgetText, &oldfont);
            if (!font)
                return FALSE;
        }

        if (text)
        {
            len = strlen(text);
            TextExtent(rport, text, len, &te);
            width  = te.te_Width;
            height = te.te_Height;
        } else
        {
            width  = ((struct Image *)gad->GadgetText)->Width;
            height = ((struct Image *)gad->GadgetText)->Height;
        }

        if (labelplace == GV_LabelPlace_Right)
        {
            x = gad->LeftEdge + gad->Width + 5;
            y = gad->TopEdge + (gad->Height - height) / 2 + 1;
        } else if (labelplace == GV_LabelPlace_Above)
        {
            x = gad->LeftEdge - (width - gad->Width) / 2;
            y = gad->TopEdge - height - 2;
        } else if (labelplace == GV_LabelPlace_Below)
        {
            x = gad->LeftEdge - (width - gad->Width) / 2;
            y = gad->TopEdge + gad->Height + 3;
        } else if (labelplace == GV_LabelPlace_In)
        {
            x = gad->LeftEdge - (width - gad->Width) / 2;
            y = gad->TopEdge + (gad->Height - height) / 2 + 1;
        } else /* GV_LabelPlace_Left */
        {
            x = gad->LeftEdge - width - 4;
            y = gad->TopEdge + (gad->Height - height) / 2 + 1;
        }

        y += rport->Font->tf_Baseline;
        if (gad->Flags & GFLG_LABELSTRING)
        {
            SetABPenDrMd(rport, 1, 0, JAM1);
            Move(rport, x, y);
            Text(rport, text, len);
        } else if (gad->Flags & GFLG_LABELIMAGE)
            DrawImage(rport, (struct Image *)gad->GadgetText, x, y);
        else
        {
            PrintIText(rport, gad->GadgetText, x, y);
            closefont(GadToolsBase, rport, font, oldfont);
        }
    }
    return TRUE;
}
