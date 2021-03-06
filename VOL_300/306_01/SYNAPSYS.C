/******************************************************************************

                                  SYNAPSYS.C

                          NEURAL NETWORK SIMULATION

      COPYRIGHT 1988, 1989, 1990, 1991 GREGORY COLVIN, ALL RIGHTS RESERVED

    Under copyright law this code may be used for educational and experimental
    purposes according to the "fair use" doctrine. If you wish to use or modify
    this code for a commercial product you must obtain a license from me at:

         17890 CIMARRON CIRCLE, NATHROP, COLORADO 81236  (719)539-4942

    This code implements a very fast backpropagation type network by
    representing synapse layers as word arrays and implementing all
    operations with integer arithmetic. Some instability in learning may
    result from truncation error. If this is a problem you may want to try
    implementing momentum as a fraction rather that a boolean, or try adding
    rounding logic to the FEEDBACK_SHIFT and DELTA_SHIFT operations in error(),
    as in the SYNAPSE_SHIFT operation in transfer().

    This code compiles to excellent code on most 32 bit machines. On 16 bit
    machines it would be a good idea to code the transfer() and error()
    functions in Assembly; the transfer() function will typically run twice
    as fast, and the error() function will typically run ten times as
    fast, when coded in Assembly on 16 bit machines. This is because most
    16 bit machines will multiply two 16 bit quantities to produce a 32 bit
    result, and all multiply operands in this program are designed not to
    exceed 16 bits. However, most compilers cannot find this optimization.

    This code compiles cleanly on most K&R compilers. ANSI compilers may
    complain about missing prototypes but should generate good code.

*****************************************************************************/

#include <stdio.h>
#include "synapsys.h"

/* create a new network of n_layer synapse layers with
     n_neuron[i] input neurons and
     n_neuron[i+1] output neurons
   for each layer of synapses

   the learning rate for each layer is set to rate[i]
   the presence of history synapses for each layer is controlled by momentum[i]
   return pointer to network, or 0 if out of memory
*/
NETWORK *new_network( n_layer, n_neuron, rate, momentum )
int n_layer, n_neuron[], rate[], momentum[];
{
  NETWORK *network;
  int i;
  LAYER *layer, *prev_layer=0;
  char *calloc();

  network = (NETWORK *)calloc( 1, sizeof(NETWORK) );
  if (!network)
    return 0;
  for (i = 0; i < n_layer; i++, prev_layer = layer)
  {
    layer = (LAYER *)calloc( 1, sizeof(LAYER) );
    if (!layer)
      return 0;
    layer->n_inputs = n_neuron[i];
    layer->n_outputs = n_neuron[i+1];
    layer->rate = rate[i];
    layer->momentum = momentum[i];
    layer->inputs = (NEURON *)calloc( layer->n_inputs, sizeof(NEURON) );
    if (!layer->inputs)
      return 0;
    if (prev_layer)
    {
      layer->prev_layer = prev_layer;
      layer->prev_layer->next_layer = layer;
      layer->prev_layer->outputs = layer->inputs;
    }
    else
      network->first_layer = layer;
    layer->synapses =
      (SYNAPSE *)calloc( layer->n_inputs*layer->n_outputs, sizeof(SYNAPSE) );
    if (!layer->synapses)
      return 0;
    if (momentum[i])
    {
      layer->history =
        (SYNAPSE *)calloc( layer->n_inputs*layer->n_outputs, sizeof(SYNAPSE) );
      if (!layer->history)
        return 0;
    }
    else
      layer->history = 0;
  }
  layer->outputs = (NEURON *)calloc( layer->n_outputs, sizeof(NEURON) );
  if (!layer->outputs)
    return 0;
  network->last_layer = layer;
  return network;
}

/* feed activations forward through all layers of a network */
feedforward( network )
NETWORK *network;
{
  int n_inputs;
  SYNAPSE *synapses;
  NEURON *inputs, *output, *end_out;
  LAYER *layer;

  /* loop forward through all layers */
  for (layer = network->first_layer; layer; layer = layer->next_layer)
  {
    synapses = layer->synapses;
    n_inputs = layer->n_inputs;
    inputs   = layer->inputs;
    output   = layer->outputs;
    end_out  = layer->n_outputs + output;

    /* feed activations forward through this layer */
    for ( ; output < end_out; output++, synapses += n_inputs)
      transfer( n_inputs, inputs, output, synapses );
  }
}


/* feed errors back through all layers of a network */
feedback( network )
NETWORK *network;
{
  int n_inputs, rate, momentum;
  SYNAPSE *synapses, *history;
  NEURON *inputs, *output, *end;
  LAYER *layer;

  /* loop back through all layers */
  for (layer = network->last_layer; layer; layer = layer->prev_layer)
  {
    /* clear out previous errors */
    n_inputs = layer->n_inputs;
    inputs   = layer->inputs;
    while (--n_inputs >= 0)
      (inputs++)->errors = 0;

    /* feed errors back through this layer */
    n_inputs = layer->n_inputs;
    inputs   = layer->inputs;
    output   = layer->outputs;
    synapses = layer->synapses;
    history  = layer->history;
    rate     = layer->rate;
    momentum = layer->momentum;
    end = output + layer->n_outputs;
    for ( ; output < end; output++, synapses += n_inputs, history += n_inputs)
      error( n_inputs, inputs, output, synapses, history, rate, momentum );
  }
}

/* transfer function */
transfer( n_inputs, inputs, output, synapses )
register int      n_inputs;                  /* number of input neurons      */
register NEURON  *inputs;                    /* vector of input neurons      */
         NEURON  *output;                    /* output neuron                */
