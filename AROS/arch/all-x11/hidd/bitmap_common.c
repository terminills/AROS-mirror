/*
    Copyright � 1995-2001, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/alerts.h>

#undef DEBUG
#define DEBUG 0
#include <aros/debug.h>

/* stegerg: maybe more safe, even if Unix malloc is used and not AROS malloc */
#define NO_MALLOC 1


static BOOL MNAME(setcolors)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_SetColors *msg)
{
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    HIDDT_PixelFormat *pf;
    
    ULONG xc_i, col_i;
    
    
    pf = BM_PIXFMT(o);
    
    if (    vHidd_ColorModel_StaticPalette == HIDD_PF_COLMODEL(pf)
    	 || vHidd_ColorModel_TrueColor == HIDD_PF_COLMODEL(pf) ) {
	 
	 /* Superclass takes care of this case */
	 
	 return OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    }

    /* Ve have a vHidd_GT_Palette bitmap */    
	
    if (!OOP_DoSuperMethod(cl, o, (OOP_Msg)msg)) return FALSE;
    
    if (data->flags & BMDF_COLORMAP_ALLOCED)
    {
LX11	

	for ( xc_i = msg->firstColor, col_i = 0;
    		    col_i < msg->numColors; 
		    xc_i ++, col_i ++ )
	{
            XColor xcol;

	    xcol.red   = msg->colors[col_i].red;
	    xcol.green = msg->colors[col_i].green;
	    xcol.blue  = msg->colors[col_i].blue;
	    xcol.pad   = 0;
	    xcol.pixel = xc_i;

	    xcol.flags = DoRed | DoGreen | DoBlue;
	    XStoreColor(data->display, data->colmap, &xcol);

	}
UX11	
    } /* if (data->flags & BMDF_COLORMAP_ALLOCED) */
    

    return TRUE;
}
/*********  BitMap::PutPixel()  ***************************/

static VOID MNAME(putpixel)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_PutPixel *msg)
{
     struct bitmap_data *data = OOP_INST_DATA(cl, o);
     
LX11
     
     XSetForeground(data->display, data->gc, msg->pixel);
     XDrawPoint(data->display, DRAWABLE(data), data->gc, msg->x, msg->y);
     
     XFlush(data->display);
UX11     
     return;
}

/*********  BitMap::GetPixel()  *********************************/
static HIDDT_Pixel MNAME(getpixel)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_GetPixel *msg)
{
    HIDDT_Pixel pixel;
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    
    XImage *image;

LX11
    XSync(data->display, False);
    
    image = XGetImage(data->display
    	, DRAWABLE(data)
	, msg->x, msg->y
	, 1, 1
	, AllPlanes
	, ZPixmap);
UX11    
    if (!image)
    	return -1L;
	
LX11    
    pixel = XGetPixel(image, 0, 0);
    
    XDestroyImage(image);
UX11    
    /* Get pen number from colortab */
    
    return pixel;
    
    
}

/*********  BitMap::DrawPixel() ************************************/
static ULONG MNAME(drawpixel)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_DrawPixel *msg)
{

    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    XGCValues gcval;
    
    gcval.function = GC_DRMD(msg->gc);
    gcval.foreground = GC_FG(msg->gc);
    gcval.background = GC_BG(msg->gc);

LX11
    XChangeGC(data->display
    	, data->gc
	, GCFunction | GCForeground | GCBackground
	, &gcval
    );
    
    XDrawPoint(data->display, DRAWABLE(data), data->gc, msg->x, msg->y);
    XFlush(data->display); /* stegerg: uncommented */
UX11    
    return 0;

    
}


/*********  BitMap::FillRect()  *************************************/
static VOID MNAME(fillrect)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_DrawRect *msg)
{
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    XGCValues gcval;
    
    
    EnterFunc(bug("X11Gfx.BitMap::FillRect(%d,%d,%d,%d)\n",
    	msg->minX, msg->minY, msg->maxX, msg->maxY));
	
    
    D(bug("Drawmode: %d\n", mode));
    
    gcval.function = GC_DRMD(msg->gc);
    gcval.foreground = GC_FG(msg->gc);
    gcval.background = GC_BG(msg->gc);

LX11
    XChangeGC(data->display
    	, data->gc
	, GCFunction | GCForeground | GCBackground
	, &gcval
    );
    
    XFillRectangle(data->display
	, DRAWABLE(data)
	, data->gc
	, msg->minX
	, msg->minY
	, msg->maxX - msg->minX + 1
	, msg->maxY - msg->minY + 1
    );

    XFlush(data->display);
UX11    
    ReturnVoid("X11Gfx.BitMap::FillRect");
    

}

