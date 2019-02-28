/*
    Copyright © 2018, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/dos.h>
#include <aros/macros.h>

#include "grub2-install.h"

#if defined(IGRUB2_FFSSUPPORT)

static ULONG FFSPartitionTypes[] =
{
    ID_FFS_DISK,
    ID_INTER_DOS_DISK,
    ID_INTER_FFS_DISK,
    ID_FASTDIR_DOS_DISK,
    ID_FASTDIR_FFS_DISK,
    -1
};

CONST_STRPTR  FFSName = "FFS Filesystem";

/* Collects the list of blocks that a file occupies on FFS filesystem */
static ULONG FFSCollectBlockList(struct Volume *volume, ULONG block, struct BlockNode *blocklist)
{
    ULONG retval, first_block;
    WORD blk_count,count;
    UWORD i;

    D(bug("[InstallGrub2:FFS] %s(%p, %ld, %p)\n", __func__, volume, block, blocklist);)

    /* Clear the core.img sector pointers region! */
    memset((UBYTE*)&blocklist[-BLCKLIST_ELEMENTS],0x00, BLCKLIST_ELEMENTS*sizeof(struct BlockNode)); 

    /*
            The number of first block of core.img will be stored in boot.img
            so skip the first filekey in the first loop
    */

    retval = _readwriteBlock(volume, block, volume->blockbuffer, volume->SizeBlock<<2,
			volume->readcmd);

    if (retval)
    {
        D(bug("[InstallGrub2:FFS] %s: ERROR reading block (error: %ld\n", __func__, retval);)
        Printf("ReadError %lu\n", (long)retval);
        return 0;
    }

    i = volume->SizeBlock - 52;
    first_block = AROS_BE2LONG(volume->blockbuffer[volume->SizeBlock-51]);
    blk_count=0;

    D(bug("[InstallGrub2:FFS] %s: First block @ %ld, i:%d\n", __func__, first_block, i);)

    do
    {
        retval = _readwriteBlock(volume, block, volume->blockbuffer, volume->SizeBlock<<2,
                        volume->readcmd);
        if (retval)
        {
            D(bug("[InstallGrub2:FFS] %s: ERROR reading block (error: %ld)\n", __func__, retval);)
            Printf("ReadError %lu\n", (long)retval);
            return 0;
        }

        D(bug("[InstallGrub2:FFS] %s: read block %ld, i = %d\n", __func__, block, i);)
        while ((i>=6) && (volume->blockbuffer[i]))
        {
            D(bug("[InstallGrub2:FFS] %s: i = %d\n", __func__, i);)
            /*
                    if current sector follows right after last sector
                    then we don't need a new element
            */
            if ((blocklist[blk_count].sector_lo) &&
                            ((blocklist[blk_count].sector_lo+blocklist[blk_count].count)==
                                    AROS_BE2LONG(volume->blockbuffer[i])))
            {
                blocklist[blk_count].count += 1;
                D(bug("[InstallGrub2:FFS] %s: sector %d follows previous - increasing count of block %d to %d\n", __func__,
                    i, blk_count, blocklist[blk_count].count);)
            }
            else
            {
                blk_count--; /* decrement first */
                D(bug("[InstallGrub2:FFS] %s: store new block (%d)\n", __func__, blk_count);)

                if ((blk_count-1) <= -BLCKLIST_ELEMENTS)
                {
                    D(bug("[InstallGrub2:FFS] %s: ERROR: out of block space at sector %d, block %d\n", __func__,
                        i, blk_count);)
                    Printf("There is no more space to save blocklist in core.img\n");
                    return 0;
                }
                D(bug("[InstallGrub2:FFS] %s: storing sector pointer for %d in block %d\n", __func__, 
                    i, blk_count);)
                blocklist[blk_count].sector_lo = AROS_BE2LONG(volume->blockbuffer[i]);
                blocklist[blk_count].sector_hi = 0;
                blocklist[blk_count].count = 1;
            }
            i--;
        }
        i = volume->SizeBlock - 51;
        block = AROS_BE2LONG(volume->blockbuffer[volume->SizeBlock - 2]);
        D(bug("[InstallGrub2:FFS] %s: next block %d, i = %d\n", __func__, block, i);)
    } while (block);

    /*
            blocks in blocklist are relative to the first
            sector of the HD (not partition)
    */

    D(bug("[InstallGrub2:FFS] %s: successfully updated pointers for %d blocks\n", __func__, blk_count);)

    i = 0;
    for (count=-1;count>=blk_count;count--)
    {
        blocklist[count].sector_lo += volume->startblock;
        blocklist[count].seg_adr = 0x820 + (i*32);
        i += blocklist[count].count;
        D(bug("[InstallGrub2:FFS] %s: correcting block %d for partition start\n", __func__, count);)
        D(bug("[InstallGrub2:FFS] %s: sector : %ld seg_adr : %p\n", __func__, 
            blocklist[count].sector_lo, blocklist[count].seg_adr);)
    }

    first_block += volume->startblock;
    D(bug("[InstallGrub2:FFS] %s: corrected first block for partition start: %ld\n", __func__, first_block);)

    return first_block;
}

BOOL IGrub2FFSSupport_init(void)
{
    struct FSSupport *fsHandler;

    D(bug("[InstallGrub2:FFS] %s: Initialising..\n", __func__));

    fsHandler = AllocMem(sizeof(struct FSSupport), MEMF_CLEAR);
    fsHandler->fss_Node.ln_Name = (char *)FFSName;
    fsHandler->fss_FilesystemIDs = FFSPartitionTypes;
    fsHandler->fss_CollectBlocks = FFSCollectBlockList;

    IGrub2_RegisterFSHandler(fsHandler);

    return TRUE;
}

ADD2INIT(IGrub2FFSSupport_init, 10);

#endif /* IGRUB2_FFSSUPPORT */
