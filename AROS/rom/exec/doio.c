/*
    Copyright © 1995-2007, The AROS Development Team. All rights reserved.
    $Id$
    Desc: Start an IO request and wait until it completes.
    Lang: english
*/
#include <exec/execbase.h>
#include <exec/io.h>
#include <aros/libcall.h>
#include <proto/exec.h>

#define DEBUG 1
#include <aros/debug.h>

/*****************************************************************************
    NAME */

	AROS_LH1(LONG, DoIO,

/*  SYNOPSIS */
	AROS_LHA(struct IORequest *, iORequest, A1),

/*  LOCATION */
	struct ExecBase *, SysBase, 76, Exec)

/*  FUNCTION
	Start an I/O request by calling the devices's BeginIO() vector.
	It waits until the request is complete.
    INPUTS
	iORequest - Pointer to iorequest structure.
    RESULT
    NOTES
	OpenDevice() notes explain LONG return type.
    EXAMPLE
    BUGS
    SEE ALSO
	OpenDevice(), CloseDevice(), DoIO(), SendIO(), AbortIO(), WaitIO()
    INTERNALS
******************************************************************************/
{
    AROS_LIBFUNC_INIT
D(bug("[%s:%u] %s(%04x)\n", iORequest->io_Device->dd_Library.lib_Node.ln_Name, iORequest->io_Unit, __func__, iORequest->io_Command));
    /*
	Prepare the message. Tell the device that it is OK to wait in the
	BeginIO() call by setting the quick bit.
    */
    ASSERT_VALID_PTR(iORequest);
    if (!iORequest) return -1;
D(bug("[%s:%u] %s: iORequest @ 0x%p\n", iORequest->io_Device->dd_Library.lib_Node.ln_Name, iORequest->io_Unit, __func__, iORequest));
    iORequest->io_Flags=IOF_QUICK;
    iORequest->io_Message.mn_Node.ln_Type=0;

    ASSERT_VALID_PTR(iORequest->io_Device);
    if (!iORequest->io_Device) return -1;
D(bug("[%s:%u] %s: Calling device ...\n", iORequest->io_Device->dd_Library.lib_Node.ln_Name, iORequest->io_Unit, __func__));
    /* Call BeginIO() vector */
    AROS_LVO_CALL1NR(void,
	AROS_LCA(struct IORequest *,iORequest,A1),
	struct Device *,iORequest->io_Device,5,
    );
//D(bug("DoIO: iORequest->io_Flags & IOF_QUICK %d\n", iORequest->io_Flags&IOF_QUICK));

    /* If the quick flag is cleared it wasn't done quickly. Wait for completion. */
    if(!(iORequest->io_Flags&IOF_QUICK))
      {
        D(bug("[%s:%u] %s: Waiting for IO to complete ...\n", iORequest->io_Device->dd_Library.lib_Node.ln_Name, iORequest->io_Unit, __func__));
	WaitIO(iORequest);
        //D(bug("DoIO: After WaitIO\n"));
      }
D(bug("[%s:%u] %s: Command %04x io_Error=%d\n", iORequest->io_Device->dd_Library.lib_Node.ln_Name, iORequest->io_Unit, __func__, iORequest->io_Command, iORequest->io_Error));
    /* All done. Get returncode. */
    return iORequest->io_Error;
    AROS_LIBFUNC_EXIT
} /* DoIO */
