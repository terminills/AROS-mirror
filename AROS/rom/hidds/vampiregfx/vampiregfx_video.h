#ifndef VAMPIREGFX_VIDEO_H
#define VAMPIREGFX_VIDEO_H

/******************************************************************************
 * *
 * * BOARD definitions
 * *
 * *******************************************************************************/

SAGA_BOARD_GET() ((*(volatile UWORD*)0x00DFF3FC) >> 8)

#define SAGA_BOARD_Unknown  0x00
#define SAGA_BOARD_V600     0x01
#define SAGA_BOARD_V500     0x02
#define SAGA_BOARD_V4       0x03
#define SAGA_BOARD_V666     0x04
#define SAGA_BOARD_V4SA     0x05
#define SAGA_BOARD_V1200    0x06
#define SAGA_BOARD_V4000    0x07
#define SAGA_BOARD_VCD32    0x08
#define SAGA_BOARD_Future   0x09

    /******************************************************************************
     * *
     * * CLOCK definitions
     * *
     * *******************************************************************************/

#define SAGA_VIDEO_PLLW    0x00DFF1F8
#define SAGA_VIDEO_PLLR    0x00DFF1FA

#define SAGA_VIDEO_PLLW_MAGIC        0x43430000
#define SAGA_VIDEO_PLLW_CS(x)        (((x) & 1) << 0)
#define SAGA_VIDEO_PLLW_CLK(x)       (((x) & 1) << 1)
#define SAGA_VIDEO_PLLW_MOSI(x)      (((x) & 1) << 2)
#define SAGA_VIDEO_PLLW_UPDATE(x)    (((x) & 1) << 3)

#define SAGA_CLOCK_PAL      0
#define SAGA_CLOCK_NTSC     1
#define SAGA_CLOCK_COUNT    439
#define SAGA_CLOCK_COUNTMAX 539

    struct SAGA_CLOCK_DATA
{
        ULONG freq[2];
            UBYTE data[18];
};

typedef struct SAGA_CLOCK_DATA SAGA_CLOCK_DATA;

/******************************************************************************
 * *
 * * VIDEO definitions
 * *
 * *******************************************************************************/

#define SAGA_MOUSE_DELTAX         16
#define SAGA_MOUSE_DELTAY         8

#define SAGA_VIDEO_MAXHV          0x4000
#define SAGA_VIDEO_MAXVV          0x4000
#define SAGA_VIDEO_MAXHR          0x8000
#define SAGA_VIDEO_MAXVR          0x8000

#define SAGA_VIDEO_SPRITEX        0x00DFF1D0
#define SAGA_VIDEO_SPRITEY        0x00DFF1D2

#define SAGA_VIDEO_SPRITECLUT     0x00DFF3A2 // 3 x RGB4 colors
#define SAGA_VIDEO_SPRITEBPL      0x00DFF800

#define SAGA_VIDEO_BPLHMOD        0x00DFF1E6
#define SAGA_VIDEO_BPLPTR         0x00DFF1EC
#define SAGA_VIDEO_MODE           0x00DFF1F4
#define SAGA_VIDEO_HPIXEL         0x00DFF300
#define SAGA_VIDEO_HSSTRT         0x00DFF302
#define SAGA_VIDEO_HSSTOP         0x00DFF304
#define SAGA_VIDEO_HTOTAL         0x00DFF306
#define SAGA_VIDEO_VPIXEL         0x00DFF308
#define SAGA_VIDEO_VSSTRT         0x00DFF30A
#define SAGA_VIDEO_VSSTOP         0x00DFF30C
#define SAGA_VIDEO_VTOTAL         0x00DFF30E
#define SAGA_VIDEO_HVSYNC         0x00DFF310
#define SAGA_VIDEO_CLUT(x)       (0x00DFF400 + (((x) & 0xFF) << 2))

#define SAGA_VIDEO_FORMAT_OFF      0
#define SAGA_VIDEO_FORMAT_CLUT8    1
#define SAGA_VIDEO_FORMAT_RGB16    2
#define SAGA_VIDEO_FORMAT_RGB15    3
#define SAGA_VIDEO_FORMAT_RGB24    4
#define SAGA_VIDEO_FORMAT_RGB32    5
#define SAGA_VIDEO_FORMAT_YUV422   6

#define SAGA_VIDEO_DBLSCAN_OFF    0
#define SAGA_VIDEO_DBLSCAN_X      1
#define SAGA_VIDEO_DBLSCAN_Y      2
#define SAGA_VIDEO_DBLSCAN_XY     (SAGA_VIDEO_DBLSCAN_X|SAGA_VIDEO_DBLSCAN_Y)

#define SAGA_VIDEO_FORMAT_BE     0 /* Pixels in Big Endian */
#define SAGA_VIDEO_FORMAT_LE     1 /* Pixels in Little Endian */
#define SAGA_VIDEO_FORMAT_ENDIAN 7 /* Pixel Format Endian-ness (Bit 7) */

#define SAGA_VIDEO_MODE_FORMAT(x) (((x) & 0xff) << 0)
#define SAGA_VIDEO_MODE_DBLSCN(x) (((x) & 0xff) << 8)

#define SAGA_CLUT_ENTRY_VALID     (1UL << 31)

#define IS_DOUBLEX(w) ((w) <= 500)
#define IS_DOUBLEY(h) ((h) <= 300)

#endif
