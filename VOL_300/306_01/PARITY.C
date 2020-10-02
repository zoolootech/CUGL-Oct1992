/*******************************************************************************
                                PARITY.C
                   COPYRIGHT 1988, 1989 GREGORY COLVIN.
                            ALL RIGHTS RESERVED.

Pattern recognition demonstration.  Learn parity of input.

Suggested input for XOR:

    show intermediate results? (y/n):
    n
    enter number of input neurons (<= 8):
    2
    bias input? (y/n):
    y
    enter number of hidden neurons (<= 8):
    3
    enter max iterations/pattern:
    1
    enter learning rate (<= 100):
    24
    momentum? (y/n):
    y
    enter amount to randomize synapses (<=100):
    20
    enter seed:
    1
    enter number of learning trials:
    200

*******************************************************************************/

#include <stdio.h>
#include "synapsys.h"

#define N_INP 8
#define N_HID 8
#define N_OUT 1

static NEURON
    Inputs[N_INP],
    Hidden[N_HID],
    Output;

static int Target;

static SYNAPSE Synapse1[N_INP*N_HID];
static SYNAPSE Synapse2[N_HID*N_OUT];
static SYNAPSE History1[N_INP*N_HID];
static SYNAPSE History2[N_HID*N_OUT];

static LAYER
    Layer1 = { 0,N_INP,Inputs,Synapse1,History1,0,0,N_HID,Hidden, 0 },
    Layer2 = { 0,N_HID,Hidden,Synapse2,History2,0,0,N_OUT,&Output,0 };

static NETWORK
    Network = { &Layer1, &Layer2 };

int parity( i )
int i;
{
    int m,n;
    for (n=0,m=0; i && m<8; m++,i>>=1)
        if (i&1)
            n++;
    return (n&1);
}

