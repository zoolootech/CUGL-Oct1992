cur_up() 
/* Move the cursor one row up */
{
        int cur_row, cur_col;
        get_cur(&cur_row,&cur_col);
        cur_row--;
        if(cur_row<0) cur_row=24;
        locate(cur_row,cur_col);
}
