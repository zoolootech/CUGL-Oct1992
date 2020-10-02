cur_lf()
/* Move the cursor one col to the left */
{
        int cur_row, cur_col;
        get_cur(&cur_row,&cur_col);
        cur_col--;
        if(cur_col<0) {
                cur_row--;
                cur_col=79;
                if(cur_row<0) cur_row=24;
        }
        locate(cur_row,cur_col);
}