/*********  BitMap::GetImage()  *************************************/

static ULONG *ximage_to_buf(OOP_Class *cl, OOP_Object *bm
	, HIDDT_Pixel *buf, XImage *image
	, ULONG width, ULONG height, ULONG depth
	, APTR dummy)
{

    if (image->bits_per_pixel == 16)
    {

	/* sg */
	
    	LONG x, y;
	UWORD *imdata = (UWORD *)image->data;
	
	for (y = 0; y < height; y ++)
	{
	    for (x = 0; x < width; x ++)
	    {
		*buf ++ = (UWORD)*imdata ++;
		
	    }
	    imdata += ((image->bytes_per_line / 2) - width); /*sg*/
	}

	
    }
    else
    {
    	LONG x, y;

LX11	
	for (y = 0; y < height; y ++)
	{
	    for (x = 0; x < width; x ++)
	    {
		*buf ++ = XGetPixel(image, x, y);
	    }
	    
	}
UX11
    
    }
    
    return buf;
}


#define ABS(a) ((a) < 0 ? -(a) : a)
	

static inline UBYTE pix_to_lut(HIDDT_Pixel pixel, HIDDT_PixelLUT *plut, HIDDT_PixelFormat *pf)
{
    ULONG i, best_match;
    ULONG diff, lowest_diff = 0xFFFFFFFF;
    HIDDT_ColComp red, green, blue;
    
    red   = RED_COMP(pixel, pf);
    green = GREEN_COMP(pixel, pf);
    blue  = BLUE_COMP(pixel, pf);
    
    for (i = 0; i < plut->entries; i ++) {
    	register HIDDT_Pixel cur_lut = plut->pixels[i];
	
    	if (pixel == cur_lut)
		return i; /* Exact match found */
	
	/* How well does these pixels match ? */
	diff =  ABS(red   - RED_COMP(cur_lut, pf))   +
		ABS(green - GREEN_COMP(cur_lut, pf)) +
		ABS(blue  - BLUE_COMP(cur_lut, pf));
		
	if (diff < lowest_diff) {
		best_match = i;
		lowest_diff = diff;
	}
	
    }
    return best_match;
}


static UBYTE *ximage_to_buf_lut(OOP_Class *cl, OOP_Object *bm
	, UBYTE *buf, XImage *image
	, ULONG width, ULONG height, ULONG depth
	, struct pHidd_BitMap_GetImageLUT *msg)
	
{
    /* This one is trickier, as we have to reverse-lookup the lut.
       This costs CPU ! Maybe one could do some kind of caching here ?
       Ie. one stores the most often used RGB combinations
       in a trie and looks up this first to see if whe can find an exact match
    */
    
    HIDDT_PixelFormat *pf = BM_PIXFMT(bm);
    
    UBYTE *pixarray = msg->pixels;
    
    if (image->bits_per_pixel == 16)
    {
	UWORD *imdata = (UWORD *)image->data;
	LONG x, y;
	
	for (y = 0; y < height; y ++)
	{
	    UBYTE *buf = pixarray;
	    
	    for (x = 0; x < width; x ++)
	    {
		*buf ++ = pix_to_lut((HIDDT_Pixel)*imdata, msg->pixlut, pf);
	    
		imdata ++;
	    }
	    imdata += ((image->bytes_per_line / 2) - width); /*sg*/
	    
	    pixarray += msg->modulo;
	    
	}
    }
    else
    {
    	LONG x, y;

LX11	
	for (y = 0; y < height; y ++)
	{
	    UBYTE *buf = pixarray;
	    for (x = 0; x < width; x ++)
	    {
		*buf ++ = pix_to_lut((HIDDT_Pixel)XGetPixel(image, x, y), msg->pixlut, pf);;
	    }
	    pixarray += msg->modulo;
	    
	}
UX11
    
    }
    
    return pixarray;
     
}    

