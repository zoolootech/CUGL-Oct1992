/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      sortest.c  v1.3  (c) 1990  Ken Harris                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      This software is made available on an AS-IS basis. Unrestricted     */
/*      use is granted provided that the copyright notice remains intact.   */
/*      The author makes no warranties expressed or implied.                */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>

main()
{
        char rec[10];
        int i;

        printf("sortest - Test of Callable Record Sort\n\n");
        printf("Sort-init...\n");
        sort_init(6, "aa1.4");

        printf("Release 100 records...\n");
        for (i=99; i>0; i--)
        {       sprintf(rec, "%04d", i);
                printf("%d\n", i);
                sort_release(rec);
        }

        printf("Sort-merge...\n");
        sort_merge();

        printf("Return 100 records...\n");
        while (sort_return(rec))
                printf("%4.4s\n",rec);
}