main()
{
    int verbose = 0, bias = 0;
    char pat, n_pat;
    int n_inp, n_out=N_OUT, n_hid;
    int i, h, rate, momentum, maxrand;
    long seed;
    SYNAPSE *w;
    int iter, maxiter, ntrials;
    long m, n, err, sse1, sse2;
    float oldsse, totsse1, totsse2, minsse1, minsse2;
    char s[80];

    for(;;)
    {

        printf( "show intermediate results? (y/n):\n" );
        gets(s);
        if (strpbrk(s,"yY")) verbose = 1;
        else verbose = 0;

        printf( "enter number of input neurons (<= %d):\n", N_INP );
        gets(s), sscanf( s, "%d", &n_inp );
        n_pat = 1 << n_inp;
        printf( "bias input? (y/n):\n" );
        gets(s);
        if (strpbrk(s,"yY")) n_inp++, bias = 1;
        else if (bias) n_inp--, bias = 0;

        printf( "enter number of hidden neurons (<= %d):\n", N_HID );
        gets(s), sscanf( s, "%d", &n_hid );

        printf( "enter max iterations/pattern:\n" );
        gets(s), sscanf( s, "%d", &maxiter );

        printf( "enter learning rate (<= 100):\n" );
        gets(s), sscanf( s, "%d", &rate );

        printf( "momentum? (y/n):\n" );
        gets(s);
        if (strpbrk(s,"nN")) momentum = 0;
        else momentum = 1;

        printf("enter amount to randomize synapses (<=100):\n");
        gets(s), sscanf( s, "%d", &maxrand );
        if (maxrand > 0)
        {   printf( "enter seed:\n");
            gets(s), sscanf( s,"%d", &seed );
        }

        printf( "enter number of learning trials:\n" );
        gets(s), sscanf( s, "%d", &ntrials );

        if (bias) printf( "%d inputs, bias, %d hidden", n_inp-1, n_hid );
        else printf( "%d inputs, no bias, %d hidden", n_inp, n_hid );
        printf( " maxrand: %d seed: %d\n", maxrand, seed );
        printf( "learning rate: %d momentum: %d maxiter: %d\n\n",
                        rate, momentum, maxiter );

        Network.first_layer->next_layer = Network.last_layer;
        Network.first_layer->n_outputs = n_hid;
        Network.first_layer->n_inputs = n_inp;
        Network.first_layer->rate = rate;
        Network.first_layer->momentum = momentum;
        Network.last_layer->prev_layer = Network.first_layer;
        Network.last_layer->n_inputs = n_hid;
        Network.last_layer->n_outputs = n_out;
        Network.last_layer->momentum = momentum;
        Network.last_layer->rate = rate;

        clear( &Network );

        randomize( &Network, maxrand, seed );

        for (oldsse=minsse1=minsse2=2000000, m=1, n=1; n <= ntrials; n++)
        {
            for (totsse1=totsse2=0, pat=0; pat < n_pat; pat++)
            {
                for (sse1=2000001, sse2=2000000, iter=0;
                     iter < maxiter /*&& sse1 > sse2*/;
                     iter++,m++)
                {
                    sse1 = sse2;

                    Target = parity(pat) ? 100 : -100;

                    if (verbose) printf( "\nINPUTS:\n" );
                    for (i=0; i < n_inp; i++)
                    {
                        if (bias && i+1 == n_inp)
                        {   Inputs[i].activation = 127;
                            if (verbose) printf( "%d",Inputs[i].activation );
                        }
                        else if (pat & (1 << i))
                        {   Inputs[i].activation = 100;
                            if (verbose) printf("%d ",Inputs[i].activation );
                        }
                        else
                        {   Inputs[i].activation = -100;
                            if (verbose) printf("%d ",Inputs[i].activation);
                        }
                    }
                    if (verbose) printf( "\n" );

                    feedforward( &Network );

                    if (verbose)
                    {
                        printf( "SYNAPSES:\n" );
                        for (w=Synapse1, h = 0; h < n_hid; h++)
                        {
                            for (i = 0; i < n_inp; i++,w++)
                                printf( "%5d ", (int)*w);
                            printf( "\n" );
                        }
                        printf( "HIDDEN:\n" );
                        for (h = 0; h < n_hid; h++)
                            printf("%5d ",Hidden[h].activation);
                        printf( "\nSYNAPSES:\n" );
                        for (w=Synapse2,h = 0; h < n_hid; h++,w++)
                            printf( "%5d ", (int)*w);
                        printf("\n");
                    }

                    err = Target - Output.activation;
                    Output.errors = err;
                    sse2 = err * err;
                        if (iter == 0) totsse1 += sse2;

                    if (verbose)
                    {
                        printf( "RESULTS:\n" );
                        printf(
                          "  pattern: %d output: %d target: %d error: %ld\n",
                          pat, Output.activation,  parity(pat)?100:-100, err );
                    }

                    feedback( &Network );

                    if (verbose && momentum)
                    {
                        printf( "MOMENTUM:\n" );
                        for (w=History1,h=0; h < n_hid; h++)
                        {
                            for (i = 0; i < n_inp; i++,w++)
                                printf( "%5d ", (int)*w);
                            printf( "\n" );
                        }
                        printf("\n");
                        for (w=History2,h=0; h < n_hid; h++,w++)
                            printf( "%5d ", (int)*w);
                        printf("\n\n");
                    }
                }
                totsse2 += sse2;
            }
            oldsse = totsse1;

            if (totsse2 < minsse2)
                minsse2 = totsse2;
            if (totsse1 < minsse1)
                minsse1 = totsse1;

            printf(
              "N: %5ld M: %5ld MSE1: %9.1f %9.1f MSE2: %9.1f %9.1f\n",
              n, m, totsse1/n_pat, minsse1/n_pat, totsse2/n_pat, minsse2/n_pat);
        }

        printf( "exit? (y/n):\n");
        gets(s); if (strpbrk(s,"yY"))
            break;
    }
    exit(0);
}
