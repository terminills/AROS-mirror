/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc: loader for AmigaDOS hunk files.
    Lang: english
*/
#include <exec/memory.h>
#include <proto/exec.h>
#include <dos/dosasl.h>
#include <dos/doshunks.h>
#include <proto/dos.h>
#include <proto/aros.h>
#include "dos_intern.h"
#undef DEBUG
#define DEBUG 1
#include <aros/debug.h>

extern struct DosLibrary * DOSBase;

struct hunk
{
  ULONG size;
  UBYTE *memory;
};

static int read_block(BPTR file, APTR buffer, ULONG size)
{
  LONG subsize;
  UBYTE *buf=(UBYTE *)buffer;

  while(size) {
    subsize=Read(file,buf,size);
    if(subsize==0) {
      ((struct Process *)FindTask(NULL))->pr_Result2=ERROR_BAD_HUNK;
      return 1;
    }
    if(subsize<0)
      return 1;
    buf+=subsize;
    size-=subsize;
  }
  return 0;
}

BPTR LoadSeg_AOS(BPTR file)
{
  struct hunk *hunktab = NULL;
  ULONG hunktype, count, first, last, offset, curhunk, numhunks;
  LONG t;
  UBYTE name_buf[255];
  register i;
  BPTR last_p = 0;
  static STRPTR segtypes[] = { "CODE", "DATA", "BSS", };

#define ERROR(a)    { *error=a; goto end; }
  LONG *error=&((struct Process *)FindTask(NULL))->pr_Result2;

  if (Seek(file, 0, OFFSET_BEGINNING) < 0)
    goto end;
  while(!read_block(file, &hunktype, sizeof(hunktype))) {
    switch(hunktype) {
      ULONG tmp, req;

    case HUNK_SYMBOL:
      while(!read_block(file, &count, sizeof(count)) && count) {
	if (Seek(file, (count+1)*4, OFFSET_CURRENT) < 0)
	  goto end;
      }
      break;
    case HUNK_UNIT:
      if (read_block(file, &count, sizeof(count)))
	goto end;
      count /= 4;
      if (read_block(file, name_buf, count))
	goto end;
      D(bug("HUNK_UNIT: \"%.*s\"\n", count, name_buf));
      break;
    case HUNK_HEADER:
      D(bug("HUNK_HEADER:\n"));
      while (1) {
	if (read_block(file, &count, sizeof(count)))
	  goto end;
	if (count == 0L)
	  break;
	count *= 4;
	if (read_block(file, name_buf, count))
	  goto end;
	D(bug("\tlibname: \"%.*s\"\n", count, name_buf));
      }
      if (read_block(file, &numhunks, sizeof(numhunks)))
	goto end;
      D(bug("\tHunk count: %ld\n", numhunks));
      hunktab = (struct hunk *)AllocVec(sizeof(struct hunk) * numhunks,
					MEMF_CLEAR);
      if (hunktab == NULL)
	ERROR(ERROR_NO_FREE_STORE);
      if (read_block(file, &first, sizeof(first)))
	goto end;
      D(bug("\tFirst hunk: %ld\n", first));
      curhunk = first;
      if (read_block(file, &last, sizeof(last)))
	goto end;
      D(bug("\tLast hunk: %ld\n", last));
      for (i = first; i <= last; i++) {
	if (read_block(file, &count, sizeof(count)))
	  goto end;
	tmp = count & 0xC0000000;
	count &= 0x3FFFFFFF;
	D(bug("\tHunk %d size: 0x%06lx bytes in ", i, count*4));
	switch(tmp) {
	case 0x80000000:
	  D(bug("FAST"));
	  req = MEMF_FAST;
	  break;
	case 0x40000000:
	  D(bug("CHIP"));
	  req = MEMF_CHIP;
	  break;
	default:
	  D(bug("ANY"));
	  req = MEMF_ANY;
	  break;
	}
	D(bug(" memory\n"));
	hunktab[i].size = count * 4;
	hunktab[i].memory = (UBYTE *)AllocVec(hunktab[i].size + sizeof(BPTR),
	                                      (req | MEMF_CLEAR));
	if (hunktab[i].memory == NULL)
	  ERROR(ERROR_NO_FREE_STORE);
	hunktab[i].memory += sizeof(BPTR);
      }
      break;
    case HUNK_CODE:
    case HUNK_DATA:
    case HUNK_BSS:
      if (read_block(file, &count, sizeof(count)))
	goto end;
      tmp = count & 0xC0000000;
      count &= 0x3fffffff;
      D(bug("HUNK_%s(%d): Length: 0x%06lx bytes in ",
	    segtypes[hunktype-HUNK_CODE], curhunk, count*4));
      switch(tmp) {
      case 0x8000000:
	D(bug("FAST"));
	req = MEMF_FAST;
	break;
      case 0x40000000:
	D(bug("CHIP"));
	req = MEMF_CHIP;
	break;
      default:
	D(bug("ANY"));
	req = MEMF_ANY;
	break;
      }
      D(bug(" memory\n"));
      if (hunktype != HUNK_BSS && count)
	if (read_block(file, hunktab[curhunk].memory, count*4))
	  goto end;
      break;
    case HUNK_RELOC32:
      D(bug("HUNK_RELOC32:\n"));
      while (1) {
	ULONG *addr;

	if (read_block(file, &count, sizeof(count)))
	  goto end;
	if (count == 0L)
	  break;
	i = count;
	if (read_block(file, &count, sizeof(count)))
	  goto end;
	D(bug("\tHunk #%ld:\n", count));
	while (i > 0) {
	  if (read_block(file, &offset, sizeof(offset)))
	    goto end;
	  D(bug("\t\t0x%06lx\n", offset));
	  addr = (ULONG *)(hunktab[curhunk].memory + offset);
	  *addr += (ULONG)(hunktab[count].memory);
	  --i;
	}
      }
      break;
    case HUNK_END:
      D(bug("HUNK_END\n"));
      ++curhunk;
      break;
    case HUNK_RELOC16:
    case HUNK_RELOC8:
    case HUNK_NAME:
    case HUNK_EXT:
    case HUNK_DEBUG:
    case HUNK_OVERLAY:
    case HUNK_BREAK:
    default:
      ERROR(ERROR_BAD_HUNK);
    }
  }
  /* Clear caches */
  for (t=last; t >= (LONG)first; t--) {
    if (hunktab[t].size) {
      CacheClearE(hunktab[t].memory, hunktab[t].size, CACRF_ClearI|CACRF_ClearD);
      ((BPTR *)hunktab[t].memory)[-1] = last_p;
      last_p = MKBADDR((BPTR *)hunktab[t].memory-1);
    }
  }
  FreeVec(hunktab);
  hunktab = NULL;
end:
  if (hunktab != NULL) {
    for (t = first; t <= last; t++)
      if (hunktab[t].memory != NULL)
	FreeVec(hunktab[t].memory - sizeof(BPTR));
    FreeVec(hunktab);
  }
  return last_p;
}
