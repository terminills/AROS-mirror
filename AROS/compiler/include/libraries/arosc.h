#ifndef _AROSC_H
#define _AROSC_H

/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc: macros for the shared library version of clib
    Lang: english
*/

#define AROS_ALMOST_COMPATIBLE

#include <dos/dos.h>
#include <exec/tasks.h>
#include <exec/semaphores.h>
#include <proto/exec.h>

struct AroscUserData
{
    struct AroscUserData *olduserdata;

    /*
      This will be useful in case the structure will be extended.
      In this way the library will always know what can "offer" to the
      program
    */
    ULONG usersize;

    /* these fields are initialized by the program */
    void *errnoptr;
    void **stdinptr;
    void **stdoutptr;
    void **stderrptr;
    void *startup_jmp_bufptr;
    void *startup_errorptr;

    /* these fields are for internal use only */
    void *env_list;
    struct MinList stdio_files;
    struct SignalSemaphore startup_memsem;
    struct DateStamp startup_datestamp;
    void *startup_mempool;
    int numslots;
    void **fd_array;
    void *stdfiles[3];

    /* these fields are initialized internally and passed to the program */
    unsigned short int *const ctype_b;
    int *const ctype_toupper;
    int *const ctype_tolower;

    /*more stuff*/
    struct MinList atexit_list;
};

extern struct Library *aroscbase;

#define AROSCNAME "arosc.library"

#ifdef _CLIB_KERNEL_

#define CLIB_USES_ETASK 1

#if CLIB_USES_ETASK
#   include "etask.h"
#   define AROSC_USERDATA(task) (struct AroscUserData *)(GetIntETask(FindTask(task))->iet_AroscUserData)
#else
#   define AROSC_USERDATA(task) (struct AroscUserData *)(FindTask(task)->tc_UserData)
#endif

#define GETUSER struct AroscUserData *clib_userdata = AROSC_USERDATA(0)

#define errno               (*(int *)         (clib_userdata->errnoptr))
#define stdin               (*(FILE **)       (clib_userdata->stdinptr))
#define stdout              (*(FILE **)       (clib_userdata->stdoutptr))
#define stderr              (*(FILE **)       (clib_userdata->stderrptr))
#define __startup_jmp_buf   (*(jmp_buf *)     (clib_userdata->startup_jmp_bufptr))
#define __startup_error     (*(LONG *)        (clib_userdata->startup_errorptr))
#define __env_list          (*((__env_item **)(&(clib_userdata->env_list))))
#define __stdio_files                         (clib_userdata->stdio_files)
#define __numslots                            (clib_userdata->numslots)
#define __fd_array          ((fdesc **)       (clib_userdata->fd_array))
#define __startup_memsem                      (clib_userdata->startup_memsem)
#define __startup_mempool   ((APTR)           (clib_userdata->startup_mempool))
#define __startup_datestamp                   (clib_userdata->startup_datestamp)
#define __stdfiles                            (clib_userdata->stdfiles)
#define __atexit_list                         (clib_userdata->atexit_list)

#else

#define GETUSER const char clib_dummyvar __attribute__((unused))

#endif

#endif /*!_AROSC_H*/





