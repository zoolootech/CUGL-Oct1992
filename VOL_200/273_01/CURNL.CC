cur_nl() 
/* Move the cursor to the beginning of the next row  */
{
        int cur_row, cur_col;
        get_cur(&cur_row,&cur_col);
        cur_row++;
        if(cur_row>24) cur_row=0;
        locate(cur_row,0);
}
