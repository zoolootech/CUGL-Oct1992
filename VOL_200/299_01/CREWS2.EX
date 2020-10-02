

Example 2. Example of Obtaining Data From a MEL Descriptor:


Descriptor:
    
pipe, length = 100 (ft), diameter = 6 (in);
    
Code fragment:
    
double pipe_length, diameter;
    
union meli_param_data data;   /* see Example 5. */
char units[MAX_STRING_LEN+1];
int array_len;
int unknown_flag;
    
meli();  /* reads descriptor */
    
meli_data("length", &data, units, &array_len,
	&unknown_flag);  /* gets pipe length */
pipe_length = data.real;  /* will equal 100 */

meli_data("diameter", &data, units, &array_len,
	&unknown_flag);  /* gets pipe diameter */
diameter = data.real;  /* will equal 6 */

/* note that units, array_len, and unknown_flag
   are not considered (used). */