#if USE_XSHM

static void getimage_xshm(OOP_Class *cl, OOP_Object *o
	, LONG x, LONG y
	, ULONG width, ULONG height
	, APTR pixarray
	, APTR (*fromimage_func)()
	, APTR fromimage_data)
{
     

    IPTR depth;
    struct bitmap_data *data;
    XImage *image;
    ULONG  bperline;
    
    ULONG lines_to_copy;
    LONG ysize;
    LONG current_y;
    LONG maxlines;
    OOP_Object *pf;
	
    data = OOP_INST_DATA(cl, o);
    
    OOP_GetAttr(o,  aHidd_BitMap_PixFmt, (IPTR *)&pf);
    OOP_GetAttr(pf, aHidd_PixFmt_Depth,  &depth);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

LX11
    image = create_xshm_ximage(data->display
    	, DefaultVisual(data->display, data->screen)
	, depth
	, ZPixmap
	, width
	, height
	, XSD(cl)->xshm_info
    );
UX11    
    if (!image)
    	ReturnVoid("X11Gfx.BitMap::PutImage(XShmCreateImage failed)");
    bperline = image->bytes_per_line;
    
    /* Calculate how many scanline can be stored in the buffer */
    maxlines = XSHM_MEMSIZE / image->bytes_per_line; 
    
    if (0 == maxlines)
    {
    	kprintf("ALERT !!! NOT ENOUGH MEMORY TO READ A COMPLETE SCANLINE\n");
    	kprintf("THROUGH XSHM IN X11GF X HIDD !!!\n");
	Alert(AT_DeadEnd);
    }
    
    current_y = 0;
    ysize = image->height;
    
    
    ObtainSemaphore(&XSD(cl)->shm_sema);

    while (ysize)
    {
	/* Get some more pixels from the Ximage */
	
        lines_to_copy = MIN(maxlines, ysize);
	
	ysize -= lines_to_copy;
	image->height = lines_to_copy;
LX11	
	get_xshm_ximage(data->display
		, DRAWABLE(data)
		, image
		, x
		, y + current_y
	);
	current_y += lines_to_copy;
UX11
	pixarray = fromimage_func(cl, o, pixarray, image, image->width, lines_to_copy, depth, fromimage_data);
	
    } /* while (pixels left to copy) */
    
    ReleaseSemaphore(&XSD(cl)->shm_sema);

LX11
	destroy_xshm_ximage(image);    
UX11    

    return;

}

#endif

static void getimage_xlib(OOP_Class *cl, OOP_Object *o
	, LONG x, LONG y
	, ULONG width, ULONG height
	, APTR pixels
	, APTR (*fromimage_func)()
	, APTR fromimage_data)
{
    ULONG *pixarray = (ULONG *)pixels;
    struct bitmap_data *data;
    XImage *image;
    IPTR depth;
    OOP_Object *pf; 
 
    data = OOP_INST_DATA(cl, o);

    OOP_GetAttr(o,  aHidd_BitMap_PixFmt, (IPTR *)&pf);
    OOP_GetAttr(pf, aHidd_PixFmt_Depth,  &depth);

LX11
    image = XGetImage(data->display
    	, DRAWABLE(data)
	, x, y
	, width, height
	, AllPlanes
	, ZPixmap);
UX11	
    if (!image)
    	return;
	
	
    fromimage_func(cl, o, pixarray, image
    	, width, height
	, depth, fromimage_data);
	
LX11    
    XDestroyImage(image);
UX11    
    
    return;
}    

static VOID MNAME(getimage)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_GetImage *msg)
{
#if USE_XSHM
    if (XSD(cl)->use_xshm)
    {
	getimage_xshm(cl, o
    	    , msg->x, msg->y
	    , msg->width, msg->height
	    , msg->pixels
	    , (APTR (*)())ximage_to_buf
	    , NULL
	);
    }
    else
#endif
    {
	 getimage_xlib(cl, o
    	     , msg->x, msg->y
	     , msg->width, msg->height
	     , msg->pixels
	     , (APTR (*)())ximage_to_buf
	     , NULL
	 );
    }
	
    return;
}


