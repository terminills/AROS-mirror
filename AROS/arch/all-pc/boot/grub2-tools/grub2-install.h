#ifndef GRUB2_INSTALL_H
#define GRUB2_INSTALL_H

/*
    Copyright © 2018, The AROS Development Team. All rights reserved.
    $Id$
*/

#define BLCKLIST_ELEMENTS                       14

/* enable supported filesystems .. */
#define IGRUB2_FFSSUPPORT
#define IGRUB2_SFSBESUPPORT
//#define IGRUB2_SFSLESUPPORT
#define IGRUB2_FATSUPPORT

struct BlockNode
{
    ULONG sector_lo;
    ULONG sector_hi;
    UWORD count;
    UWORD seg_adr;
};

struct Volume;

typedef ULONG (*FSS_CollectBlocks)(struct Volume *volume, ULONG block, struct BlockNode *blocklist);

struct FSSupport
{
    struct Node         fss_Node;
    ULONG               *fss_FilesystemIDs;
    FSS_CollectBlocks   fss_CollectBlocks;
};

struct Volume
{
    struct MsgPort      *mp;
    struct IOExtTD      *iotd;
    struct FSSupport    *fsHandler;
    ULONG               readcmd;
    ULONG               writecmd;
    ULONG               startblock;
    ULONG               countblock;

    CONST_STRPTR        device;
    ULONG               unitnum;

    UWORD               SizeBlock;
    UBYTE               flags;
    BYTE                partnum;
    ULONG               *blockbuffer;
    ULONG               dos_id;
};

#define VF_IS_TRACKDISK (1<<0)
#define VF_IS_RDB       (1<<1)

extern void IGrub2_RegisterFSHandler(struct FSSupport *fsHandler);
extern struct FSSupport *IGrub2_FindFSHandler(ULONG FileSysID);

extern ULONG collectBlockListFFS(struct Volume *volume, ULONG block, struct BlockNode *blocklist);
extern ULONG collectBlockListSFSBE(struct Volume *volume, ULONG block, struct BlockNode *blocklist);
extern ULONG collectBlockListFAT(struct Volume *volume, ULONG block, struct BlockNode *blocklist);

extern ULONG _readwriteBlock(struct Volume *volume, ULONG block, APTR buffer, ULONG length, ULONG command);

#endif /* GRUB2_INSTALL_H */
