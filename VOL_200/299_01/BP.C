/****************************************************************************/
/*  file name: bp.c                                                         */
/*  (c) by Ronald Michaels. This program may be freely copied, modified,    */
/*  transmitted, or used for any non-commercial purpose.                    */
/*  this is the main file of the back propagation program                   */
/*  This program compiles under the Zortech c compiler v. 1.07 using their  */
/*  graphics library or under Ecosoft v4.07 (set GRAPH 0)                   */
/****************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"error.h"
#include"random.h"

#define GRAPH 0  /* GRAPH 1 if it is desired to link in the graphics */
					  /* GRAPH 0 if no graph is desired */ 

#if GRAPH==1
#include"plot.h"
#endif

#ifdef ECO
#include<malloc.h>             /*  required for eco-c compiler  */
#endif

#define U(x) (unsigned int)(x)    /*  type conversion */
#define SQ(x) ((x)*(x))           /*  square macro  */

/* function prototypes */
void getdata         (FILE *bp1,FILE *bp2);
void getpattern      (FILE *bp1,int,int,double *);
void allocate_memory (void);
void init_weights    (int,int,double *);
void learn           (int);
void foreward        (int,int,double *,double *,double *);
void recognise       (void);
void calc_delta_o    (int,int,double *,double *,double *);
void calc_delta_h    (int,int,double *,double *,double *,double *);
void calc_descent    (int,int,double,double,double *,double *,double *);
void correct_weight  (int,int,double *,double *);
double activate      (double);
double pattern_error (int,int,double *,double *);
void print_scale     (void);
void get_seed(void);
void get_limits(void);

void dump            (int);   /* function to dump intermediate results */

/* external variable declarations  */
double *input;       /* pointer to input matrix */
double *output;      /* pointer to output unit output vector */
double *target;      /* pointer to target matrix */
double *weight_h;    /* pointer to weight matrix to hidden units */
double *weight_o;    /* pointer to weight matrix to output units */
double *hidden;      /* pointer to hidden unit output vector */
double *delta_o;     /* pointer to output unit delta vector */
double *delta_h;     /* pointer to hidden unit delta vector */
double *descent_h;   /* pointer to weight change matrix for weights to
                            hidden units */
double *descent_o;   /* pointer to weight change matrix for weights to
                               output units */

int n_pattern;       /* number of training patterns to be used */
int n_input;         /* number of input units in one pattern (dimensionality) */
int n_hidden;        /* number of hidden units */
int n_output;        /* number of output units in one target (dimensionality) */

double learning_rate;       /* learning rate parameter */
double momentum;            /* proportion of previous weight change */

FILE *bp3;                  /*  pointer to output file bp3.dat */

/****************************************************************************/
int main()                  /* some compilers want main to be void */
{

   FILE *bp1;               /*  pointer to input file bp1.dat */
   FILE *bp2;               /*  pointer to input file bp2.dat */

   char buff[10];           /*  buffer to hold number of cycles  */

   int choice;              /* program control choice */
   int p;                   /* pattern counter  */
	int cycles;              /* number of cycles for learning algorithm */
	
   if((bp1=fopen("bp1.dat","r"))==NULL){  /* open data input file */
      error(0,FATAL);
   }
   if((bp2=fopen("bp2.dat","r"))==NULL){  /* open configuration file */
      error(1,FATAL);
   }
   if((bp3=fopen("bp3.dat","w"))==NULL){  /* open output file */
      error(2,FATAL);
   }

   /* get training pattern size from input file bp1.dat */
   getdata(bp1,bp2);

   /* allocate space for input vectors  */
   allocate_memory();

   /* load input patterns into memory */
   getpattern(bp1,n_pattern,n_input,input);

   /* load target patterns into memory */
   getpattern(bp1,n_pattern,n_output,target);

	get_seed();    /* seed random number generator */
	get_limits();  /* set range of random numbers */

   /* initialise weight matrices with random weights */
   init_weights(n_input,n_hidden,weight_h);
   init_weights(n_hidden,n_output,weight_o);

   /* enter program control loop */
   for(;;){

      printf("\nBack Propagation Generalised Delta Rule Learning Program\n");
      printf("          Learn\n          Recognise\n");
      printf("          Dump\n          Quit\n");
      printf("choice:");
      choice = getch();
      putchar(choice);

      switch(choice){
         case 'l':
         case 'L':
            printf("\nHow Many Cycles?\n");
				cycles=atoi(gets(buff));
				if(cycles<1)cycles=1;
            learn(cycles);
            break;
         case 'r':
         case 'R':
            recognise();
            break;
         case 'd':
         case 'D':
            for(p=0;p<n_pattern;p++)dump(p);
            printf("\nNetwork variables dumped into file bp3.dat");
            break;
         case 'q':
         case 'Q':
            exit(0);
         default:
            break;
      }
   }
   fclose(bp1);
   fclose(bp2);
   fclose(bp3);
}

