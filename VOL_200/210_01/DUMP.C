    /*                     - DUMP.C, V 1.7 -
                    A memory HEX/ASCII Dump program
                         by K.G.Busch, Oct 86                       */

    #include "stdio"

    main()
    {
        int maddr, mchar, mptr;
        int b, c, q;
        int mask = 0Xfff0;
        void head(), border();

        do {
            printf("\nEnter the address in HEX to peek at: ");
            scanf("%x%*c",&maddr);
            mptr = maddr & mask;

            do {
                head();
                border();
                for (b=0; b<=15; b++) {
                    printf("\n%4X  ",mptr);
                    for (q=0; q<=15; q++) hlook(&mchar,&mptr,&q);
                    printf("   ");
                    for (q=0; q<=15; q++) alook(&mchar,&mptr,&q);
                    mptr += 16;
                }
                printf("\nENTER to continue otherwise exit");
                printf(" for another address: ");
            } while ((c = getchar()) == '\n');
        } while ((c = getchar()) == '\n');
    }

        /* ------- The head function for column labels ------- */

        void head()
            {
                int q;
                printf("\nAddr  ");
                for (q=0; q<=15; q++) {
                    printf("%2X ",q);
                }
            }

        /* -------- The double line border function -------- */

        void border()
            {
                int q;
                char bdr;
                bdr = '=';
                printf("\n");
                for (q=1; q<=53; q++) {
                    putchar(bdr);
                }
            }

        /* ------ The look functions for peek and print ------ */

        hlook(ptr1,ptr2,ptr3)           /* Point to mchar, mptr, and q */
            int *ptr1,*ptr2,*ptr3;
         {
            *ptr1 = peek(*ptr2 + *ptr3);
            printf("%2X ",*ptr1);
         }

        alook(ptr1,ptr2,ptr3)         /* Point to mchar, mptr, and q */
            int *ptr1,*ptr2,*ptr3;
        {
            *ptr1 = peek(*ptr2 + *ptr3);
            if (*ptr1 >= 32 && *ptr1 <= 126) printf("%c",*ptr1);
            else printf(".");
        }

    /* ------------ This is the end of the program ------------ */

int to mchar, mptr, and q */
            int *ptr1,*ptr2,*ptr3;
         {
            *ptr1 = peek(*ptr2 + *ptr3);
 