static VOID MNAME(getimagelut)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_GetImageLUT *msg)
{
#if USE_XSHM
    if (XSD(cl)->use_xshm)
    {
	getimage_xshm(cl, o
    	    , msg->x, msg->y
	    , msg->width, msg->height
	    , msg->pixels
	    , (APTR (*)())ximage_to_buf_lut
	    , msg
	);
    }
    else
#endif
    {
	getimage_xlib(cl, o
    	    , msg->x, msg->y
	    , msg->width, msg->height
	    , msg->pixels
	    , (APTR (*)())ximage_to_buf_lut
	    , msg
	);
    }
    	
    return;
}


#undef DEBUG
#define DEBUG 0
#include <aros/debug.h>

/*********  BitMap::PutImage()  *************************************/


static ULONG *buf_to_ximage(OOP_Class *cl, OOP_Object *bm
	, HIDDT_Pixel *buf, XImage *image
	, ULONG width, ULONG height, ULONG depth
	, struct pHidd_BitMap_PutImage *msg
)
{

    /* Test if modulo == width */
    BOOL use_modulo = TRUE;
/*    kprintf("buf_to_ximage(cl=%p, o=%p, buf=%p, image=%p, width=%d, height=%d, depth=%d, msg=%p)\n"
    	, cl, bm, buf, image, width, height, depth, msg);
    
*/
    if (msg->modulo == HIDD_BM_BytesPerLine(bm, msg->pixFmt, width)) {
    	use_modulo = FALSE;
	
    }
    
    
    switch (msg->pixFmt) {
    case vHidd_StdPixFmt_Native:
    	if (!use_modulo) {
	    memcpy(image->data, buf, msg->modulo * height);
	} else {
	    LONG y;
            UBYTE *imdata = image->data;

	
	    for (y = 0; y < height; y ++) {
		memcpy(imdata, buf, msg->modulo);

#if 0			
		((UBYTE *)imdata) += msg->modulo;
#else
		((UBYTE *)imdata) += image->bytes_per_line; /*sg*/
#endif
		((UBYTE *)buf) += msg->modulo;
	    }
	}
	break;
	
    case vHidd_StdPixFmt_Native32:
/* kprintf("Native32 format, bits_per_pixel=%d\n", image->bits_per_pixel);
*/    	switch (image->bits_per_pixel) {
	
	case 16: {
	    LONG x, y;
	    
	    UWORD *imdata = (UWORD *)image->data;
/*	    kprintf("16 bit Native32, modulo=%d, imdat=%x\n"
	    	, msg->modulo, imdata);
*/

	    for (y = 0; y < height; y ++) {
		HIDDT_Pixel *p = buf;
		for (x = 0; x < width; x ++) {
		    *imdata ++ = (UWORD)*p ++;
		}
		((UBYTE *)imdata) += (image->bytes_per_line - width * 2); /*sg*/
		((UBYTE *)buf) += msg->modulo;
	    }
	    break; }
		
		
	case 24: {
	    LONG x, y;
	    
	    UBYTE *imdata = image->data;
	    HIDDT_PixelFormat *pf;
	    
	    pf = BM_PIXFMT(bm);
	    
	    for (y = 0; y < height; y ++) {
	    
	    	HIDDT_Pixel *p = buf;
		
	    	for (x = 0; x < width; x ++) {
		     register HIDDT_Pixel pix;
		     
		     pix = *p ++;
#if (AROS_BIG_ENDIAN == 1)
	    	    *imdata ++ = pix >> 16;
		    *imdata ++ = (pix & pf->green_mask) >> 8;
		    *imdata ++ = (pix & pf->blue_mask);
#else
		    *imdata ++ = (pix & pf->blue_mask);
		    *imdata ++ = (pix & pf->green_mask) >> 8;
		    *imdata ++ = pix >> 16;
#endif
		}
		((UBYTE *)imdata) += (image->bytes_per_line - width * 3); /*sg*/		
		((UBYTE *)buf) += msg->modulo;
	    }
	    break; }
	    
	default: {
	    LONG x, y;

LX11	    
	    for (y = 0; y < height; y ++) {
	    	HIDDT_Pixel *p;
	    
	    	p = buf;
	    	for (x = 0; x < width; x ++) {
		     XPutPixel(image, x, y, *p ++);
		}
		((UBYTE *)buf) += msg->modulo;
	    }
UX11
	    break; }
	
	} /* switch (image->bits_per_pixel) */
	
	break;
	     
	
	
     default: {
     
	OOP_Object *srcpf, *dstpf, *gfxhidd;
	APTR srcPixels = buf, dstBuf = image->data;
		
	//kprintf("DEFAULT PIXEL CONVERSION\n");
	
	OOP_GetAttr(bm, aHidd_BitMap_GfxHidd, (IPTR *)&gfxhidd);
	srcpf = HIDD_Gfx_GetPixFmt(gfxhidd, msg->pixFmt);
	
	OOP_GetAttr(bm, aHidd_BitMap_PixFmt, (IPTR *)&dstpf);
	
	//kprintf("CALLING ConvertPixels()\n");
	
     	HIDD_BM_ConvertPixels(bm, &srcPixels
		, (HIDDT_PixelFormat *)srcpf
		, msg->modulo
		, &dstBuf
		, (HIDDT_PixelFormat *)dstpf
		, image->bytes_per_line
		, width, height
		, NULL /* We have no CLUT */
	);
	
	//kprintf("CONVERTPIXELS DONE\n");
	
	
	((UBYTE *)buf) += msg->modulo * height;
	break; }

    } /* switch (msg->pixFmt) */
    
    return buf;
}



