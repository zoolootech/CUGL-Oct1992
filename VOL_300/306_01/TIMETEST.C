/******************************************************************************
                               TIMETEST.C
                   COPYRIGHT 1988, 1989 GREGORY COLVIN.
                            ALL RIGHTS RESERVED.

    Benchmark single layer neural network in learning arbitary vector.

    A good example:

        enter number of iterations:
        10

        enter number of inputs:
        20

        enter number of outputs
        20

        enter learning rate (0 to 128):
        25

        enter momentum (0 or 1):
        0

*******************************************************************************/

#include <stdio.h>
#include <time.h>
#include "synapsys.h"

main()
{
  char s[81];
  int i, j, err, n_neuron[2], rate, momentum;
  long n, n_inp, n_out, t1, t2;
  float tt, te, sse;
  NETWORK *network;
  NEURON *inputs, *outputs;

  /* prompt for network to test */
  printf ( "enter number of iterations:\n" );
  gets(s), n = atoi( s );
  printf( "enter number of inputs:\n" );
  gets(s), n_inp = atoi( s );
  n_neuron[0] = n_inp;
  printf( "enter number of outputs:\n" );
  gets(s), n_out = atoi( s );
  n_neuron[1] = n_out;
  printf( "enter learning rate (0 to 128):\n" );
  gets(s), rate = atoi( s );
  printf( "enter momentum (0 or 1):\n" );
  gets(s), momentum = atoi( s );

  /* create one layer network, randomize synapses */
  network = new_network( 1, n_neuron, &rate, &momentum  );
  if (!network)
    printf( "\nout of memory\n" ), exit(0);
  inputs = network->first_layer->inputs;
  outputs = network->last_layer->outputs;
  randomize( network, 127, 1L );

  /* initialize input */
  for (i=1; i<=n_inp; i++)
    inputs[i].activation = i*100/n_inp;

  /* time network */
  for (tt=te=i=0; i<n; i++)
  {
    /* calculate outputs */
    t1 = clock();
    feedforward( network );
    t2 = clock();
    tt += (float)(t2 - t1);

    /* calculate errors, report mean squared error */
    for (sse=0,j=0; j<n_out; j++, sse += err*err)
      err = outputs[j].errors = j*100/n_out - outputs[j].activation;
    printf( "iteration %d  mse %f\n", i, sse/n_out );

    /* correct synapse weights */
    t1 = clock();
    feedback( network );
    t2 = clock();
    te += (float)(t2 - t1);
  }

  /* report results */
  printf( "feedforward: %9.2f transfer()/sec\n",
          (n*n_inp*n_out)/(tt/CLOCKS_PER_SEC+.000001) );
  printf( "feedback:    %9.2f error()/sec\n",
          (n*n_inp*n_out)/(te/CLOCKS_PER_SEC+.000001) );
}
