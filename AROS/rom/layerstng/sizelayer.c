/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include <aros/libcall.h>


/*****************************************************************************

    NAME */
#include <proto/layers.h>
#include "layers_intern.h"

	AROS_LH4(LONG, SizeLayer,

/*  SYNOPSIS */
	AROS_LHA(LONG          , dummy, A0),
	AROS_LHA(struct Layer *, l    , A1),
	AROS_LHA(LONG          , dx   , D0),
	AROS_LHA(LONG          , dy   , D1),

/*  LOCATION */
	struct LayersBase *, LayersBase, 11, Layers)

/*  FUNCTION
        Resizes the given layer by adding dx to its width and dy
        to its height.
        If parts of simple layers become visible those parts are
        added to the damage list and a refresh is triggered for
        those layers. 
        If the new layer is bigger than before the additional parts
        are added to a damage list if the layer is a non-super-
        bitmap layer. Refresh is also triggered for this layer.

    INPUTS
        l    - pointer to layer to be resized
        dx   - delta to be added to the width
        dy   - delta to be added to the height

    RESULT
        TRUE  - layer could be resized
        FALSE - error occurred (out of memory)

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    layers_lib.fd and clib/layers_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct LayersBase *,LayersBase)

    return 0;

    AROS_LIBFUNC_EXIT

} /* SizeLayer */
