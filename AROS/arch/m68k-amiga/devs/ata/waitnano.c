/*
    Copyright © 2013-2019, The AROS Development Team. All rights reserved
    $Id$
*/

#include <exec/types.h>

#include "timer.h"
#include "ata.h"


BOOL ata_Calibrate(struct IORequest* tmr, struct ataBase *base)
{
    base->ata_ItersPer100ns = 1;
    return TRUE;
}

void ata_WaitNano(ULONG ns, struct ataBase *base)
{
    asm volatile("tst.b 0xbfe001":::"cc");
}