static UBYTE *buf_to_ximage_lut(OOP_Class *cl, OOP_Object *bm
	, UBYTE *pixarray, XImage *image
	, ULONG width, ULONG height, ULONG depth
	, struct pHidd_BitMap_PutImageLUT *msg
)
{
    HIDDT_Pixel *lut = msg->pixlut->pixels;
    
    if (image->bits_per_pixel == 16)
    {
    	LONG x, y;
	UWORD *imdata = (UWORD *)image->data;
	
	for (y = 0; y < height; y ++)
	{
	    UBYTE *buf = pixarray;
	    for (x = 0; x < width; x ++)
	    {
		*imdata ++ = (UWORD)lut[*buf ++];
		
	    }
	    pixarray += msg->modulo;
	    imdata += ((image->bytes_per_line / 2) - width); /*sg*/
	}
    }
    else
    {
    	LONG x, y;
	
	for (y = 0; y < height; y ++)
	{
	    UBYTE *buf = pixarray;
	    for (x = 0; x < width; x ++)
	    {
		XPutPixel(image, x, y, lut[*buf ++]);
	    }
	    
	    pixarray += msg->modulo;
	    
	}
    
    }
    
    return pixarray;
}


#if USE_XSHM
static void putimage_xshm(OOP_Class *cl, OOP_Object *o, OOP_Object *gc
	, LONG x, LONG y
	, ULONG width, ULONG height
	, APTR pixarray
	, APTR (*toimage_func)()
	, APTR toimage_data)
{

    IPTR depth;
    struct bitmap_data *data;
    XImage *image;
    ULONG  bperline;
    
    ULONG lines_to_copy;
    LONG ysize;
    LONG current_y;
    LONG maxlines;
    OOP_Object *pf;

	
    data = OOP_INST_DATA(cl, o);
    OOP_GetAttr(o, aHidd_BitMap_PixFmt, (IPTR *)&pf);
    OOP_GetAttr(pf, aHidd_PixFmt_Depth, &depth);


#define MIN(a, b) (((a) < (b)) ? (a) : (b))


LX11
    image = create_xshm_ximage(data->display
    	, DefaultVisual(data->display, data->screen)
	, depth
	, ZPixmap
	, width
	, height
	, XSD(cl)->xshm_info
    );
UX11    
    if (!image)
    	ReturnVoid("X11Gfx.BitMap::PutImage(XShmCreateImage failed)");
    bperline = image->bytes_per_line;
    
    /* Calculate how many scanline can be stored in the buffer */
    maxlines = XSHM_MEMSIZE / image->bytes_per_line; 
    
    if (0 == maxlines)
    {
    	kprintf("ALERT !!! NOT ENOUGH MEMORY TO WRITE A COMPLETE SCANLINE\n");
    	kprintf("THROUGH XSHM IN X11GF X HIDD !!!\n");
	Alert(AT_DeadEnd);
    }
    
    current_y = 0;
    ysize = image->height;
    
    
    ObtainSemaphore(&XSD(cl)->shm_sema);

    while (ysize)
    {
	/* Get some more pixels from the HIDD */
	
        lines_to_copy = MIN(maxlines, ysize);
	
	ysize -= lines_to_copy;
	image->height = lines_to_copy;
	
	pixarray = toimage_func(cl, o, pixarray, image, image->width, lines_to_copy, depth, toimage_data);
	
LX11	
	XSetFunction(data->display, data->gc, GC_DRMD(gc));

	put_xshm_ximage(data->display
		, DRAWABLE(data)
		, data->gc
		, image
		, 0, 0
		, x
		, y + current_y
		, image->width, lines_to_copy
		, FALSE
	);

UX11
	current_y += lines_to_copy;
	
    } /* while (pixels left to copy) */
    
    
    ReleaseSemaphore(&XSD(cl)->shm_sema);

LX11
    XFlush(data->display); /* stegerg: added */
    destroy_xshm_ximage(image);    
UX11    

    return;

}

