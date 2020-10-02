/*	CORO1.C -- Coroutine package for BDS C

Copyright (c) 1983 by Kevin B. Kenny

Released to the BDS C Users' Group, Inc. for non-commercial distribution

	Kevin Kenny
	729-A E. Cochise Dr.
	Phoenix, Arizona   85020

	This file comprises the portions of the BDS C coroutine
package that are implemented in C.

c_call		Call another coroutine
c_callby	Change the current coroutine's caller
c_caller	Find the current coroutine's caller
c_create	Create a coroutine
c_destroy	Destroy a coroutine (works only if ALLOC_ON defined)
c_detach	Return to the coroutine that called this one.
c_getinfo	Retrieve information specified by coroutine's creator.
c_passer	Find coroutine that last invoked current one.
c_resume	Lateral transfer to another coroutine.
c_setinfo	Change information originally specified by creator.
c_type		Find out how current coroutine was invoked.
c_wmi		Find current coroutine (Who aM I?)

	Refer to the accompanying documentation for a detailed description
of these functions.
*/

#include <bdscio.h>
#include <coro.h>

/* CALL -- Call another coroutine, possibly passing a value */

/*  Usage:
	retval = c_call (fcv, passval);
    Where:
	fcv	is a pointer to the target coroutine's FCV.
	passval	is passed as a value to the target coroutine.
	retval	is the value passed on the next invocation of this
		coroutine (usually via c_detach)
*/

int c_call (fcv, passval) 
    C_FCV * fcv; int passval;
    {
	fcv -> cf_caller_ = c_wmi ();	/* Set target's caller */
	fcv -> cf_type_ = CT_CALL;		/* Set transfer type */
	return (corpass (fcv, passval));	/* Do the transfer */
	}


/* CALLBY -- Change the caller of the current coroutine */

/*  Usage:
	c_callby (fcv);
    Where:
	fcv	is a pointer to the FCV of the coroutine that will become
		the caller.
*/

int c_callby (fcv)
    C_FCV * fcv;
    {
	c_wmi () -> cf_caller_ = fcv;
	}


/* CALLER -- Find the caller of the current co-routine */

/* Usage:
	fcv = c_caller ();
   Where:
	fcv 	(returned) is a pointer to the FCV of the current
		co-routine's caller.
*/

C_FCV * c_caller ()
    {
	return (c_wmi () -> cf_caller_);	
	}


/* CREATE -- Create a coroutine */

/*  Usage:
	fcv = c_create (funct, stack, info);
    Where:
	funct	is the "main program" (initial function entry) of the
		coroutine.
	stack	is the size of the coroutine's stack area in bytes.
	info	is an information word (generally a pointer to a
		static storage area) describing the coroutine.  It
		can later be retrieved by "getinfo" or modified by "setinfo".
	fcv	(returned) is a pointer to the newly-created coroutine's
		FCV, or ERROR if the coroutine could not be created
		because suficient memory space was not available.
*/

C_FCV * c_create (funct, stack, info)
    int (*funct) ();
    int stack;
    int info;
    {
	union fcvp {
	    C_FCV * f;
	    char  * b;
	    int i;
	    } fcv;
	struct c_initstk_ * istk;
	int corstart ();		/* Coroutine starter */
	char * sbrk ();			/* If coroutines are to be created,
					   a different memory manager is
					   used.  Make sure the right one
					   is loaded. */

	fcv.b=alloc (stack + sizeof *(fcv.f) + sizeof *istk);

	if (fcv.i == ERROR) return (ERROR);
	
	setmem (fcv.b, stack + sizeof *(fcv.f) + sizeof *istk, 0);
					/* Clear the FCV and stack */

	/* Set the stack size and stack pointer words in the fcv, and
	   build the initial stack content */

	(fcv.f) -> cf_stksiz_ = stack + sizeof *istk + sizeof *(fcv.f);
	istk = (fcv.f) -> cf_sp_ = fcv.i + sizeof *(fcv.f) + stack;
	istk -> ci_initfn_ = funct;
	istk -> ci_start_ = &corstart;
	
	/* Build the user-specified information word and caller linkage.
	   Passer will be set the first time the coroutine is called. */

	(fcv.f) -> cf_caller_ = c_wmi ();
	(fcv.f) -> cf_info_ = info;

	return (fcv.f);
	}

