cur_dn()
/* Move the down one row. This will wrap the cursor to the top of
   the screen if the cursor is on the last row
*/
{
        int cur_row, cur_col;
        get_cur(&cur_row,&cur_col);
        cur_row++;
        if(cur_row>24) cur_row=0;
        locate(cur_row,cur_col);
}
