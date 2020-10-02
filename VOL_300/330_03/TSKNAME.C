/*
   --- Version 2.2 90-10-12 10:33 ---

   CTask Name searching routines

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   This file is new with version 2.1.
   The find_name and find_group_name functions were moved here from
   'tsksub.c', since they are not needed in the basic kernel.
   This will save some code in applications not using the routines.

   The add_name function to add a statically allocated structure to
   the name list has been added in version 2.1.
*/

#include "tsk.h"
#include "tsklocal.h"

#if (TSK_NAMED)

local int Staticfunc tsk_streq (byteptr n1, byteptr n2)
{
   while (*n1 && *n1 == *n2)
      {
      n1++;
      n2++;
      }
   return *n1 == *n2; 
}

/*
   find_name
   find_group_name
      find structure, given name and type. 
      If type is less than zero, the first name-element matching the
      name is returned.
      If type is greater or equal to zero, the first structure matching
      the name and type is returned.
*/

#if (GROUPS)
farptr Globalfunc find_group_name (gcbptr group, byteptr name, int kind)
{
   queptr curr;

   if (kind == TYP_GROUP && tsk_streq (name, group->namelist.name))
      return group;

   for (curr = group->namelist.list.first; !(curr->kind & Q_HEAD); curr = curr->next)
      if (kind < 0 || (int)curr->kind == kind)
         if (tsk_streq (name, ((nameptr)curr)->name))
            return (kind >= 0) ? ((nameptr)curr)->strucp : curr;

   return LNULL;
}
#endif


farptr Globalfunc find_name (byteptr name, int kind)
{
#if (GROUPS)
   farptr curr;
   gcbptr group;

   if (tsk_global == LNULL)
      if (!ctask_resident ())
         return LNULL;

   for (group = GLOBDATA current_task->group; 
        group != LNULL; group = group->home)
      if ((curr = find_group_name (group, name, kind)) != LNULL)
         return curr;
#else
   queptr curr;

   if (kind == TYP_GROUP)
      return LNULL;

   for (curr = GLOBDATA name_list.list.first; !(curr->kind & Q_HEAD);
        curr = curr->next)
      if (kind < 0 || (int)curr->kind == kind)
         if (tsk_streq (name, ((nameptr)curr)->name))
            return (kind >= 0) ? ((nameptr)curr)->strucp : curr;
#endif

   return LNULL;
}


void Globalfunc add_event_name (nameptr elem)
{
   CRITICAL;

   if (elem->name [0] && elem->list.first == LNULL)
      {
      C_ENTER;
#if (GROUPS)
      tsk_putqueue (&GLOBDATA current_task->group->namelist.list, (queptr)&elem->list);
#else
      tsk_putqueue (&GLOBDATA name_list.list, (queptr)&elem->list);
#endif
      C_LEAVE;
      }
}
#endif


