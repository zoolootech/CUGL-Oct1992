/******************************************************************************

                                  SYNAPSYS.H

                          NEURAL NETWORK SIMULATION

      COPYRIGHT 1988, 1989, 1990, 1991 GREGORY COLVIN, ALL RIGHTS RESERVED

******************************************************************************/

/* define WORD to be signed sixteen bit integer on most machines */
#define WORD short

/*
    A neuron is characterized by two functions: a transfer function and an
    error function.

    Each neuron can take inputs from many neurons and transfer the sum of the
    inputs to an output activation.  The activation of a neuron is given in
    units of 1/128, and must be constrained to vary within the range of
    -127/128 through 127/128.  The transfer function is linear from -102/128
    through 102/128.  Above and below those values a logarithmic function is
    used to constrain the resulting activation within limits.

    Each neuron can also sum the error signals from many output neurons and
    compute an error correction factor.  The error correction factor is
    proportional to the learning rate, which is given in units of 1/128,
    and to the gain of the synaptic layer.

*/
typedef struct
{ WORD activation;            /* activation of neuron for feedforward        */
  WORD errors;                /* sum of errors from feedback                 */
} NEURON;

#define ACTIVATION_SHIFT 7    /* units of 1/128: 2**-7 */
#define RATE_SHIFT       7    /* units of 1/128: 2**-7 */

/*
    Each synapse connects an input neuron to an output neuron, and consists of
    a modifiable weight.  The value at each synapse is constrained to vary from
    -32767 to 32767.  This value is in units of 1/1024.  Thus the effective
    weight varies from -31.999 to 31.999.
*/
typedef WORD SYNAPSE;
#define MAX_SYNAPSE 32767
#define SYNAPSE_SHIFT 10      /* units of 1/1024: 2**-10 */

/*
    A layer of synapses is modeled as an array of input neurons, a matrix of
    synapses, and an array of output neurons.  Each input neuron has a weighted
    synapse on every output neuron.

    In feedforward mode the activations of the input neurons are fed through
    the synapses to the output neurons.

    In feedback mode the error signals applied to the output neurons are used
    to modify the synaptic weights with a correction factor, which is also fed
    back as an error signal to the input neurons.

    In multi-layer networks the outputs of one layer serve as the inputs to the
    next layer, so that feedforward and feedback can be carried through the
    entire network one layer at a time.
*/
typedef struct layer
{ struct layer *prev_layer;   /* pointer to previous layer, if any           */
  int n_inputs;               /* number of input neurons                     */
  NEURON *inputs;             /* same address as outputs of previous layer   */
  SYNAPSE *synapses;          /* synapses[n_inputs][n_outputs]               */
  SYNAPSE *history;           /* previous values for use in learning         */
  char rate;                  /* learning rate, larger values learn faster   */
  char momentum;              /* learning momentum, true or false            */
  int n_outputs;              /* number of output neurons                    */
  NEURON *outputs;            /* same address as inputs of next layer        */
  struct layer *next_layer;   /* pointer to next layer, if any               */
} LAYER;

typedef struct
{ LAYER *first_layer;
  LAYER *last_layer;
} NETWORK;

NETWORK *new_network();

/*
        Good random number generator with period 4286449341.
        Result is unsigned WORD with uniform distribution in range 0 to 65535.
        Seeds must be unsigned WORD, with no side effects.
        Quite fast if seeds are in registers.
*/
#define U2RAND(seed1,seed2) \
(((seed1)*=65421,++(seed1))+((seed2)*=65521,++(seed2)))

