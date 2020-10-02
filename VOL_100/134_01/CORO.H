/* Definitions for the C functions for coroutine management */

/* Function control vector (C_FCV_) */

/* The unique identifier for every coroutine is a pointer to its
   function control vector (FCV).  This structure provides all the
   necessary information to link two coroutines. Its format is as
   follows:

	Offset	Symbol		Function
	======	======		========

fcv ---------->	cf_sp_		Coroutine stack pointer	----------------+
	  2	cf_passer_	Pointer to passer's FCV			|
	  4	cf_caller_	Pointer to caller's FCV			|
	  6	cf_type_	Type of last invocation (see below)	|
	  8	cf_stksiz_	Size of stack area			|
	 10	cf_info_	User supplied data.
	 12			Space for coroutine's stack		|
				  .					.
				  .					.
				  .					.
(TOS)	  0	cs_bc_		Saved BC register content <-------------+
	  2	cs_pc_		PC where coroutine will resume
	  4	cs_passee_	Target coroutine of last control transfer
	  6	cs_passval_	Value passed on last "call"/"detach"/"resume"/
					"return"
	  .			  .	
	  .			  .
	  .			  .
cf_stksiz_+6			Location of initial entry to coroutine
cf_stksiz_+8			Location of "c_start_" function that starts
				coroutine initially.

	The main coroutine is something of a special case; it uses the
main "C" stack.  Only the first eight bytes of its FCV are meaningful.
*/

#define C_FCV struct c_fcv_	/* Poor man's typedef */

struct c_fcv_ {
	char * cf_sp_;		/* Stack pointer */
	C_FCV * cf_passer_;	/* "Passer" FCV address */
	C_FCV * cf_caller_;	/* "Caller" FCV address */
	int cf_type_;		/* Type of last invocation */
	int    cf_stksiz_;	/* Size of stack area */
	int cf_info_;		/* User specified information */
	};

struct c_stktop_ {
	int cs_bc_;		/* Saved BC register content */
	char * cs_pc_;		/* PC to resume */
	C_FCV * cs_passee_;	/* Last coroutine invoked */
	int cs_passval_;	/* Value passed to it */
	};

struct c_initstk_ {
	int (* ci_initfn_);	/* Initial function to call */
	int (* ci_start_);	/* Entry point of coroutine starter */
	};



/*	Types of control transfer (cf_type_ field) */

#define CT_UNKNOWN	0	/* Unknown (never invoked via standard
				   mechanism) */
#define CT_CALL		1	/* C_CALL */
#define CT_RESUME	2	/* C_RESUME */
#define CT_DETACH	3	/* C_DETACH */
#define CT_RETURN	4	/* Standard C function return caused
				   implicit DETACH */
V struct c_fcv_	/* Poor man's typedef */

struct c_fcv_ {
	char * cf_sp_;		/* 