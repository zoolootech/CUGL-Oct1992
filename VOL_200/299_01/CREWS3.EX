

Example 3. Example of Outputting a MEL descriptor:

Code Fragment:

double pipe_length = 100, diameter = 6;
    
union melo_param_data data;   /* see Example 5. */
char length_units[] = "ft";
char diameter_units[] = "in";
int array_len = 0;
int unknown_flag = 0;

melo_init("pipe");  /* initialize */

/* get data ready to output: */
data.real = pipe_length;
melo_data("length", &data, length_units, array_len, 
	unknown_flag);
data.real = diameter;
melo_data("diameter", &data, diameter_units, 
	array_len, unknown_flag);

melo();  /* translates data into string */

Descriptor:

pipe, 
	length = 100 (ft), 
	diameter = 6 (in);

