/*
    Copyright © 2018, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/dos.h>
#include <aros/macros.h>

#include "grub2-install.h"

#if defined(IGRUB2_SFSBESUPPORT)

static ULONG SFSBEPartitionTypes[] =
{
    ID_SFS_BE_DISK,
    -1
};

CONST_STRPTR  SFSBEName = "SFS-BE Filesystem";

/* Collects the list of blocks that a file occupies on SFS filesystem */
static ULONG SFSBECollectBlockList(struct Volume *volume, ULONG objectnode, struct BlockNode *blocklist)
{
    ULONG retval, first_block = 0;
    WORD blk_count = 0, count = 0;
    ULONG block_objectnoderoot = 0, block_sfsobjectcontainer = 0, block_extentbnoderoot = 0;
    ULONG nextblock = 0, searchedblock = 0;
    WORD i = 0;
    UBYTE * tmpBytePtr = NULL;

    D(
        bug("[InstallGrub2:SFS] %s(%p, %ld, %p)\n", __func__, volume, block, blocklist);
        bug("[InstallGrub2:SFS] %s: startblock: %ld\n", __func__, volume->startblock);
        bug("[InstallGrub2:SFS] %s: objectnode: %ld\n", __func__, objectnode);
        bug("[InstallGrub2:SFS] %s: %ld, %d, %d\n", __func__, volume->countblock, volume->SizeBlock, volume->partnum);
    )

    /* Clear the core.img sector pointers region! */
    memset((UBYTE*)&blocklist[-BLCKLIST_ELEMENTS],0x00, BLCKLIST_ELEMENTS*sizeof(struct BlockNode)); 

    /* Description of actions:
     * 1. Load SFS root block
     * 2. From root block find the block containing root of objectnodes
     * 3. Traverse the tree of objectnodes until block of objectdescriptor is found
     * 4. Search the objectdescriptor for entry matching given objectnode from entry read the
     *    first block of file
     * 5. Having first file block, find the extentbnode for that block and read number 
     *    of blocks. Put first block and number of blocks into BlockList.
     * 6. If the file has more blocks than this exntentbnode hold, find first file
     *    block in next extentbnode. Go to step 5.
     * Use the SFS source codes for reference. They operate on structures not pointers
     * and are much easier to understand.
     */
 
    /* Read root block */
    retval = _readwriteBlock(volume, 0, volume->blockbuffer, volume->SizeBlock<<2,
                volume->readcmd);

    if (retval)
    {
        D(bug("[InstallGrub2:SFS] %s: ERROR reading root block (error: %ld)\n", __func__, retval);)
        Printf("ReadError %lu\n", (long)retval);
        return 0;
    }

    /* Get block pointers from root block */
    block_objectnoderoot = AROS_BE2LONG(volume->blockbuffer[28]); /* objectnoderoot - 29th ULONG */
    block_extentbnoderoot = AROS_BE2LONG(volume->blockbuffer[27]); /* extentbnoderoot - 28th ULONG */
    
    D(bug("[InstallGrub2:SFS] %s: objectnoderoot: %ld, extentbnoderoot %ld\n", __func__, 
        block_objectnoderoot, block_extentbnoderoot);)

    /* Find the SFSObjectContainer block for given objectnode */
    /* Reference: SFS, nodes.c, function findnode */
    nextblock = block_objectnoderoot;
    D(bug("[InstallGrub2:SFS] %s: searching in nextblock %d for sfsobjectcontainer for objectnode %ld\n", __func__, 
        nextblock, objectnode);)
    while(1)
    {
        _readwriteBlock(volume, nextblock, volume->blockbuffer, volume->SizeBlock<<2,
                        volume->readcmd);
    
        /* If nodes == 1, we are at the correct nodecontainer, else go to next nodecontainer */
        if (AROS_BE2LONG(volume->blockbuffer[4]) == 1)
        {
            /* read entry from position: be_node + sizeof(fsObjectNode) * (objectnode - be_nodenumber) */
            tmpBytePtr = (UBYTE*)volume->blockbuffer;
            ULONG index = 20 + 10 * (objectnode - AROS_BE2LONG(volume->blockbuffer[3]));
            block_sfsobjectcontainer = AROS_BE2LONG(((ULONG*)(tmpBytePtr + index))[0]);
            D(bug("[InstallGrub2:SFS] %s: leaf found in nextblock %ld, sfsobjectcontainer block is %ld \n", __func__, 
                nextblock, block_sfsobjectcontainer);)
            break;
        }
        else
        {
            UWORD containerentry = 
                (objectnode - AROS_BE2LONG(volume->blockbuffer[3]))/AROS_BE2LONG(volume->blockbuffer[4]);
            nextblock = AROS_BE2LONG(volume->blockbuffer[containerentry + 5]) >> 4; /* 9-5 (2^9 = 512) */;
            D(bug("[InstallGrub2:SFS] %s: check next block %ld\n", __func__, nextblock);)
        }
    }

    if (block_sfsobjectcontainer == 0)
    {
        D(bug("[InstallGrub2:SFS] %s: SFSObjectContainer not found\n", __func__);)
        Printf("SFSObjectContainer not found\n");
        return 0;
    }

    /* Find the SFSObject in SFSObjectContainer for given objectnode */
    first_block = 0;
    while((block_sfsobjectcontainer != 0) && (first_block == 0))
    {
        /* Read next SFS container block */
        retval = _readwriteBlock(volume, block_sfsobjectcontainer, volume->blockbuffer, volume->SizeBlock<<2,
                        volume->readcmd);

        if (retval)
        {
            D(bug("[InstallGrub2:SFS] %s: ERROR reading block (error: %ld)\n", __func__, retval);)
            Printf("ReadError %lu\n", (long)retval);
            return 0;
        }

        /* Iterate over SFS objects and match the objectnode */
        /* 
         * The first offset comes from :
         * sizeof(sfsblockheader) = uint32 + uint32 + uint32 (field of sfsobjectcontainer)
         * parent, next, previous = uint32 + uint32 + uint32 (fields of sfsobjectcontainers)
         */
        tmpBytePtr = ((UBYTE*)volume->blockbuffer) + 12 + 12; /* tmpBytePtr points to first object in container */

        while (AROS_BE2LONG(((ULONG*)(tmpBytePtr + 4))[0]) > 0) /* check on the objectnode field */
        {
            
            /* Compare objectnode */
            if (AROS_BE2LONG(((ULONG*)(tmpBytePtr + 4))[0]) == objectnode)
            {
                /* Found! */
                first_block = AROS_BE2LONG(((ULONG*)(tmpBytePtr + 12))[0]); /* data */
                D(bug("[InstallGrub2:SFS] %s: first block is %ld\n", __func__, first_block);)
                break;
            }
            
            /* Move to next object */
            /* Find end of name and end of comment */
            tmpBytePtr += 25; /* Point to name */
            count = 0;
            for (i = 2; i > 0; tmpBytePtr++, count++)
                if (*tmpBytePtr == '\0')
                    i--;

            /* Correction for aligment */
            if ((count & 0x01) == 0 )
                tmpBytePtr++;
        }
        
        /* Move to next sfs object container block */
        block_sfsobjectcontainer =  AROS_BE2LONG(volume->blockbuffer[4]); /* next field */       
        
    }

    if (first_block == 0)
    {
        D(bug("[InstallGrub2:SFS] %s: First block not found\n", __func__);)
        Printf("First block not found\n");
        return 0;
    }

    /* First file block found. Find all blocks of file */
    searchedblock = first_block;
    blk_count = 0;

    while(1)
    {
        nextblock = block_extentbnoderoot;
        UBYTE * BNodePtr = NULL;
        
        while(1)
        {
            /* Find the extentbnode for this block */

            D(bug("[InstallGrub2:SFS] %s: searching in nextblock %d for extentbnode for block %ld\n", __func__, 
                nextblock, searchedblock);)

            UBYTE * BTreeContainerPtr = NULL;
            BNodePtr = NULL;

            _readwriteBlock(volume, nextblock, volume->blockbuffer, volume->SizeBlock<<2,
                            volume->readcmd);
            
            BTreeContainerPtr = (UBYTE*)(volume->blockbuffer + 3); /* Starts right after the header */

            D(bug("[InstallGrub2:SFS] %s: tree container nodecount: %d\n", __func__, 
                AROS_BE2WORD(((UWORD*)BTreeContainerPtr)[0]));)

            for (i = AROS_BE2WORD(((UWORD*)BTreeContainerPtr)[0]) - 1; i >=0; i--) /* Start from last element */
            {
                /* Read the BNode */
                tmpBytePtr = BTreeContainerPtr + 4 + i * BTreeContainerPtr[3];

                if (AROS_BE2LONG(((ULONG*)(tmpBytePtr))[0]) <= searchedblock) /* Check on the key field */
                {
                    BNodePtr = tmpBytePtr;
                    break;
                }
            }
            
            /* Fail if BNodePtr still NULL */
            if (BNodePtr == NULL)
            {
                D(bug("[InstallGrub2:SFS] %s: Failed to travers extentbnode tree.\n", __func__);)
                Printf("Failed to travers extentbnode tree.\n");
                return 0;
            }

            /* If we are at the leaf, stop */
            if (BTreeContainerPtr[2])
                break;

            /* Else search further */
            nextblock = AROS_BE2LONG(((ULONG*)(BNodePtr))[1]); /* data / next field */
        }

        /* Found. Add BlockList entry */
        D(bug("[InstallGrub2:SFS] %s: extentbnode for block %ld found. Block count: %d\n", __func__, 
            searchedblock, AROS_BE2WORD(((UWORD*)(BNodePtr + 12))[0]));)

        /* Add blocklist entry */
        blk_count--;

        /* Check if we still have spece left to add data to BlockList */
        if ((blk_count-1) <= -BLCKLIST_ELEMENTS)
        {
            D(bug("[InstallGrub2:SFS] %s: ERROR: out of block space\n", __func__);)
            Printf("There is no more space to save blocklist in core.img\n");
            return 0;
        }

        blocklist[blk_count].sector_lo = searchedblock;
        blocklist[blk_count].sector_hi = 0;
        blocklist[blk_count].count = AROS_BE2WORD(((UWORD*)(BNodePtr + 12))[0]);

        /* Handling of special situations */
        if (searchedblock == first_block)
        { 
            /* Writting first pack of blocks. Pointer needs to point to second file block */
            blocklist[blk_count].sector_lo++;
            blocklist[blk_count].count--;
            if (blocklist[blk_count].count == 0)
            {
                /* This means that the first pack of blocks contained only one block - first block */
                /* Since the first blocklist needs to start at second file block, 'reset' the blk_count */
                /* so that next iteration will overwrite the current results */
                blk_count++;
            }
        }
        
        /* Are there more blocks to read? */
        if (AROS_BE2LONG(((ULONG*)(BNodePtr))[1]) == 0)
        {
            D(bug("[InstallGrub2:SFS] %s: All core.img blocks found!\n", __func__);)
            break;
        }
        else
            searchedblock = AROS_BE2LONG(((ULONG*)(BNodePtr))[1]); /* data / next field */
    }


    /* Correct blocks for volume start */
        
    /* Blocks in blocklist are relative to the first sector of the HD (not partition) */
    i = 0;
    for (count=-1;count>=blk_count;count--)
    {
        blocklist[count].sector_lo += volume->startblock;
        blocklist[count].seg_adr = 0x820 + (i*32);
        i += blocklist[count].count;
        D(
            bug("[InstallGrub2:SFS] %s: correcting block %d for partition start\n", __func__, count);
            bug("[InstallGrub2:SFS] %s: sector : %ld seg_adr : %p\n", __func__, 
                blocklist[count].sector_lo, blocklist[count].seg_adr);
        )
    }

    first_block += volume->startblock;

    return first_block;
}

BOOL IGrub2SFSBESupport_init(void)
{
    struct FSSupport *fsHandler;

    D(bug("[InstallGrub2:SFS] %s: Initialising..\n", __func__));

    fsHandler = AllocMem(sizeof(struct FSSupport), MEMF_CLEAR);
    fsHandler->fss_Node.ln_Name = (char *)SFSBEName;
    fsHandler->fss_FilesystemIDs = SFSBEPartitionTypes;
    fsHandler->fss_CollectBlocks = SFSBECollectBlockList;

    IGrub2_RegisterFSHandler(fsHandler);

    return TRUE;
}

ADD2INIT(IGrub2SFSBESupport_init, 10);

#endif /* IGRUB2_SFSBESUPPORT */

#if defined(IGRUB2_SFSLESUPPORT)

static ULONG SFSLEPartitionTypes[] =
{
    ID_SFS_LE_DISK,
    -1
};

CONST_STRPTR  SFSLEName = "SFS-LE Filesystem";

#endif /* IGRUB2_SFSLESUPPORT */