#endif

static void putimage_xlib(OOP_Class *cl, OOP_Object *o, OOP_Object *gc
	, LONG x, LONG y
	, ULONG width, ULONG height
	, APTR pixarray
	, APTR (*toimage_func)()
	, APTR toimage_data)
{

    IPTR depth;
    struct bitmap_data *data;
    XImage *image;
    ULONG  bperline;
    OOP_Object *pf;
	
    data = OOP_INST_DATA(cl, o);
    OOP_GetAttr(o,  aHidd_BitMap_PixFmt, (IPTR *)&pf);
    OOP_GetAttr(pf, aHidd_PixFmt_Depth,  &depth);



LX11	
    image = XCreateImage(data->display
	, DefaultVisual(data->display, data->screen)
	, depth
	, ZPixmap
	, 0
	, NULL
	, width, height
	, 32
	, 0
    );
UX11	
    if (!image)
    	ReturnVoid("X11Gfx.BitMap::PutImage(XCreateImage failed)");
	    
    bperline	= image->bytes_per_line;

    #if NO_MALLOC
    image->data = (char *)AllocVec((size_t)height * bperline, MEMF_PUBLIC);
    #else	
    image->data = (char *)malloc((size_t)height * bperline);
    #endif
    if (!image->data)
    {
LX11	
	XFree(image);
UX11	    
    	ReturnVoid("X11Gfx.BitMap::PutImage(malloc(image data) failed)");
    }
    
    toimage_func(cl, o, pixarray, image, width, height, depth, toimage_data);
	
LX11
    XSetFunction(data->display, data->gc, GC_DRMD(gc));
    XPutImage(data->display
    		 , DRAWABLE(data)
		 , data->gc
		 , image
		 , 0, 0
		 , x, y
		 , width, height
    );
    XFlush(data->display);
UX11 
    #if NO_MALLOC
    FreeVec(image->data);
    #else   
    free(image->data);
    #endif
    
LX11    
    XFree(image);
UX11   

    return;
}
	
static VOID MNAME(putimage)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_PutImage *msg)
{
    EnterFunc(bug("X11Gfx.BitMap::PutImage(pa=%p, x=%d, y=%d, w=%d, h=%d)\n",
    	msg->pixels, msg->x, msg->y, msg->width, msg->height));
	
#if USE_XSHM
    if (XSD(cl)->use_xshm)
    {
	putimage_xshm(cl, o, msg->gc
    	    , msg->x, msg->y
	    , msg->width, msg->height
	    , msg->pixels
	    , (APTR (*)()) buf_to_ximage
	    , msg
	);
    }
    else
#endif
    {
	putimage_xlib(cl, o, msg->gc
    	    , msg->x, msg->y
	    , msg->width, msg->height
	    , msg->pixels
	    , (APTR (*)()) buf_to_ximage
	    , msg
	);
    }

    ReturnVoid("X11Gfx.BitMap::PutImage");
}

/*********  BitMap::PutImageLUT()  *************************************/


