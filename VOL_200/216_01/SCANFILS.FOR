*----------------------------------------------------------------------*
*                                                                      *
*                             SCANFILS                                 *
*                             ~~~~~~~~                                 *
*   This routine checks the date/time of files in the RAM disk, and    *
*   copies back to floppy disk all files for which the date/time has   *
*   been changed in dBase2. A summary report of actions is output to   *
*   a text file.                                                       *
*                                                                      *
*----------------------------------------------------------------------*

      program scanfils
      implicit integer (a-z)
      character*24 cpyfmt
      character*8 olname(50),nuname(50),dename(50),udname(50),
     .            oldate(50),nudate(50),fname
      character*6 oltime(50),nutime(50)
      character*4 olextn(50),nuextn(50),deextn(50),udextn(50)
      character*2 tmpfmt(12),nurch
      character   tmpnam(8)
      data oldir,nudir,report,savefi /2,3,4,8/
      data uc,dc /0,0/
      data tmpfmt /'(''','co','py',' e',':''','A8',',1','H.','A4',
     .             ',''','b:',''')'/
      open (6,file='prn')
      open (oldir,file='e:old.dir')
      open (nudir,file='e:new.dir')
      open (report,file='e:report.txt',status='new')
      open (savefi,file='a:savefils.bat',status='new')

   10 format(A8,1XA4,10XA8,2XA6)
   20 format(' Number of files deleted: ',I2)
   30 format(5XA8,1H.A4)
   40 format(' Number of files added:   ',I2)
   50 format(' Number of files changed: ',I2)
   60 format('copy e:'A8,1H.A4,'b:')
   70 format(///)

* Get past the trash --------------------------------------------------

      read  (oldir,70)
      read  (nudir,70)

* Load in the old and new directories ----------------------------------

      do 100 k=1,50
      read  (oldir,10,end=110) olname(k),olextn(k),oldate(k),oltime(k)
      if (oltime(k) .eq. ' free ') go to 110
  100 continue
  110 lasol = k-1
      olname(k) = '        '
      olextn(k) = '    '
      oldate(k) = '        '
      oltime(k) = '      '
      do 120 k=1,50
      read  (nudir,10,end=130) nuname(k),nuextn(k),nudate(k),nutime(k)
      if (nutime(k) .eq. ' free ') go to 130
  120 continue
  130 lasnu = k-1
      nuname(k) = '        '
      nuextn(k) = '    '
      nudate(k) = '        '
      nutime(k) = '      '

* Match-up file names -------------------------------------------------

      do 250 o=1,lasol

        do 200 n=1,lasnu
          if (olname(o) .eq. nuname(n) .and. olextn(o) .eq. nuextn(n))
     .                                                         go to 210
  200   continue
        go to 240

* We have matchup! Check to see if date/time has been changed, ---------

  210   if (oldate(o) .eq. nudate(n) .and. oltime(o) .eq. nutime(n))
     .                                                         go to 220

* and, if so, save the filename and write its name to savefils.bat.

        uc = uc + 1
        udname(uc) = nuname(n)
        udextn(uc) = nuextn(n)

* Before writing to savefils, however, got to squeeze out spaces

      write (fname,'(A8)') nuname(n)
      read  (fname,'(8A1)') (tmpnam(j),j=1,8)
        do 211 k=1,8
        if (tmpnam(k) .eq. ' ') go to 213
  211   continue
  213 length = k-1
      write (nurch,'(''A'',I1)') length
      read  (nurch,'(A2)') tmpfmt(6)
      write (cpyfmt,'(12A2)') tmpfmt
      write (savefi,cpyfmt) nuname(n),nuextn(n)

* Pack nudir arrays ------------------------

  220   do 230 k=n,lasnu
        m = k+1
        nuname(k) = nuname(m)
        nuextn(k) = nuextn(m)
        nudate(k) = nudate(m)
        nutime(k) = nutime(m)
  230   continue

        lasnu = lasnu-1
        go to 250

* Save list of deleted files. ----------------

  240   dc = dc+1
        dename(dc) = olname(o)
        deextn(dc) = olextn(o)
  250 continue

* WRAP-UP --------------------------------------------------------------
* Check to see if any files were created in ram disk, and if so copy
* them to drive b:. Since the nuname array was packed after each
* match-up, it will now be empty unless a new file has been created.

      if (lasnu .eq. 0) go to 310

      do 300 k=1,lasnu
      write (fname,'(A8)') nuname(k)
      read  (fname,'(8A1)') (tmpnam(j),j=1,8)
        do 270 i=1,8
        if (tmpnam(i) .eq. ' ') go to 280
  270   continue
  280 length = i-1
      write (nurch,'(''A'',I1)') length
      read  (nurch,'(A2)') tmpfmt(6)
      write (cpyfmt,'(12A2)') tmpfmt
      write (savefi,cpyfmt) nuname(k),nuextn(k)
  300 continue

  310 write (report,20) dc
      do 320 k=1,dc
      write (report,30) dename(k),deextn(k)
  320 continue

      write (report,40) lasnu
      do 330 k=1,lasnu
      write (report,30) nuname(k), nuextn(k)
  330  continue

      write (report,50) uc
      do 340 k=1,uc
      write (report,30) udname(k),udextn(k)
  340 continue

      close (report)
      close (savefi)

      stop
      end
