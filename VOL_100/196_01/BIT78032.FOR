C {BIT78032.FOR of JUGPDS Vol.19]
Ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
C                                                                             c
C         Gardner Number Table and g52,g53,g105                     c
C                                                                              c
C         g(n) = 2[llog2(n)], 1 <= n <= 105                                        c
C                                                                             c
C         Nano-Pico Kyositu BIT 78-03 (Vol.10,No.04) p88-89                       c
C                             & 78-07 (Vol.10, No.8) p88                          c
C                                                                             c
C         transferred by Toshiya Ohta & Studio Gala 85-06-12                 c
C                                                                             c
C                                                                             c
Ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
C
      dimension it(105),imp(105),imq(105)
C sosuuhyo it o tsukuru
      do 1 i=1,105
          it(i) = 1
    1 continue
      do 2 i=2,105
          if (it(i).eq.0) goto 2
          j = 2*i
   25     if (j.gt.105) goto 2
          it(j) = 0
          j = j+i
    2 continue
C mondai 1 no kaitoo
      iprime = 2
      k1 = 2
   35 k2 = 2*k1-1
      if (k2.gt.105) k2 = 105
      do 4 j=k1,k2
          imp(j) = k1
    4 continue
      k1 = k2+1
      if (k1.le.105) goto 35
    3 continue
      write(1,200) iprime
  200 format(1h ,'Iprime =',i5)
      do 33 i=1,105
          write(1,201) i,imp(i)
  201     format(1h ,'I = ',i4,i10)
   33 continue
C mondai 2 no kaitoo
      do 5 iprime=3,105
          if (it(iprime).eq.0) goto 5
          do 50 i=iprime,105
               imq(i) = imp(i)
               ip = iprime
   51          if (ip.eq.i) imr = ip
               if (ip.gt.i) goto 52
                    iwork = i-ip
                    imr = ip*imp(iwork)
   52          if (imr.gt.imq(i)) imq(i) = imr
               ip = ip*iprime
   50          if(ip.le.i) goto 51
          do 60 i=iprime,105
               imp(i) = imq(i)
   60     continue
    5 continue
      write(1,100) imp(52),imp(53),imp(104),imp(105)
  100 format(1h ,4(i15,2x))
      stop
      end
