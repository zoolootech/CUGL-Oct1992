C [BIT78031.FOR of JUGPDS Vol.19]
Ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
C                                                                             c
C   An awarded program of a contest (Japan Student Association of Computers)  c
C      by kihira shuu?                                                        c
C                                               			      c	
C     transerred from BIT 1978-04(Vol.10,No.4), p86-87			      c	
C	 by Toshiya Ohta & Studio Gala, June 11, 1985			      c
C                                                                             c
C                                                                             c
Ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
C
      dimension m(4),a(4),m1(24),m2(24),m3(24),m4(24)
      data a/1h+,1h*,1h-,1h//

      m(1) = 1
      m(2) = 2
      m(3) = 6
      m(4) = 9
      n = 0
      do 10 i=1,4
          do 10 j=1,4
               do 10 k=1,4
                    do 10 l=1,4
                        if ((i-j)*(i-k)*(i-l)*(j-k)*(j-l)*(k-l)) 15,10,15
   15                    n = n+1
                         m1(n) = m(i)
                         m2(n) = m(j)
                         m3(n) = m(k)
                         m4(n) = m(l)
   10 continue
      do 20 n=1,24
          do 30 i=1,4
              if (i-2) 301,301,302
  301          if (m1(n)-m2(n)) 302,30,30
  302          call sub(i,mm,m1(n),m2(n),isw)
               if (isw) 30,303,30
  303          do 35 j=1,4
                   if (i-j) 353,351,353
  351              if (i-2) 352,352,353
  352              if (m2(n)-m3(n)) 353,35,35
  353              call sub(j,nn,mm,m3(n),isw)
                   if (isw) 35,355,35
  355              do 135 k=1,4
                        if (j-k) 358,356,358
  356                   if (j-2) 357,357,358
  357                   if (m3(n)-m4(n)) 358,135,135
  358                   call sub(k,kk,nn,m4(n),isw)
                        if (isw) 135,359,135
  359                   if ((kk-70)*(kk-90)) 360,360,135
  360                   write(1,600) kk,m1(n),a(i),m2(n),a(j),m3(n),
     +                               a(k),m4(n)
  600                   format(1h ,i2,3h=((,i2,a1,i2,1h),a1,i2,1h),
     +                         a1,i2)
  135               continue
   35          continue
               do 40 k=i,4
                   if (i-k) 403,401,403
  401              if (i-2) 402,402,403
  402              if (m1(n)-m3(n)) 403,40,40
  403              if (k-2) 404,404,405
  404              if (m3(n)-m4(n)) 405,40,40
  405              call sub(k,nn,m3(n),m4(n),isw)
                   if (isw) 40,410,40
  410              do 140 j=1,4
                        if (i-j) 412,411,412
  411                   if (i-2) 140,140,412
  412                   if (j-k) 414,413,414
  413                   if (j-2) 140,140,414
  414                   call sub(j,kk,mm,nn,isw)
                        if (isw) 140,415,140
  415                   if ((kk-70)*(kk-90)) 416,416,140
  416                   write(1,610) kk,m1(n),a(i),m2(n),a(j),m3(n),
     +                               a(k),m4(n)
  610                   format(1h ,i2,2h=(,i2,a1,i2,1h),a1,1h(,i2,
     +                         a1,i2,1h))
  140               continue
   40          continue
   30     continue
   20 continue
      stop
      end
C
C
      subroutine sub(ii,mc,ma,mb,isw)
      isw = 0
      goto (1,2,3,4), ii
    1     mc = ma+mb
      goto 7
    2     mc = ma*mb
      goto 7
    3     mc = ma-mb
      goto 7
    4     if (ma-ma/mb*mb) 5,6,5
    5          isw = 1
          goto 7
    6          mc = ma/mb
    7 return
      end