static VOID MNAME(putimagelut)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_PutImageLUT *msg)
{
    EnterFunc(bug("X11Gfx.BitMap::PutImage(pa=%p, x=%d, y=%d, w=%d, h=%d)\n",
    	msg->pixels, msg->x, msg->y, msg->width, msg->height));
	
#if USE_XSHM
    if (XSD(cl)->use_xshm)
    {
	putimage_xshm(cl, o, msg->gc
    	    , msg->x, msg->y
	    , msg->width, msg->height
	    , msg->pixels
	    , (APTR (*)())buf_to_ximage_lut
	    , msg
	);
    }
    else
#endif
    {
	putimage_xlib(cl, o, msg->gc
    	    , msg->x, msg->y
	    , msg->width, msg->height
	    , msg->pixels
	    , (APTR (*)())buf_to_ximage_lut
	    , msg
	);
    }

    ReturnVoid("X11Gfx.BitMap::PutImageLUT");
}


#undef DEBUG
#define DEBUG 0
#include <aros/debug.h>

/*** BitMap::BlitColorExpansion() **********************************************/
static VOID MNAME(blitcolorexpansion)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_BlitColorExpansion *msg)
{
    ULONG cemd;
    XImage *dest_im;
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    HIDDT_Pixel fg, bg;
    LONG x, y;
    
    Drawable d = 0;
    
    EnterFunc(bug("X11Gfx.BitMap::BlitColorExpansion(%p, %d, %d, %d, %d, %d, %d)\n"
    	, msg->srcBitMap, msg->srcX, msg->srcY, msg->destX, msg->destY, msg->width, msg->height));
    
    
    OOP_GetAttr(msg->srcBitMap, aHidd_X11BitMap_Drawable, (IPTR *)&d);
    
    if (0 == d)
    {
    	/* We know nothing about the source bitmap. Let the superclass handle this */
	OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
	return;
    }
    
    fg = GC_FG(msg->gc);
    bg = GC_BG(msg->gc);
    cemd = GC_COLEXP(msg->gc);

    if (0 != d)
    {

LX11    
	XSetForeground(data->display, data->gc, fg);
    	if (cemd & vHidd_GC_ColExp_Opaque)  
	{

	    XSetBackground(data->display, data->gc, bg);
	    XSetFunction(data->display, data->gc, GXcopy);
	    
	    XCopyPlane(data->display
	    	, d, DRAWABLE(data)
		, data->gc
		, msg->srcX, msg->srcY
		, msg->width, msg->height
		, msg->destX, msg->destY
		, 0x01
	    );
	} else {
	    /* Do transparent blit */
	    
	    XGCValues val;
	    val.stipple		= d;
	    val.ts_x_origin	= msg->destX - msg->srcX;
	    val.ts_y_origin	= msg->destY - msg->srcY;
	    val.fill_style	= FillStippled;

	    XSetFunction(data->display, data->gc, GXcopy); /* stegerg: added, CHECKME:!! */
	    
	    XChangeGC(data->display
	    	, data->gc
		, GCStipple|GCTileStipXOrigin|GCTileStipYOrigin|GCFillStyle
		, &val
	    );
	    XFillRectangle(data->display
	    	, DRAWABLE(data)
		, data->gc
		, msg->destX, msg->destY
		, msg->width, msg->height
	    );
	    XSetFillStyle(data->display, data->gc, FillSolid);

	}
UX11	

    }
    else
    {
    	/* We know nothing about the format of the source bitmap
	   an must get single pixels
	*/

LX11    
	dest_im = XGetImage(data->display
		, DRAWABLE(data)
		, msg->destX, msg->destY
		, msg->width, msg->height
		, AllPlanes
		, ZPixmap);
    	
UX11    
	if (!dest_im)
    	    ReturnVoid("X11Gfx.BitMap::BlitColorExpansion()");


	D(bug("Src bm: %p\n", msg->srcBitMap));
	for (y = 0; y < msg->height; y ++)
	{
	    for (x = 0; x < msg->width; x ++)
	    {
		ULONG is_set;
	    
	    	is_set = HIDD_BM_GetPixel(msg->srcBitMap, x + msg->srcX, y + msg->srcY);
	    
	    	if (is_set)
	    	{
	    	    XPutPixel(dest_im, x, y, fg);
		
	    	}
	    	else
	    	{
		    if (cemd & vHidd_GC_ColExp_Opaque)
		    {
			XPutPixel(dest_im, x, y, bg);
		    }
		}
	    } /* for (each x) */
	    
    	} /* for (each y) */
    
	/* Put image back into display */
LX11    
	XSetFunction(data->display, data->gc, GC_DRMD(msg->gc));
	XPutImage(data->display
    		, DRAWABLE(data)
		, data->gc
		, dest_im
		, 0, 0
		, msg->destX, msg->destY
		, msg->width, msg->height
    	);

    	XDestroyImage(dest_im);
UX11
    }

LX11
    XFlush(data->display);
UX11 
    ReturnVoid("X11Gfx.BitMap::BlitColorExpansion");
}

