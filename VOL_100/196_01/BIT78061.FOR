C [BIT7861.FOR of JUGPDS Vol.19]
Ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
C                                                                             c
C         four color probrem test                                             c
C                                                                             c
C         nano pico kyositu bit 78-06 (vol10-07) p85-87                       c
C                                                                             c
C         data entered by toshiya oota & studio gala 85-06-13                 c
C                                                                             c
C                                                                             c
Ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
C
      dimension jv(3,30),js(30),jn(30),kf(7,20),kn(20),kc(20)
C     js       .......... sign
C     jn       .......... colored faces
C     kn       .......... no of sides
C     nverti   .......... no of vertices
C     nface    .......... no of faces
C
C     read no of vertices & faces
      call open(5,'FORT05  DAT',0)
 1000 read(5,100) nverti,nface
  100 format(2i5)
      if (nverti.le.0) goto 99
      write(1,600) nverti,nface
  600 format(//1h ,'no . vertices=',i5,'  no. faces=',i5/)
      do 1 i=1,nface
          kn(i) = 0
    1 kc(i) = -1
      do 2 i=1,nverti
          js(i) = 0
          jn(i) = 0
C     initial input of data
          read(5,110) (jv(j,i),j=1,3)
  110     format(3i5)
          write(1,210) i,(jv(j,i),j=1,3)
  210     format(1h ,'vertice no=',i5,'  face no =',3i5)
C     preparation,setting vertices for each face
          do 5 j=1,3
               ind = jv(j,i)
               k = kn(ind) + 1
               kf(k,ind) = i
               kn(ind) = k
    5     continue
    2 continue
      write(1,290)
  290 format(/1h )
C output of vertices for each face
      do 8 i=1,nface
          k = kn(i)
          write(1,220) i,k,(kf(j,i),j=1,k)
  220     format(1h ,'face no=',i3,'   no of vertices=',i3,
     +           '   vertice no =',7i5)
    8 continue
C     test of sign on vertices
   10 do 20 i=1,nface
          ind = kn(i)
          ism = ind
          do 12 j=1,ind
               k = kf(j,i)
   12     ism = ism + js(k)
          if ((ism/3)*3.ne.ism) goto 50
   20 continue
C     getting suitable sign
      write(1,290)
      write(1,230) (i,i=1,nverti)
      write(1,230) (js(i),i=1,nverti)
  230 format(1h ,30i3)
C     coloring procedure
      ind = jv(1,1)
      kc(ind) = 0
      ind = jv(2,1)
      kc(ind) = 1
      ind = jv(3,1)
      kc(ind) = 2+js(1)
      do 30 i=1,nverti
          do 31 k=1,3
               ind = jv(k,i)
               if (kc(ind).ge.0) jn(i) = jn(i) + 1
   31     continue
   30 continue
C     search for uncolored face
   33 do 35 i=2,nverti
          if (jn(i).eq.2) goto 40
   35 continue
C     finish,output the coloring
      write(1,290)
      write(1,250) (i,i=1,nface)
      write(1,250) (kc(i),i=1,nface)
  250 format(1h ,20i3)
      write(6,290)
      					goto 1000
C     looking for the uncolored face
   40 do 41 k=1,3
          ind = jv(k,i)
          if (kc(ind).lt.0) goto 42
   41 continue
C     implementation of tait algorithm
   42 j = jrs(k)
      m1 = jv(j,i)
      m1 = kc(m1)
      j = jrs(j)
      m2 = jv(j,i)
      m2 = kc(m2)
      ism = nsw(m1,m2)
      j = jrs(ism)
      if (js(i).eq.1) j = jrs(j)
      kc(ind) = nsw(m1,j)
      k = kn(ind)
      do 45 i=1,k
          j = kf(i,ind)
          jn(j) = jn(j) + 1
   45 continue
      					goto 33
C     add 1 to the binary number js(i)
   50 ism = nverti-1
      do 55 k=1,ism
          if (js(k).eq.0) goto 57
   55 continue
      write(1,270)
  270 format(1h ,'IMPOSSIBLE!!!')
      					goto 1000
C     1 & 0 are interchangable. hence js(nface) may be fixed to 0.
   57 js(k) = 1
      if (k.eq.1) goto 10
      k = k - 1
      do 58 i=1,k
   58 js(i) = 0
      					goto 10
   99 write(1,290)
      stop
      end
C
      function nsw(m1,m2)
C     binary addition for two bits
      nsw=iabs(m1-m2)
      if(m1+m2.eq.3) nsw=3
      return
      end
C
      function jrs(m1)
C     add 1 in mod 3
      jrs=m1+1
      if(m1.eq.3) jrs=1
      return
      end
