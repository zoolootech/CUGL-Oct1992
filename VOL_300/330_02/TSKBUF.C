/*
   --- Version 2.2 90-10-12 10:33 ---

   TSKBUF.C - CTask - Buffered Message handling routines.

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   NOTE: None of the Buffer routines my be used from inside an interrupt
         handler. The routines are relatively slow, since they use
         the normal word pipe and resource calls. Optimization would be 
         possible by using block moves and internally handling resources.
*/

#include "tsk.h"
#include "tsklocal.h"


bufferptr Globalfunc create_buffer (bufferptr buf, farptr pbuf, 
                                    word bufsize TN(byteptr name))
{
#if (TSK_DYNAMIC)
   if (buf == LNULL)
      {
      if ((buf = tsk_palloc (sizeof (buffer))) == LNULL)
         return LNULL;
      buf->flags = F_TEMP;
      }
   else
      buf->flags = 0;
   if (pbuf == LNULL)
      {
      if ((pbuf = tsk_palloc (bufsize)) == LNULL)
         {
         if (buf->flags & F_TEMP)
            tsk_pfree (buf);
         return LNULL;
         }
      buf->flags |= F_STTEMP;
      }
#endif

   create_wpipe (&buf->pip, pbuf, bufsize TN(name));
   create_resource (&buf->buf_read TN(name));
   create_resource (&buf->buf_write TN(name));
   buf->msgcnt = 0;

#if (TSK_NAMED)
   tsk_add_name (&buf->name, name, TYP_BUFFER, buf);
#endif

   return buf;
}


void Globalfunc delete_buffer (bufferptr buf)
{
   CHECK_EVTPTR (buf, TYP_RESOURCE, "Delete Buffer");

   delete_wpipe (&buf->pip);
   delete_resource (&buf->buf_read);
   delete_resource (&buf->buf_write);
   buf->msgcnt = 0;

#if (TSK_NAMED)
   tsk_del_name (&buf->name);
#endif

#if (TSK_DYNAMIC)
   if (buf->flags & F_STTEMP)
      tsk_pfree (buf->pip.wcontents);
   if (buf->flags & F_TEMP)
      tsk_pfree (buf);
#endif
}


int Globalfunc read_buffer (bufferptr buf, farptr msg, int size, dword timeout)
{
   int i, len, l1, l2;
   word w;

   CHECK_EVTPTR (buf, TYP_RESOURCE, "Read Buffer");

   if ((i = request_resource (&buf->buf_read, timeout)) < 0)
      return i;

   if ((len = read_wpipe (&buf->pip, timeout)) < 0)
      {
      release_resource (&buf->buf_read);
      return len;
      }

   l1 = (len < size) ? len : size;

   for (i = 0; i < l1; i++)
      {
      if (!(i & 1))
         w = read_wpipe (&buf->pip, 0L);
      else
         w = w >> 8;
      ((byteptr)msg) [i] = (byte)w;
      }

   l2 = (len + 1) >> 1;
   for (i = (l1 + 1) >> 1; i < l2; i++)
      read_wpipe (&buf->pip, 0L);
   if (l1 < size)
      ((byteptr)msg) [l1] = 0;

   buf->msgcnt--;

   release_resource (&buf->buf_read);
   return len;
}


int Globalfunc c_read_buffer (bufferptr buf, farptr msg, int size)
{
   int res;
   CRITICAL;

   CHECK_EVTPTR (buf, TYP_RESOURCE, "Cond Read Buffer");

   res = -1;
   C_ENTER;

   if (buf->pip.filled && !buf->buf_read.count)
      {
      request_resource (&buf->buf_read, 0L);
      C_LEAVE;
      res = read_buffer (buf, msg, size, 0L);
      }
   else
      C_LEAVE;

   return res;
}


local int Staticfunc tsk_wrbuf (bufferptr buf, word w, dword timeout)
{
   int i;

   if ((i = write_wpipe (&buf->pip, w, timeout)) < 0)
      release_resource (&buf->buf_write);
   return i;
}


int Globalfunc write_buffer (bufferptr buf, farptr msg, int size, dword timeout)
{
   int i, res, l2;

   CHECK_EVTPTR (buf, TYP_RESOURCE, "Write Buffer");

   if (size < 0 || (word)((size + 3) >> 1) > buf->pip.bufsize)
      return -3;

   if ((i = request_resource (&buf->buf_write, timeout)) < 0)
      return i;

   if ((i = tsk_wrbuf (buf, (word)size, timeout)) < 0)
      return i;

   l2 = (size + 1) >> 1;

   for (i = 0; i < l2; i++)
      if ((res = tsk_wrbuf (buf, ((wordptr)msg) [i], timeout)) < 0)
         return res;

   buf->msgcnt++;

   release_resource (&buf->buf_write);
   return size;
}


int Globalfunc c_write_buffer (bufferptr buf, farptr msg, int size)
{
   int res;
   CRITICAL;

   CHECK_EVTPTR (buf, TYP_RESOURCE, "Cond Write Buffer");

   if (size < 0 || (word)((size + 3) >> 1) > buf->pip.bufsize)
      return -3;

   C_ENTER;
   res = -1;

   if (wpipe_free (&buf->pip) >= (word)((size + 3) >> 1) && !buf->buf_write.count)
      {
      request_resource (&buf->buf_write, 0L);
      C_LEAVE;
      res = write_buffer (buf, msg, size, 0L);
      }
   else
      C_LEAVE;

   return res;
}


word Globalfunc check_buffer (bufferptr buf)
{
   CHECK_EVTPTR (buf, TYP_RESOURCE, "Check Buffer");
   return buf->msgcnt;
}

