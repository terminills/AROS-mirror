/*
    (C) 1995-96 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang:
*/


#include <aros/libcall.h>
#include <proto/mathieeedoubtrans.h>
#include <proto/exec.h>
#include <exec/types.h>
#include "mathieeedoubtrans_intern.h"


/*****************************************************************************

    NAME */

      AROS_LHQUAD1(LONG, IEEEDPTieee,

/*  SYNOPSIS */
      AROS_LHAQUAD(QUAD, y, D0, D1),

/*  LOCATION */
      struct MathIeeeDoubBasBase *, MathIeeeDoubBasBase, 9, Mathieeedoubbas)

/*  FUNCTION
        Convert IEEE double to IEEE single precision number

    INPUTS
        y  - IEEE double precision number

    RESULT

        IEEE single precision number

        Flags:
          zero     : result is zero
          negative : result is negative
          overflow : value was out of range for IEEE single precision

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/
{
AROS_LIBFUNC_INIT
LONG Res, tmp;

  SetSR(0, Zero_Bit | Overflow_Bit | Negative_Bit );

  if (is_eqC(y, 0x0, 0x0, 0x0UUL))
  {
    SetSR(Zero_Bit, Zero_Bit | Overflow_Bit | Negative_Bit );
    return 0;
  }

  SHRU32(Res, y, 52 );
  SHRU32(tmp, y, 29 );
  /* calculate the exponent */
  Res &=0x7ff;
  Res = Res + 0x7e - 0x3fe;
  Res <<= 23;

  Res |= (tmp & IEEESPMantisse_Mask);

  if (is_lessSC(y, 0x0, 0x0, 0x0UUL))
  {
    SetSR(Negative_Bit, Zero_Bit | Overflow_Bit | Negative_Bit );
    Res |= IEEESPSign_Mask;
  }

  return Res;
AROS_LIBFUNC_EXIT
} /* IEEEDPTieee */