register SYNAPSE *synapses;                  /* vector of synapses on output */
{
  register long sum = 0;                     /* for accumulating inputs      */
  long i;                                    /* for intermediate calculation */

  /* feed input activation forward through synapses by accumulating products */
  while (--n_inputs >= 0)
    sum += (long)*synapses++ * (inputs++)->activation;

  /* limit activation overload with log if below -100 or above 100) */
  if (sum > 0)
  {
    sum += 1 << SYNAPSE_SHIFT-1;             /* round sum                    */
    sum >>= SYNAPSE_SHIFT;                   /* shift sum back into range    */
    if (sum > 100)                           /* sum = 100 + log2(sum-100)    */
      for (i = sum, sum = 100; (i >>= 1) >= 100; sum++)
        ;
  }
  else if (sum < 0)
  {
    sum -= 1 << SYNAPSE_SHIFT-1;             /* round sum                    */
    sum >>= SYNAPSE_SHIFT;                   /* shift sum back into range    */
    if (sum < -100)                          /* sum = -100 - log2(-sum-100)  */
      for (i = -sum, sum = -100; (i >>= 1) >= 100; sum--)
        ;
  }
  output->activation = sum;
}

/* error function */
error( n_inputs, inputs, output, synapses, history, rate, momentum )
register int       n_inputs;        /* number of input neurons               */
register NEURON   *inputs;          /* vector of input neurons               */
         NEURON   *output;          /* output neuron                         */
register SYNAPSE  *synapses;        /* vector of synapses on output          */
         SYNAPSE  *history;         /* vector of synapse history             */
int                rate;            /* transfer learning rate                */
int                momentum;        /* if true use synapse history           */
{
  WORD error;                       /* correction error for synapse          */
  long weight;                      /* synapse weight                        */
  long feedback;                    /* amount to feedback to previous layer  */
  long delta;                       /* amount to change synapse weight       */

  /* amount of left shift to bring feedback and delta back into proper range */
#define FEEDBACK_SHIFT \
        SYNAPSE_SHIFT+ACTIVATION_SHIFT+RATE_SHIFT-ACTIVATION_SHIFT
#define DELTA_SHIFT \
        ACTIVATION_SHIFT+ACTIVATION_SHIFT+RATE_SHIFT-SYNAPSE_SHIFT

  /* get error, factor in derivative of log limit function if overload */
  error = output->errors;
  if (output->activation > 100)
    error = (error * 100) / output->activation;
  else if (output->activation < -100)
    error = (error * 100) / output->activation;
  error *= rate;                    /* error proportional to learning rate   */

  while (--n_inputs >= 0)           /* calculate new synapse weights:        */
  {
    weight = *synapses;             /*   get weight from synapse             */
    feedback = weight;              /*   feedback proportional to weight     */
    feedback *= error;              /*   feedback proportional to error      */
    feedback >>= FEEDBACK_SHIFT;    /*   shift feedback into range of errors */
    inputs->errors += feedback;     /*   feedback to input errors            */
    delta = inputs->activation;     /*   delta proportional to input         */
    inputs++;                       /*   next input                          */
    delta *= error;                 /*   delta proportional to error         */
    delta >>= DELTA_SHIFT;          /*   shift delta into range of weight    */
    if (momentum)
    { delta += *history;            /*   add momentum to delta               */
      *history++ = (SYNAPSE)delta;  /*   save delta for next feedback cycle  */
    }
    weight += delta;                /*   synapse weight corrected by delta   */
    if (weight > MAX_SYNAPSE)
      weight = MAX_SYNAPSE;         /*   limit weight in case of overflow    */
    else if (weight < -MAX_SYNAPSE)
      weight = -MAX_SYNAPSE;        /*   limit weight in case of underflow   */
    *synapses++ = (SYNAPSE)weight;  /*   put new weight back in synapse      */
  }
}


/* set all synapse weights and history in a network to zero */
clear( network )
NETWORK *network;
{
  register SYNAPSE *s, *end;
  register LAYER *layer = network->first_layer;
  do
  {
    s = layer->synapses;
    if (s)
    { end = s + layer->n_inputs * layer->n_outputs;
      do *s++ = 0; while (s < end);
    }
    s = layer->history;
    if (s)
    { end = s + layer->n_inputs * layer->n_outputs;
      do *s++ = 0; while (s < end);
    }
  } while (layer = layer->next_layer);
}

/* set all synapse weights in a network to val, set history to zero */
set( network, val )
NETWORK *network;
int val;
{
  register SYNAPSE *s, *end;
  register LAYER *layer = network->first_layer;
  do
  {
    s = layer->synapses;
    if (s)
    { end = s + layer->n_inputs * layer->n_outputs;
      do *s++ = val; while (s < end);
    }
    s = layer->history;
    if (s)
    { end = s + layer->n_inputs * layer->n_outputs;
      do *s++ = 0; while (s < end);
    }
  } while (layer = layer->next_layer);
}

/* add a signed pseudo-random value to all weights in a network */
randomize( network, max, seed )
NETWORK *network;
unsigned max;
long seed;
{
  register unsigned WORD seed1 = seed, seed2 = seed>>16;
  WORD div = 32768/max;
  register SYNAPSE *weight, *end;
  LAYER *layer = network->first_layer;
  do
  {
    weight = layer->synapses;
    if (weight)
    {
      end = weight + layer->n_inputs * layer->n_outputs;
      do
        *weight++ += (WORD)((long)U2RAND(seed1,seed2)-32768)/div;

      while (weight < end);
    }
  } while (layer = layer->next_layer);
}
