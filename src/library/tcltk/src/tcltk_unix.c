
#include "tcltk.h" /* declarations of our `public' interface */

#ifndef Win32
#include <R_ext/eventloop.h>
#endif

#include <R.h>
#include <stdlib.h>

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) dgettext ("tcltk", String)
#else
#define _(String) (String)
#endif

extern int (*R_timeout_handler)();
extern long R_timeout_val;


/* R event structure */
typedef struct {
    Tcl_EventProc *proc;
    struct Tcl_Event *nextPtr;
} RTcl_Event;



#define R_INTERFACE_PTRS 1
#include <Rinterface.h>  /* R_GUIType and more for console */
/* Add/delete Tcl/Tk event handler */

static void (* OldHandler)(void);
static int OldTimeout;
static int Tcl_loaded = 0;
static int Tcl_lock = 0; /* reentrancy guard */

static void TclSpinLoop(void *data)
{
    while (Tcl_DoOneEvent(TCL_DONT_WAIT))
	;
}

static void TclHandler(void)
{
    if (!Tcl_lock && Tcl_GetServiceMode() != TCL_SERVICE_NONE) {
	Tcl_lock = 1;
	(void) R_ToplevelExec(TclSpinLoop, NULL);
	Tcl_lock = 0;
    }
    /* Tcl_ServiceAll is not enough here, for reasons that escape me */
    OldHandler();
}

static int Gtk_TclHandler(void)
{
    if (!Tcl_lock && Tcl_GetServiceMode() != TCL_SERVICE_NONE) {
	Tcl_lock = 1;
	(void) R_ToplevelExec(TclSpinLoop, NULL);
	Tcl_lock = 0;
    }
    return 1;
}

static void addTcl(void)
{
    if (Tcl_loaded)
	error(_("Tcl already loaded"));
    Tcl_loaded = 1;
    if (strcmp(R_GUIType, "GNOME") == 0) {
	/* This gets polled in the gnomeGUI console's event loop */
        R_timeout_handler = Gtk_TclHandler;
        R_timeout_val = 500;
    } else {
        OldHandler = R_PolledEvents;
	OldTimeout = R_wait_usec;
	R_PolledEvents = TclHandler;
	if ( R_wait_usec > 10000 || R_wait_usec == 0)
	    R_wait_usec = 10000;
    }
}

void delTcl(void)
{
    if (!Tcl_loaded)
	error(_("Tcl is not loaded"));
    if (strcmp(R_GUIType, "GNOME") == 0) {
        R_timeout_handler = NULL;
        R_timeout_val = 0;
    } else {
        if (R_PolledEvents != TclHandler)
	    error(_("Tcl is not last loaded handler"));
	R_PolledEvents = OldHandler;
	R_wait_usec = OldTimeout;
    }
    Tcl_loaded = 0;
}

/* ----- Event loop interface routines -------- */
static Tcl_Time timeout;

static void RTcl_setupProc(ClientData clientData, int flags)
{
    Tcl_SetMaxBlockTime(&timeout);
}
static void RTcl_eventProc(RTcl_Event *evPtr, int flags)
{
    fd_set *readMask = R_checkActivity(0 /*usec*/, 1 /*ignore_stdin*/);

    if (readMask==NULL)
	return;

    R_runHandlers(R_InputHandlers, readMask);
}
static void RTcl_checkProc(ClientData clientData, int flags)
{
    fd_set *readMask = R_checkActivity(0 /*usec*/, 1 /*ignore_stdin*/);
    RTcl_Event * evPtr;
    if (readMask==NULL)
	return;

    evPtr = (RTcl_Event*) Tcl_Alloc(sizeof(RTcl_Event));
    evPtr->proc = (Tcl_EventProc*) RTcl_eventProc;

    Tcl_QueueEvent((Tcl_Event*) evPtr, TCL_QUEUE_HEAD);
}


void Tcl_unix_setup(void)
{
    addTcl(); /* notice: this sets R_wait_usec.... */
    timeout.sec = 0;
    timeout.usec = R_wait_usec;
    Tcl_CreateEventSource(RTcl_setupProc, RTcl_checkProc, 0);
}



#ifndef TCL80
/* ----- Tcl/Tk console routines ----- */

