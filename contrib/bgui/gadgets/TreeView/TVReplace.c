/*
 * @(#) $Header$
 *
 * BGUI Tree List View class
 *
 * (C) Copyright 1999 Manuel Lemos.
 * (C) Copyright 1996-1999 Nick Christie.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 1.2  2000/05/09 20:00:44  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:56  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:01  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********************  TREEVIEW CLASS: REPLACE  ***********************
************************************************************************/

/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include "TreeViewPrivate.h"
#include "TVUtil.h"

/************************************************************************
**************************  LOCAL DEFINITIONS  **************************
************************************************************************/


/************************************************************************
*************************  EXTERNAL REFERENCES  *************************
************************************************************************/

/*
 * Functions from TVUtil are listed in TVUtil.h
 */

/************************************************************************
*****************************  PROTOTYPES  ******************************
************************************************************************/

ASM ULONG TV_Replace(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvReplace *tvr);

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
****************************  TV_REPLACE()  *****************************
************************************************************************/

ASM ULONG TV_Replace(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvReplace *tvr)
{
return(0);
}

