#include <dos.h>
cur_rt()
/* Move the cursor one col to the right */
{
        int cur_row, cur_col;
        get_cur(&cur_row,&cur_col);
        cur_col++;
        if(cur_col>79) {
                cur_row++;
                cur_col=1;
                if(cur_row>24) cur_row=1;
        }
        locate(cur_row,cur_col);
}