/****************************************************************************/
/* getdata                                                                  */
/* this function gets data from the data file regarding the size and number */
/* of patterns and the configuration file                                   */
/****************************************************************************/

void getdata(
   FILE *bp1,              /*  pointer to input file bp1.dat */
   FILE *bp2               /*  pointer to input file bp2.dat */
)
{
   if(fscanf(bp1,"%d",&n_pattern)==EOF){  /* get the number */
      error(3,FATAL);             /* of pattern vectors */
   }
   if(fscanf(bp1,"%d",&n_input)==EOF){    /* get the dimensionality */
      error(3,FATAL);             /* of input vectors */
   }
   if(fscanf(bp1,"%d",&n_output)==EOF){   /* get the dimensionality */
      error(3,FATAL);             /* of target vectors */
   }
   if(fscanf(bp1,"%d",&n_hidden)==EOF){   /* get the number */
      error(3,FATAL);             /* of hidden units */
   }
   if(fscanf(bp2,"%lf",&learning_rate)==EOF){  /* get learning rate */
      error(4,FATAL);
   }
   if(fscanf(bp2,"%lf",&momentum)==EOF){  /* get learning momoentum */
      error(4,FATAL);
   }
}

/****************************************************************************/
/* allocate_memory                                                          */
/* this function allocates memory for the network                           */
/****************************************************************************/

