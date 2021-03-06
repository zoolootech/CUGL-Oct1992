
main()
{
 char res;

 while (1)
  {
   heading();                   /* display heading */

   if (deviceoff())             /* check that devices are on */
     break;

   if (chkcable())              /* check cable connected */
     break;

   if (baudrate())              /* check baud rate */
     break;

   if (parity())                /* parity */
     break;

   if (databits())              /* check data bits */
     break;

   if (pc())                    /* handle PC involved case */
    {
     if (pcmale())              /* on serial not parallel port? */
       break;
 
     if (pcport())              /* COM1 or COM2 */
       break;

     if (bothpc())              /* two PCs case */
       break;

     if (pcterm())              /* PC to a terminal case */
       break;

     if (pcmodem())             /* PC to a modem case */
       break;

     if (pcprint())             /* PC to a printer */
       break;
   
     if (fullcable());          /* try full straight thru cable */
       break;
  
     devincomp();
     break;
  }
   else if (term())             /* no PC, terminal case */
    {
     if (pcmodem())
       break;

     if (decterm())             /* DEC terminal */
       break;

     if (bothterm())            /* both terminals */
       break;

     if (fullcable())           /* try 25 lead straight thru cable */
       break;

     if (dcedte())
       break;

     devincomp();
     break;
    }

   else
    {
     if (dcedte())
       break;

     devincomp();
     break;
    }
 }

 blank();
 blank();
 printf("Glad I could be of service to you.\n");
}

/* heading - displays header for program */
heading()
{
 clear();
 printf("RS-232 MINI EXPERT SYSTEM.\n");
 printf("Version 1.0   by   Joe Kilar \n");
 blank();
 printf("This program will help you solve RS-232 serial communications ");
 printf("problems. \n");
 printf("Examples are connecting PCs to serial printers, modems, or"); 
 printf(" terminals. \n");
 blank();
 printf("If you're not sure about an answer you may try skipping it by \n");
 printf("answering no to whether the devices are communicating.  Jot down \n");
 printf("the question in case further aids don't result in success. \n");
 wait();
 return;
}


deviceoff()
{
 char r;

 clear();
 printf("Check that power to both devices is on.  If power is not \n");
 printf("on, turn it on. \n");
 r = respny();
 return(r);
}


chkcable()
{
 char r;

 clear();
 printf("Check that the serial cable is firmly connected to both devices.\n");
 printf("If not, ensure a good connection. \n");
 r = respny();
 return(r);
}

baudrate()
{
 char r;

 clear();
 printf("Check that the baud rates selected or required for both devices \n");
 printf("are the same.   Baud rate reflects the speed at which data is \n");
 printf("communicated.  Typical baud rates are 300,1200,2400,9600, and "); 
 printf("19.2K\n");
 blank();
 printf("Some devices have auto-baud.  They lock onto the baud rate of\n");
 printf("the other device sending successive RETURNs (sometimes SPACEs). "); 
 printf("At times \n");
 printf("noise may make it lock onto the wrong baud rate.  Try turning it \n");
 printf("off and then back on to reset it.\n");
 blank();
 printf("Ensure that the baud rates are identical and/or reset any \n");
 printf("device using auto-baud. \n");
 r = respny();
 return(r);
}

parity()
{
 char r;

 clear();
 printf("Check that the parity's selected or required for both devices \n");
 printf("are the same.   Parity has to do with an extra bit sent that can \n");
 printf("help detect transmission errors.  It is usually set to even, odd \n");
 printf("or none. \n");
 blank();
 printf("Ensure that the parity's are identical. \n");
 r = respny();
 return(r);
}

databits()
{
 char r;

 clear();
 printf("Check that each device is set for the same number of data bits. \n");
 printf("Usual values are  7  or  8.\n");
 blank();
 printf("Ensure that the data bits selected are identical. \n");
 r = respny();
 return(r);
}
 
pc()
{
 char r;

 clear();
 printf("Is one or both devices an IBM or compatible PC? \n");
 r = resp("No","Yes");
 return(r);
}

pcmale()
{
 char r;

 clear();
 printf("Check that the cable is connected to the PC's serial and \n");
 printf("not parallel port.\n");
 printf("The serial port has male pins protruding from it, the parallel\n"); 
 printf("port is female. \n");
 blank();
 printf("Make sure the cable is connected to the serial port. \n");
 r = respny();
 return(r);
}

pcport()
{
 char r;

 clear();
 printf("If you have more than one serial port or have a PCjr, make sure\n");
 printf("you are connected to the correct serial port.\n");
 blank();
 printf("The port that comes on an XT, usually closest to the side with\n");
 printf("the power switch, is COM1.  The serial port on back of a PCjr \n");
 printf("may look like COM1 or COM2 depending on the way the software \n");
 printf("accesses it.  Try switching to the other port if you \n");
 printf("are in doubt. \n");
 blank();
 printf("Double check which port is used, switch the cable, or tell the\n");
 printf("program to use the other if there is another or you have a PCjr. \n");
 r = respny();
 return(r);
}

