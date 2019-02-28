/*
    Copyright © 2018, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/dos.h>
#include <aros/macros.h>

#include "grub2-install.h"

#if defined(IGRUB2_FATSUPPORT)

static ULONG FATPartitionTypes[] =
{
    ID_FAT12_DISK,
    ID_FAT16_DISK,
    ID_FAT32_DISK,
    -1
};

CONST_STRPTR  FATName = "FAT Filesystem";

static ULONG FATFSCollectBlockList(struct Volume *volume, ULONG objectnode, struct BlockNode *blocklist)
{
    D(bug("[InstallGrub2:FAT] %s(%p, %ld, %p)\n", __func__, volume, block, blocklist);)
    return 0;
}

BOOL IGrub2FATSupport_init(void)
{
    struct FSSupport *fsHandler;

    D(bug("[InstallGrub2:FAT] %s: Initialising..\n", __func__));

    fsHandler = AllocMem(sizeof(struct FSSupport), MEMF_CLEAR);
    fsHandler->fss_Node.ln_Name = (char *)FATName;
    fsHandler->fss_FilesystemIDs = FATPartitionTypes;
    fsHandler->fss_CollectBlocks = FATFSCollectBlockList;

    IGrub2_RegisterFSHandler(fsHandler);

    return TRUE;
}

ADD2INIT(IGrub2FATSupport_init, 10);

#endif /* IGRUB2_FATSUPPORT */
