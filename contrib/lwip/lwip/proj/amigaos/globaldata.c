/*
    Copyright � 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: globaldata.c,v 1.1 2002/07/28 11:13:55 sebauer Exp $
*/

#include <exec/types.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>

#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

struct HostentNode localnode;
char *local_aliases[] = {"local",NULL};
struct in_addr local = {0x7f000001};
struct in_addr *local_addr[] = {&local,NULL};

/**************************************************************************
 ...
**************************************************************************/
struct GlobalData *GlobalData_New(struct Library *SocketBase)
{
    struct GlobalData *gldata = AllocVec(sizeof(struct GlobalData),MEMF_PUBLIC|MEMF_CLEAR);
    if (!gldata) return NULL;
    if (!(gldata->pool = CreatePool(MEMF_PUBLIC,4096,4096)))
    {
	FreeVec(gldata);
	return NULL;
    }
    InitSemaphore(&gldata->hosts_semaphore);
    NewList((struct List*)&gldata->static_hosts_list);
    NewList((struct List*)&gldata->dns_hosts_list);

    /* hardcoded */
    localnode.hostent.h_name = "localhost";
    localnode.hostent.h_aliases = local_aliases;
    localnode.hostent.h_length = sizeof(struct in_addr);
    localnode.hostent.h_addr_list = (char**)local_addr;
    localnode.hostent.h_addrtype = AF_INET;

    AddTail((struct List*)&gldata->static_hosts_list,(struct Node*)&localnode);

    return gldata;
}

/**************************************************************************
 ...
**************************************************************************/
void GlobalData_Dispose(struct Library *SocketBase, struct GlobalData *gldata)
{
    if (!gldata) return;
    if (gldata->pool) DeletePool(gldata->pool);
    FreeVec(gldata);
}

/**************************************************************************
 ...
**************************************************************************/
void *gl_mem_allocate(struct Library *SocketBase, int size)
{
    int *mem = AllocPooled(GLDATA(SocketBase)->pool,size+sizeof(int));
    if (!mem) return NULL;
    mem[0] = size;
    return mem+1;
}

/**************************************************************************
 ...
**************************************************************************/
void gl_mem_free(struct Library *SocketBase, void *mem)
{
    int *m = mem;
    if (m)
    {
	m--;
	FreePooled(LOCDATA(SocketBase)->pool,m,m[0]+sizeof(int));
    }
}

