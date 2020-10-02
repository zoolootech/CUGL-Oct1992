/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKRSC.C - CTask - Resource handling routines.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany
*/

#include "tsk.h"
#include "tsklocal.h"

/*
   create resource - initialise resource.
*/

resourceptr Globalfunc create_resource (resourceptr rsc TN(byteptr name))
{
#if (TSK_DYNAMIC)
   if (rsc == LNULL)
      {
      if ((rsc = tsk_palloc (sizeof (resource))) == LNULL)
         return LNULL;
      rsc->flags = F_TEMP;
      }
   else
      rsc->flags = 0;
#endif

   tsk_init_qhead (&rsc->waiting, TYP_RESOURCE);
   rsc->count = 0;
   rsc->owner = LNULL;

#if (TSK_NAMED)
   tsk_add_name (&rsc->name, name, TYP_RESOURCE, rsc);
#endif

   return rsc;
}

/*
   delete_resource - Kill all tasks waiting for the resource.
*/

void Globalfunc delete_resource (resourceptr rsc)
{
   CRITICAL;

   CHECK_EVTPTR (rsc, TYP_RESOURCE, "Delete Resource");

   C_ENTER;
   tsk_kill_queue (&(rsc->waiting));
   rsc->owner = LNULL;
   rsc->count = 1;
   C_LEAVE;

#if (TSK_NAMED)
   tsk_del_name (&rsc->name);
#endif

#if (TSK_DYNAMIC)
   if (rsc->flags & F_TEMP)
      tsk_pfree (rsc);
#endif
}


/*
   request_resource - Wait until resource is available. If the resource
                      is free on entry, it is assigned to the task, and
                      the task continues to run.
                      If the task requesting the resource already owns it,
                      this routine returns 0.
*/

int Globalfunc request_resource (resourceptr rsc, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (rsc, TYP_RESOURCE, "Request Resource");

   C_ENTER;
   if (!rsc->count || rsc->owner == GLOBDATA current_task)
      {
      rsc->count = 1;
      rsc->owner = GLOBDATA current_task;
      C_LEAVE;
      return 0;
      }
   GLOBDATA current_task->retptr = LNULL;
   tsk_wait (&rsc->waiting, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}


/*
   c_request_resource - If the resource is free on entry, it is assigned 
                        to the task, otherwise an error status is returned.
*/

int Globalfunc c_request_resource (resourceptr rsc)
{
   int rv;
   CRITICAL;

   CHECK_EVTPTR (rsc, TYP_RESOURCE, "Cond Request Resource");

   C_ENTER;
   if ((rv = (!rsc->count || rsc->owner == GLOBDATA current_task)) != 0)
      {
      rsc->count = 1;
      rsc->owner = GLOBDATA current_task;
      }
   C_LEAVE;
   return (rv) ? 0 : -1;
}


int Globalfunc request_cresource (resourceptr rsc, dword timeout)
{
   CRITICAL;

   CHECK_EVTPTR (rsc, TYP_RESOURCE, "Request CResource");

   C_ENTER;
   if (!rsc->count || rsc->owner == GLOBDATA current_task)
      {
      rsc->count++;
      rsc->owner = GLOBDATA current_task;
      C_LEAVE;
      return 0;
      }
   GLOBDATA current_task->retptr = LNULL;
   tsk_wait (&rsc->waiting, timeout);
   return (int)((dword)GLOBDATA current_task->retptr);
}

int Globalfunc c_request_cresource (resourceptr rsc)
{
   int rv;
   CRITICAL;

   CHECK_EVTPTR (rsc, TYP_RESOURCE, "Cond Request CResource");

   C_ENTER;
   if ((rv = (!rsc->count || rsc->owner == GLOBDATA current_task)) != 0)
      {
      rsc->count++;
      rsc->owner = GLOBDATA current_task;
      }
   C_LEAVE;
   return (rv) ? 0 : -1;
}


/*
   release_resource - Release the resource. If there are tasks waiting for
                      the resource, it is assigned to the first waiting
                      task, and the task is made eligible.
*/

void Globalfunc release_resource (resourceptr rsc)
{
   tcbptr curr;
   CRITICAL;

   CHECK_EVTPTR (rsc, TYP_RESOURCE, "Release Resource");

   if (rsc->owner != GLOBDATA current_task)
      return;

   C_ENTER;
   if (!--rsc->count)
      {
      if ((curr = (tcbptr)rsc->waiting.first)->cqueue.kind & Q_HEAD)
         {
         rsc->owner = LNULL;
         C_LEAVE;
         return;
         }

      rsc->count++;
      rsc->owner = curr;
      tsk_runable (curr);
      }
   C_LEAVE;
}


/*
   check_resource - returns 1 if the resource is available.
*/

int Globalfunc check_resource (resourceptr rsc)
{
   CHECK_EVTPTR (rsc, TYP_RESOURCE, "Check Resource");
   return rsc->count == 0;
}

