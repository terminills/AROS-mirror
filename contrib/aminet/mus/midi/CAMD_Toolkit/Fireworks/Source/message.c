
#include "fireworks.h"
#include "fireworks_protos.h"

extern BOOL WBMode;


/*----------------------------*/
/* Show a message to the user */
/*----------------------------*/

LONG MessageA(UBYTE *Msg,UBYTE *Options,APTR Args)
{
	LONG retval;
	
	BOOL req = FALSE;

	if (Options && strchr(Options,'|'))
		req = TRUE;
	
	if (IntuitionBase && (WBMode || req))
	{
		struct EasyStruct Req={sizeof(struct EasyStruct),0,"Fireworks",0, NULL};
		
		if (!Options) Options = "I see";
		
		Req.es_TextFormat=Msg;
		Req.es_GadgetFormat=Options;
		
		retval=EasyRequestArgs(((struct Process*)MyTask)->pr_WindowPtr,&Req,0,Args);
	}
	else
	{
		if (DOSBase)
		{
			VPrintf(Msg,Args);
			Printf("\n");
		}
		retval=0;
	}
	return(retval);
}


/*------------------------------------------*/
/* Show non-blocking (asynchronous) message */
/*------------------------------------------*/


void AsyncMessageFunction(struct Globals *glob, struct Prefs *pref, APTR UserData)
{
	struct MsgData *md = (struct MsgData*)UserData;
	
	MessageA(md->Msg, md->Options, md->Args);
}

#if !defined(__AROS__)
LONG __stdargs AsyncMessage(struct Globals *glob, TaskFlag flg, UBYTE *Msg,UBYTE *Options,...)
{
	BOOL req = FALSE;
	struct MsgData md;

	va_list Args;
	va_start(Args,Options);

	// if (Options) if (strchr(Options,'|')) req = TRUE;
	if (Options) req = TRUE;

	if (IntuitionBase && (WBMode || req))
	{
		md.Msg = Msg;
		md.Options= Options;
		md.Args = Args;

		if (!StartAsyncTask(glob, NULL, "Fireworks messaging task", flg, &AsyncMessageFunction, &md, sizeof(struct MsgData)))
		{
			MessageA(Msg,Options,md.Args);
		}
	}
	va_end(Args);
	
	return(0);
}
#endif