bothpc()
{
 char r;

 clear();
 printf("Are both devices IBM or compatible PCs? \n");
 r = resp("No","Yes");
 if (r)
   nullmodem();
 return(r);
}

pcterm()
{
 char r;

 clear();
 printf("Is the other device a terminal? \n");
 r = resp("No","Yes");
 if (r)
   nullmodem();
 return(r);
}

pcmodem()
{
 char r;
 
 clear();
 printf("Is the other device a modem? \n");
 r = resp("No","Yes");
 if (r)
   fullcable();
 return(r);
}

pcprint()
{
 char r;

 clear();
 printf("Is the other device a printer? \n");
 r = resp("No","Yes");
 if (r)
  {
   clear();
   blank();
   printf("Make sure the printer is an RS-232 device and not a Centronics \n");
   printf("parallel device.  If it is Centronics, you need to attach it \n");
   printf("to the parallel port of the PC. \n");
   wait();
  }
 return(r);
}

fullcable()
{
 char r;

 clear();
 blank();
 printf("Try a straight thru cable that uses all 25 pins.  Make sure the \n");
 printf("cable is not a null modem cable. \n");
 r = respny();
 return(r);
}


nullmodem()
{
 clear();
 printf("You need a null modem cable.  You can purchase one from a \n");
 printf("computer store.  You can also make one yourself by wiring \n");
 printf("up a cable as shown below. \n");
 blank();
 printf("     2 ------------------------------------------------ 3 \n");
 printf("     3 ------------------------------------------------ 2 \n");
 printf("     4 ------------------------------------------------ 5 \n");
 printf("     5 ------------------------------------------------ 4 \n");
 printf("     6 ------------------------------------------------ 20\n");
 printf("     7 ------------------------------------------------ 7 \n");
 printf("    20 ------------------------------------------------ 6 \n");
 blank();
 printf("If the null modem cable doesn't work, see someone in Digital \n");
 printf("Electronics for help.\n");
 wait();
 return;
}

term()
{
 char r;

 clear();
 printf("Is one or both devices a terminal? \n");
 r = resp("No","Yes");
 return(r);
}

decterm()
{
 char r;

 clear();
 printf("If the terminal is a DEC model such as VT100 or VT220 \n");
 printf("and other device is not a DEC computer or modem to a DEC \n");
 printf("computer (e.g. VAX), turn off the XON/XOFF feature. \n");
 r = respny();
 return(r);
}

bothterm()
{
 char r;

 clear();
 printf("Are both devices terminals? \n");
 r = resp("No","Yes");
 if (r)
   nullmodem();
 return(r);
}

dcedte()
{
 char r;

 clear();
 printf("RS-232 devices fall into two classes - DTE (Data Terminal Equip.)\n");
 printf("and DCE (Data Communications Equip.).  Try a full 25 pin straight\n");
 printf("through cable if one device is DTE and the other DCE.  If both \n");
 printf("devices are DTEs or DCEs, you need a null modem cable. The device\n");
 printf("manual may indicate the device's type.\n");
 blank();
 printf("Devices that are usually DTEs: terminals and IBM compatible PCs.\n");
 printf("Devices that are usually DCEs: modems, ports to minicomputers,\n");
 printf("   printers, VME bus serial ports, and devices designed to talk \n");
 printf("   with terminals such as serial ports on micro development \n");
 printf("   systems. \n");
 blank();
 printf("Does it seem that you need a null modem cable as the devices are\n");
 printf("both DTEs or DCEs? \n");
 r = resp("No","Yes");
 if (r)
   nullmodem();
 clear();
 r = respny();
 return(r);
}


devincomp()
{
 clear();
 printf("The devices are probably not supplying all the signals required \n");
 printf("by the other.  See someone in Digital Electronics for help. \n");
 wait();
 return;
}


respny()
{
 char r;

 blank();
 printf("    Are the devices communicating properly now? \n");
 r = resp("No","Yes");
 return(r);
}


resp(zero,one)
char *zero,*one;
{
 char r;

 blank();
 blank();
 printf("            0  - %s \n",zero);
 blank();
 printf("            1 -  %s \n",one);

tryagain:
 scr_rowcol(22,10);
 printf("Strike either the  0  or  1  key for appropriate response \n");
 r = getchar();
 if (r == '0')
   r = 0;
 else if (r == '1')
   r = 1;
 else
   goto tryagain;
 return(r);
}



wait()
{
 scr_rowcol(22,10);
 printf("Strike any key to continue \n");
 getchar();
 return;
}


clear()
{
 scr_clr();
 scr_rowcol(1,1);
 return;
}

blank()
{
 printf(" \n");
 return;
}