#undef DEBUG
#define DEBUG 0
#include <aros/debug.h>

/*** BitMap::Get() *******************************************/

static VOID MNAME(get)(OOP_Class *cl, OOP_Object *o, struct pRoot_Get *msg)
{
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    ULONG idx;
    if (IS_X11BM_ATTR(msg->attrID, idx))
    {
	switch (idx)
	{
	    case aoHidd_X11BitMap_Drawable:
	    	*msg->storage = (IPTR)DRAWABLE(data);
		break;

	    case aoHidd_X11BitMap_MasterWindow:
	        *msg->storage = (IPTR)data->masterxwindow;
		break;
		
	    default:
	    	OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
		break;
	}
    }
    else
    {
    	OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    }
    
    
    return;
}



/*** BitMap:: DrawLine() ***************************/
VOID MNAME(drawline)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_DrawLine *msg)
{
    OOP_Object *gc = msg->gc;
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    
LX11
    if (GC_DOCLIP(gc)) {
    	XRectangle cr;
	
	cr.x = GC_CLIPX1(gc);
	cr.y = GC_CLIPY1(gc);
	cr.width  = GC_CLIPX2(gc) - cr.x + 1;
	cr.height = GC_CLIPY2(gc) - cr.y + 1;
    
    	XSetClipRectangles(data->display
		, data->gc
		, 0, 0
		, &cr
		, 1
		, Unsorted
	);
    }
    
    XSetForeground(data->display, data->gc, GC_FG(gc));
    XSetFunction(data->display, data->gc, GC_DRMD(gc));
	
    XDrawLine(data->display, DRAWABLE(data), data->gc
	, msg->x1, msg->y1
	, msg->x2, msg->y2 
    );
	
    if (GC_DOCLIP(gc)) {
    	XSetClipMask(data->display, data->gc, None);
    }	
    
    XFlush(data->display); /* stegerg: added */
UX11
}


/********** BitMap::DrawEllipse ******************************/

VOID MNAME(drawellipse)(OOP_Class *cl, OOP_Object *o, struct pHidd_BitMap_DrawEllipse *msg)
{
    OOP_Object *gc = msg->gc;
    struct bitmap_data *data = OOP_INST_DATA(cl, o);
    
LX11
    if (GC_DOCLIP(gc)) {
    	XRectangle cr;
	
/* kprintf("X11::Drawllipse: clip %d %d %d %d\n"
	, GC_CLIPX1(gc), GC_CLIPY1(gc), GC_CLIPX2(gc), GC_CLIPY2(gc));
*/	
	cr.x = GC_CLIPX1(gc);
	cr.y = GC_CLIPY1(gc);
	cr.width  = GC_CLIPX2(gc) - cr.x + 1;
	cr.height = GC_CLIPY2(gc) - cr.y + 1;
    
    	XSetClipRectangles(data->display
		, data->gc
		, 0, 0
		, &cr
		, 1
		, Unsorted
	);
    }
    
    XSetForeground(data->display, data->gc, GC_FG(gc));
    
/* kprintf("X11::Drawllipse: coord %d %d %d %d\n"
	, msg->x, msg->y, msg->rx, msg->ry);
   
*/	
    XDrawArc(data->display, DRAWABLE(data), data->gc
	, msg->x - msg->rx, msg->y - msg->ry 
	, msg->rx * 2, msg->ry * 2
	, 0, 360 * 64
    );
	
    if (GC_DOCLIP(gc)) {
    	XSetClipMask(data->display, data->gc, None);
    }	
    
    XFlush(data->display); /* stegerg: added */
UX11
}