/* DESTROY -- Destroy a coroutine */

/* Usage:
	destroy (fcv);
   Where:
	fcv	is a pointer to the fcv of the coroutine to be
		destroyed.
*/

int c_destroy (fcv)
    C_FCV *fcv;
    {
	free (fcv);
	}

/* DETACH -- Return to the coroutine that called this one */

/* Usage:
	retval = c_detach (passval);
   Where:
	passval	is passed as a value to the coroutine that called
		the present one.
	retval	is the value passed on the next call to the present
		coroutine.
*/

int c_detach (passval)
    int passval;
    {
	C_FCV *fcv;
	fcv = c_caller ();
	fcv -> cf_type_ = CT_DETACH;
	return (corpass (fcv, passval));
	}

/* GETINFO -- Get information specified about a coroutine */

/* Usage:
	info = c_getinfo (fcv);
   Where:
	fcv	is a pointer to the FCV of the coroutine to query.
	info	is the "info" word set in the FCV by "create" or "setinfo".
   Notes:
	The "info" word is designed to keep coroutine-local static storage.
	It will generally be a pointer to the static storage area.
*/

int c_getinfo (fcv)
    C_FCV * fcv;
    {
	return (fcv -> cf_info_);
	}

/* PASSER -- Find coroutine that last transferred control to us. */

/* Usage:
	fcv = c_passer ();
   Where:
	fcv	is a pointer to the FCV of the coroutine that last
		transferred control to the present one by any means.
*/

C_FCV * c_passer () {
	return (c_wmi() -> cf_passer_);
	}

/* RESUME -- GO TO another coroutine, inheriting caller */

/* Usage:
	retval = c_resume (fcv, passval);
   Where:
	fcv	is a pointer to the FCV of the coroutine to be
		resumed.  If this coroutine DETACHes, iut
		will return to the caller of the coroutine
		that did the C_RESUME.
	passval	is the value to pass to the target coroutine.
	retval	is the value passed on the next invocation of the
		present coroutine.
*/

int c_resume (fcv, passval)
    C_FCV *fcv; int passval;
    {
	fcv -> cf_caller_ = c_wmi() -> cf_caller_;
	fcv -> cf_type_ = CT_RESUME;
	return (corpass (fcv, passval));
	}

/* SETINFO -- Change the "info" word in FCV */

/* Usage:
	setinfo (fcv, info);
   Where:
	fcv	is a pointer to the FCV of the coroutine to be altered.
	info	is the new content of the "info" word.
*/

int c_setinfo (fcv, info)
    C_FCV * fcv;
    int info;
    {
	return (fcv -> cf_info_ = info);
	}

/* TYPE -- Find out how the current coroutine was invoked */

/* Usage
	type = c_type ();
   Where:
	type is one of the following values:
		0 Unknown how we got here.
		1 C_CALL
		2 C_RESUME
		3 C_DETACH
		4 A called coroutine "fell off the end".
*/

int c_type () {
	return (c_wmi () -> cf_type_);
	}

/* WMI -- Find current coroutine's FCV (Who aM I?) */

/* Usage:
	fcv = c_wmi ();
   Where:
	fcv	is returned as a pointer to the current coroutine's
		FCV.
*/

C_FCV * c_wmi () {
	C_FCV **corwmip();
	return (*corwmip());
	}
_FCV *fcv; int passval;
    {
	fcv -> cf_caller_ = c_wmi() -> cf_caller_;
	fcv -> cf_type_ = CT_RESUME;
	