void allocate_memory()
{
   /* allocate space for input vectors */
   if((input=(double *)calloc(U(n_pattern*n_input),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for target vectors  */
   if((target=(double *)calloc(U(n_pattern*n_output),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for output vectors  */
   if((output=(double *)calloc(U(n_pattern*n_output),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for hidden unit vector */
   if((hidden=(double *)calloc(U(n_hidden),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for hidden unit delta vector */
   if((delta_h=(double *)calloc(U(n_hidden),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for output unit delta vector */
   if((delta_o=(double *)calloc(U(n_output),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for hidden weights */
   if((weight_h=(double *)calloc(U((n_input+1)*n_hidden),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for output weights */
   if((weight_o=(double *)calloc(U((n_hidden+1)*n_output),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for weight changes to hidden weights */
   if((descent_h=(double *)calloc(U((n_input+1)*n_hidden),sizeof(double)))==NULL){
      error(6,FATAL);
   }
   /* allocate space for weight changes to output weights */
   if((descent_o=(double *)calloc(U((n_hidden+1)*n_output),sizeof(double)))==NULL){
      error(6,FATAL);
   }
}

/****************************************************************************/
/* getpattern                                                               */
/* this function loads values for patterns and targets into memory          */
/****************************************************************************/

void getpattern(
   FILE *data,             /* pointer to input data file */
   int n_pattern_vector,   /* number of patterns to be read */
   int n_units,            /* dimensionality of pattern */ 
   double *matrix          /* pointer to matrix to hold values */
)
{
   int p;
   int i;

   for(p=0;p<n_pattern_vector;p++){
      for(i=0;i<n_units;i++){
         if(fscanf(data,"%lf",matrix+(p*n_units+i))<=NULL){
            error(3,FATAL);
         }
      }
   }
}

/****************************************************************************/
/* init_weights                                                             */
/* this function initialises weight matrices with random numbers            */
/****************************************************************************/

void init_weights(
   int n_input_units,
   int n_output_units,
   double *weight
)
{
   int i;
   int j;

   for(j=0;j<n_output_units;j++){
      /* note additional weights for bias unit */
      *(weight+j*(n_input_units+1)+n_input_units) = d_rand();
      for(i=0;i<n_input_units;i++){
         *(weight+j*(n_input_units+1)+i) = d_rand();
      }
   }
}

/****************************************************************************/
/* learn                                                                    */
/* this function implements the generalised delta rule with back propagation*/
/****************************************************************************/

void learn(
   int n_cycle
)
{
   int learning_cycle;
   int p;

	#if GRAPH==1
	double x_scale;
	double y_scale;
	#endif

   /* dump(0);  */  /* for a complete history of the learning process */

	#if GRAPH==1
		init_graph();
	#else
	   print_scale();  /* prints scale to measure number of cycles */
	#endif
	
   for(learning_cycle=0;learning_cycle<n_cycle;learning_cycle++){
	
		#if GRAPH!=1
      if(learning_cycle%10==0)printf(".");
		#endif
		
      for(p=0;p<n_pattern;p++){

         /* compute hidden layer output */
         foreward(n_input,n_hidden,weight_h,input+p*n_input,hidden);

         /* compute output layer output */
         foreward(n_hidden,n_output,weight_o,hidden,output+p*n_output);

         /* calculate delta for output units */
         calc_delta_o(p,n_output,delta_o,target,output+p*n_output);

         /* calculate delta for hidden units */
         calc_delta_h(n_output,n_hidden,delta_o,delta_h,hidden,weight_o);

         /* calculate descent for output weights */
         calc_descent(n_hidden,n_output,learning_rate,momentum,
            descent_o,delta_o,hidden);

         /* calculate descent for hidden weights */
         calc_descent(n_input,n_hidden,learning_rate,momentum,
            descent_h,delta_h,(input+p*n_input));

         /* correct output weights */
         correct_weight(n_hidden,n_output,weight_o,descent_o);

         /* correct hidden weights */
         correct_weight(n_input,n_hidden,weight_h,descent_h);

         /* dump(p); */   /* for a complete history of the learning process */
			/* caution this slows program down and creates a large file */
      }
		#if GRAPH==1
		if(learning_cycle==0){
         set_scales(pattern_error(n_pattern,n_output,target,output),
            n_cycle,&x_scale,&y_scale);
      }
      if((learning_cycle+1)%10==0){
         point(pattern_error(n_pattern,n_output,target,output),
            learning_cycle+1,x_scale,y_scale);
		}
	#endif  
   }
  	#if GRAPH==1
   point(pattern_error(n_pattern,n_output,target,output),
   learning_cycle,x_scale,y_scale);
	close_graph();
	#endif
}

/****************************************************************************/
/* recognise                                                                */
/* this function presents the input patterns and compares the output        */
/* to the target patterns                                                   */
/****************************************************************************/
void recognise ()
{
   int p;
   int i;
   int k;

   for(p=0;p<n_pattern;p++){

      /* compute hidden layer output */
      foreward(n_input,n_hidden,weight_h,(input+p*n_input),hidden);

      /* compute output layer output */
      foreward(n_hidden,n_output,weight_o,hidden,output+p*n_output);

      /* print input pattern */
      printf("\ninput  ");
      for(i=0;i<n_input;i++){
         printf("%3.1f  ",*(input+(p*n_input+i)));
      }
      printf("\n");

      /* print output pattern */
      printf("output  ");
      for(k=0;k<n_output;k++){
         printf("%f  ",*(output+p*n_output+k));
      }
      printf("\n");

      /* print target pattern */
      printf("target  ");
      for(k=0;k<n_output;k++){
         printf("%f  ",*(target+(p*n_output+k)));
      }
      printf("\n");
   }

   /* print error */
   printf("RMS error = %f ",pattern_error(n_pattern,n_output,target,output));

   printf("          press any key to proceed\n");
   getch();
}

/****************************************************************************/
/* foreward                                                                 */
/* this function calculates the output of a unit given input and weight     */
/* the below diagram is meant to illustrate the operation of this function
     
          n_input_units       i is index

          0      1      2      3      4
unit_in-> o      o      o      o      bias  (set to 1.0)
          |      |      |      |      o     
          |      |      |      |      |     
          |0     |1     |2     |3     |4    
 weight-> X------X------X------X------X---->activate(sum)---->o 0 <- unit_out
          |      |      |      |      |     
          |5     |6     |7     |8     |9                       
          X------X------X------X------X---->activate(sum)---->o 1
          |      |      |      |      |     
          |10    |11    |12    |13    |14          
          X------X------X------X------X---->activate(sum)---->o 2
          |      |      |      |      |     
          |15    |16    |17    |18    |19   
          X------X------X------X------X---->activate(sum)---->o 3
                                                                n_output_units     
                                                                j is index 

     weight matrix is  n_output_units X (n_input_units+1)
*/
/****************************************************************************/

void foreward (
   int n_input_units,
   int n_output_units,
   double *weight,
   double *unit_in,
   double *unit_out
)
{
   int i;
   int j;
   double sum;

   for(j=0;j<n_output_units;j++){
      sum = 0.0;
      for(i=0;i<n_input_units;i++){
         sum = sum + (*(unit_in+i))*(*(weight+(j*(n_input_units+1)+i)));
         }
      sum = sum + (*(weight+(j*(n_input_units+1)+n_input_units)));
      *(unit_out+j) = activate(sum);
   }
}

/****************************************************************************/
/* activate                                                                 */
/* this function calculates the output of a unit using a linear             */
/* approximation to the sigmoid function                                    */
/****************************************************************************/
/*
double activate(
   double sum
)
{
   double activation;

   if(sum < -1.8) activation = 0.1;

   else if(sum > 1.8) activation = 0.9;

   else{
      activation = (sum+2.0)/4.0;
   }
   return activation;
}
*/
/****************************************************************************/
/* activate                                                                 */
/* this function calculates the output of a unit using a linear             */
/* approximation to the sigmoid function                                    */
/****************************************************************************/
/*
double activate(
   double sum
)
{
   double activation;

   if(sum < -1.3) activation = 0.1;

   else if(sum > 1.3) activation = 0.9;

   else if(sum < -1.0){
      activation = (2.0*sum+3.0)/4.0;
   }

   else if(sum > 1.0){
      activation = (2.0*sum+1.0)/4.0;
   }

   else{
      activation = (sum+2.0)/4.0;
   }
   return activation;
}
*/
/****************************************************************************/
/* activate                                                                 */
/* this function calculates the output of a unit using the sigmoid function */
/****************************************************************************/

double activate(
   double sum
)
{
   double activation;

   activation = 1.0/(1.0+exp(-sum));
   return activation;
}

/****************************************************************************/
/* activate                                                                 */
/* this function calculates the output of a unit using the step function    */
/* it does not seem to work with the delta rule                             */
/****************************************************************************/
/*
double activate(
   double sum
)
{
   double activation;

   if(sum>=0.0)activation = 0.9;
   else activation = 0.1;
   return activation;
}
*/
/****************************************************************************/
/* calc_delta_o                                                             */
/* this function calculates delta for output units                          */
/****************************************************************************/

void calc_delta_o(
   int p,					/* number of the pattern under consideration */
   int n_output_units,	/* number of output units (dimensionality) */
   double *delta,			/* pointer to delta matrix */
   double *unit_target,	/* pointer to target matrix */
   double *unit_out     /* pointer to output matrix */
)
{
   int j;
   double temp;

   for(j=0;j<n_output_units;j++){
       *(delta+j) = ((*(unit_target+(p*n_output_units)+j))-(*(unit_out+j)))*
         (*(unit_out+j))*(1-(*(unit_out+j)));
   }
}

/****************************************************************************/
/* calc_delta_h                                                             */
/* this function calculates delta for hidden units                          */
/****************************************************************************/

void calc_delta_h(
   int n_output_units,		/* number of output units */
   int n_hidden_units,		/* number of hidden units */
   double *delta_out,		/* delta for output units */
   double *delta_hid,		/* delta for hidden units */
   double *unit_hid,			/* pointer to hidden units */
   double *weight				/* pointer to weight matrix */
)
{
   int j;
   int k;
   double sum;

   for(j=0;j<n_hidden_units;j++){
      sum = 0.0;
      for(k=0;k<n_output_units;k++){
         sum = sum+(*(delta_out+k))*(*(weight+k*(n_hidden_units+1)+j));
      }
      *(delta_hid+j) = (*(unit_hid+j)) * (1-(*(unit_hid+j))) * sum;
   }
}

/****************************************************************************/
/* calc_descent                                                             */
/* this function calculates values for the descent matrix using the deltas  */
/****************************************************************************/

void calc_descent(
   int n_input_units,
   int n_output_units,
   double rate,
   double moment,
   double *descent,
   double *delta,
   double *unit_in
)
{
   int i;
   int j;

   for(j=0;j<n_output_units;j++){
      for(i=0;i<n_input_units;i++){
         *(descent+j*(n_input_units+1)+i) = rate*(*(delta+j))*(*(unit_in+i))+
            moment*(*(descent+j*(n_input_units+1)+i));
      }
   /*note additional descents for bias weight*/
   *(descent+j*(n_input_units+1)+n_input_units) = rate*(*(delta+j))+
      moment*(*(descent+j*(n_input_units+1)+n_input_units));
   }
}

/****************************************************************************/
/* correct_weight                                                           */
/* this function updates the weight matrix using the latest descent values  */
/****************************************************************************/

void correct_weight(
   int n_input_units,
   int n_output_units,
   double *weight,
   double *descent
)
{
   int i;
   int j;

   for(i=0;i<n_input_units+1;i++){ /*note additional descents for bias weight*/
      for(j=0;j<n_output_units;j++){
         *(weight+j*(n_input_units+1)+i) = *(weight+j*(n_input_units+1)+i) +
         *(descent+j*(n_input_units+1)+i);
      }
   }
}

/****************************************************************************/
/* pattern_error                                                            */
/* this function calculates error of outputs vs targets for output units    */
/* error is output to a data file                                           */
/****************************************************************************/

double pattern_error(
   int n_pattern_vectors,
   int n_output_units,
   double *unit_target,
   double *unit_out
)
{
   int p;
   int j;
   double temp;

   temp = 0.0;
   for (p=0;p<n_pattern_vectors;p++){
      for(j=0;j<n_output_units;j++){
         temp = temp + SQ((*(unit_target+(p*n_output_units)+j))-
            (*(unit_out+p*n_output_units+j)));
      }
   }
   return sqrt(temp);
}


/****************************************************************************/
/* print_scale                                                              */
/* this function prints a scale for displaying number of iterations of      */
/* learning cycle if graphics mode is not selected                          */
/****************************************************************************/

void print_scale()
{
   printf("\n        100       200       300       400");
   printf("       500       600       700      800");
   printf("---------+---------+---------+---------+");
   printf("---------+---------+---------+---------+");
}

/****************************************************************************/
/* dump                                                                     */
/* this function prints intermediate results to file bp3.dat                */
/* call dump() from function learn()                                        */
/****************************************************************************/

void dump(
   int p
)
{
   int i;
   int j;
   int k;

   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);
   if(fprintf(bp3,"input pattern no. %d\n",p)==NULL)
      error(5,WARN);
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);

   /* print input pattern */
   if(fprintf(bp3,"input pattern\n")==NULL)
      error(5,WARN);
   for(i=0;i<n_input;i++){
      if(fprintf(bp3,"%f  ",*(input+(p*n_input+i)))==NULL)
         error(5,WARN);
   }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);

   /* print hidden pattern */
   if(fprintf(bp3,"hidden pattern\n")==NULL)
      error(5,WARN);
   for(j=0;j<n_hidden;j++){
      if(fprintf(bp3,"%f  ",*(hidden+j))==NULL)
         error(5,WARN);
   }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);

   /* print deltas for hidden pattern */
   if(fprintf(bp3,"deltas for hidden pattern\n")==NULL)
      error(5,WARN);
   for(j=0;j<n_hidden;j++){
      if(fprintf(bp3,"%f  ",*(delta_h+j))==NULL)
         error(5,WARN);
   }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);

   /* print output pattern */
   if(fprintf(bp3,"output pattern\n")==NULL)
      error(5,WARN);
   for(k=0;k<n_output;k++){
      if(fprintf(bp3,"%f  ",*(output+p*n_output+k))==NULL)
         error(5,WARN);
   }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);

   /* print deltas for output pattern */
   if(fprintf(bp3,"deltas for output pattern\n")==NULL)
      error(5,WARN);
   for(k=0;k<n_output;k++){
      if(fprintf(bp3,"%f  ",*(delta_o+k))==NULL)
         error(5,WARN);
   }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);

   /* print target pattern */
   if(fprintf(bp3,"target pattern\n")==NULL)
      error(5,WARN);
   for(k=0;k<n_output;k++){
      if(fprintf(bp3,"%f  ",*(target+(p*n_output+k)))==NULL)
         error(5,WARN);
   }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);

   /* print weights to hidden units */
   if(fprintf(bp3,"weights to hidden units\n")==NULL)
      error(5,WARN);
   for(j=0;j<n_hidden;j++){
      for(i=0;i<n_input+1;i++){   /* note additional weights for bias unit */
         if(fprintf(bp3,"%f  ",*(weight_h+j*(n_input+1)+i))==NULL)
            error(5,WARN);
      }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);
   }

   /* print descents for weights to hidden units */
   if(fprintf(bp3,"descents to weights to hidden units\n")==NULL)
      error(5,WARN);
   for(j=0;j<n_hidden;j++){
      for(i=0;i<n_input+1;i++){   /* note additional descent for bias unit */
         if(fprintf(bp3,"%f  ",*(descent_h+j*(n_input+1)+i))==NULL)
            error(5,WARN);
      }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);
   }

   /* print weights to output units */
   if(fprintf(bp3,"weights to output units\n")==NULL)
      error(5,WARN);
   for(k=0;k<n_output;k++){
      for(j=0;j<n_hidden+1;j++){   /* note additional weights for bias unit */
         if(fprintf(bp3,"%f  ",*(weight_o+k*(n_hidden+1)+j))==NULL)
            error(5,WARN);
      }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);
   }

   /* print descents for weights to output units */
   if(fprintf(bp3,"descents for weights to output units\n")==NULL)
      error(5,WARN);
   for(k=0;k<n_output;k++){
      for(j=0;j<n_hidden+1;j++){  /* note additional descents for bias unit */
         if(fprintf(bp3,"%f  ",*(descent_o+k*(n_hidden+1)+j))==NULL)
            error(5,WARN);
      }
   if(fprintf(bp3,"\n")==NULL)
      error(5,WARN);
   }
}

/****************************************************************************/
/* get_seed                                                                 */
/* this function asks for a number and uses it to seed rand()               */
/****************************************************************************/

void get_seed()
{
   char buff[10];
   long int s;

   printf("\n\rBack Propagation Generalised Delta Rule Learning Program\n\r");
   printf("Enter seed for pseudorandom number generator.\n\rDefault = 1\n\rseed: ");
   s = atol(gets(buff));
   if(s<1) s = 1;
   s_seed(s);
   printf("\n\rseed = %ld",s);
}

/****************************************************************************/
/* get_limits                                                               */
/* this function gets limits for d_rand                                     */
/****************************************************************************/

void get_limits()
{
	double upper,lower;     /* limits for random number generator */

   char buff[10];

   printf("\n\rEnter range for weights.\n\r");
   printf("defaults:\n\r   upper limit = 1.0\n\r   lower limit = -1.0\n\r");
   printf("enter upper limit: ");
   upper = atof(gets(buff));
   printf("enter lower limit: ");
   lower = atof(gets(buff));
   if(lower>=upper){
      printf("upper limit must be greater than lower limit; ");
      printf("default selected\n\r");
      upper = 1.0;
      lower = -1.0;
   }
   printf("upper limit = %4.2f\n\rlower limit = %4.2f",upper,lower);
	s_limits(upper,lower);
}