/* From former src/unix/devUI.h
extern int  (*ptr_R_ReadConsole)(char *, unsigned char *, int, int);
extern void (*ptr_R_WriteConsole)(char *, int);
extern void (*ptr_R_ResetConsole)();
extern void (*ptr_R_FlushConsole)();
extern void (*ptr_R_ClearerrConsole)();
extern FILE * R_Consolefile;
extern FILE * R_Outputfile; */

/* Fill a text buffer with user typed console input. */

static int
RTcl_ReadConsole (char *prompt, unsigned char *buf, int len,
		  int addtohistory)
{
    Tcl_Obj *cmd[3];
    int i, code;

    cmd[0] = Tcl_NewStringObj("Rc_read", -1);
    cmd[1] = Tcl_NewStringObj(prompt, -1);
    cmd[2] = Tcl_NewIntObj(addtohistory);

    for (i = 0 ; i < 3 ; i++)
	Tcl_IncrRefCount(cmd[i]);

    code = Tcl_EvalObjv(RTcl_interp, 3, cmd, 0);
    if (code != TCL_OK)
	return 0;
    else
#ifdef SUPPORT_MBCS
    {
	    char *buf_utf8;
	    Tcl_DString buf_utf8_ds;
	    Tcl_DStringInit(&buf_utf8_ds);
	    buf_utf8 =
		    Tcl_UtfToExternalDString(NULL,
		    			     Tcl_GetStringResult(RTcl_interp),
					     len,
					     &buf_utf8_ds);
            strncpy((char *)buf, buf_utf8, len);
	    Tcl_DStringFree(&buf_utf8_ds);
    }
#else /* SUPPORT_MBCS */
	strncpy((char *)buf, (char *) Tcl_GetStringResult(RTcl_interp), len);
#endif /* SUPPORT_MBCS */

    /* At some point we need to figure out what to do if the result is
     * longer than "len"... For now, just truncate. */

    for (i = 0 ; i < 3 ; i++)
	Tcl_DecrRefCount(cmd[i]);

    return 1;
}

/* Write a text buffer to the console. */
/* All system output is filtered through this routine. */
static void
RTcl_WriteConsole (char *buf, int len)
{
    Tcl_Obj *cmd[2];
#ifdef SUPPORT_MBCS
    char *buf_utf8;
    Tcl_DString  buf_utf8_ds;

    Tcl_DStringInit(&buf_utf8_ds);
    buf_utf8 = Tcl_ExternalToUtfDString(NULL, buf, -1, &buf_utf8_ds);
#endif /* SUPPORT_MBCS */

    /* Construct command */
    cmd[0] = Tcl_NewStringObj("Rc_write", -1);
#ifdef SUPPORT_MBCS
    cmd[1] = Tcl_NewStringObj(buf_utf8, -1);
#else /* SUPPORT_MBCS */
    cmd[1] = Tcl_NewStringObj(buf, len);
#endif /* SUPPORT_MBCS */

    Tcl_IncrRefCount(cmd[0]);
    Tcl_IncrRefCount(cmd[1]);

    Tcl_EvalObjv(RTcl_interp, 2, cmd, 0);

    Tcl_DecrRefCount(cmd[0]);
    Tcl_DecrRefCount(cmd[1]);
#ifdef SUPPORT_MBCS
    Tcl_DStringFree(&buf_utf8_ds);
#endif /* SUPPORT_MBCS */
}

/* Indicate that input is coming from the console */
static void
RTcl_ResetConsole ()
{
}

/* Stdio support to ensure the console file buffer is flushed */
static void
RTcl_FlushConsole ()
{
}


/* Reset stdin if the user types EOF on the console. */
static void
RTcl_ClearerrConsole ()
{
}

void RTcl_ActivateConsole ()
{
    ptr_R_ReadConsole = RTcl_ReadConsole;
    ptr_R_WriteConsole = RTcl_WriteConsole;
    ptr_R_ResetConsole = RTcl_ResetConsole;
    ptr_R_FlushConsole = RTcl_FlushConsole;
    ptr_R_ClearerrConsole = RTcl_ClearerrConsole;
    R_Consolefile = NULL;
    R_Outputfile = NULL;
}
#endif
