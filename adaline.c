/**
   adaline.c

   The functions implement the adaptive
   linear element - Adaline - and the alpha-LMS
   learning law.

   Contents:
   calculate_net
   calculate_output
   display_inputs
   display_weights
   get_straight_input_from_user
   get_straight_input_vectors
   get_target_from_user
   initialize_weights
   process_new_case
   train_the_adaline
   train_weights
*/

#include <stdio.h>
#include <stdlib.h>

/**
   long get_straight_input_from_user(x, N)

   This function gets long from the
   user via simple interaction using the screen
   and the keyboard.
*/
void get_straight_input_from_user(long x[], long N)
{
   char  string[80];
   int   a;
   int   i;
   long  s;

   for(i=0; i<N+1; i++)
       x[i] = -1;

   x[0] = 1;

   for(i=1; i<N+1; i++)
   {
      printf("\nEnter input %d >>", i);
      gets(string);
      a    = atoi(string);
      x[i] = a;
   }

}  /* ends get_straight_input_from_user */


/**
   long get_target_from_user()

   This function gets a long from the user
   via the screen and keyboard.
*/
long get_target_from_user()
{
   char  string[80];
   long  s;
   printf("\nEnter the target >>");
   gets(string);
   s = atoi(string);
   return(s);
}  /* ends get_target_from_user */


/**
   void display_weights(w, N)

   This function displays the weight vector
   on the screen.
*/
void display_weights(long w[], long N)
{
   int i;
   for(i=0; i<N+1; i++)
   {
      if(i%15 == 0) printf("\n  ");
      printf("%10ld", w[i]);
   }
}  /* ends display_weights */


/**
   void display_inputs(x, N)

   This function displays the input vector
   on the screen.
*/
void display_inputs(long x[], long N)
{
   int i;
   for(i=0; i<N+1; i++){
      if(i%15 == 0) printf("\n  ");
      printf("%6ld", x[i]);
   }
}  /* ends display_inputs */


/**
   long get_straight_input_vectors(inputs, x, N)

   This function gets an input vector from the user
   via interaction using the screen and the keyboard.
*/
long get_straight_input_vectors(FILE *inputs, long x[], long N)
{
   char  string[80];
   int   i, s;
   long  target;

   printf("\nEnter number of input vectors >>");
   gets(string);
   s = atoi(string);
   for(i=0; i<s; i++)
   {
      printf("\n\tInput vector %d", i);
      get_straight_input_from_user(x, N);
      display_inputs(x, N);
      target = get_target_from_user();
      x[N+1] = target;
      fwrite(x, (N+2)*sizeof(long), 1, inputs);
   }  /* ends loop over i */
   fclose(inputs);

   return s;
}  /* ends get_straight_input_vectors */


/*
   void initialize_weights(w, N)

   This function initializes the members of the
   weights vector w.  Set the to 0 10 20 30 0 ...
*/

void initialize_weights(long w[], long N)
{
   for(int i=0; i<N+1; i++)
   {
      w[i] = ((i+1)%4) * 10;
   }
}  /* ends initialize_weights */

/*
   long calculate_net(net, N, w, x)

   This function calculates the net
   net = vector product of x * w
*/
void calculate_net(long *net, long N, long w[], long x[])
{
   *net = 0;
   for(long i=0; i<N+1; i++)
   {
      *net = *net + w[i]*x[i];
   }

   /* return *net; */
}  /* ends calculate_net */


/**
   long calculate_output(net)

   This function set the output value
   based on the net.
   output = 1 if net >= 0
   output = 0 if net < 0
*/
long calculate_output(long net)
{
   long result = 1;
   if(net < 0)
       result = -1;
   return result;
}  /* ends calculate_output */


/*
   long train_weights(target, net, eta, w, x, N)

   This function adjusts the weights in the weight
   vector w.  It uses the alpha LMS algorithm.
*/
void train_weights(long target, long net, float eta, long w[], long x[], long N)
{
   long delta_w, i;
   for(i=0; i<N+1; i++)
   {
      delta_w = eta*x[i]*(target-net);
      w[i]    = w[i] + delta_w;
   }
}  /* ends train_weights */


/*
   long process_new_case(weights, x, w, N)

   This function process a case for a new input.
   It is the working mode for the neural network.
*/
void process_new_case(FILE *weights, long x[], long w[], long N)
{
   long net, output;

   fread(w, (N+1)*sizeof(long), 1, weights);
   fclose(weights);
   get_straight_input_from_user(x, N);
   display_inputs(x, N);
   display_weights(w, N);
   calculate_net(&net, N, w, x);
   output = calculate_output(net);
   printf("\nnet=%ld  output=%ld  ", net, output );

}  /* ends process_new_case */
/*
   long train_the_adaline(inputs, weights, x, w, N)

   This function trains an Adaline.  It loops around
   and around through all of the input vectors.  If
   the Adaline produces the wrong answer for a vector,
   this function calls the train_weights function which
   trains the weights for that Adaline.  If the Adaline
   produces the correct result for all of the input
   vectors, then training is completed.  If one of the
   vectors produces an incorrect result from the Adaline,
   then the training loop repeats for all of the vectors.
   If after too many attempts, the Adaline still produces
   an incorrect answer, then you quit.
*/
void train_adaline(FILE *inputs, FILE *weights, long x[], long w[], long N)
{
   char  string[80];
   int   i, s;
   int   adjusting = 1,
         counter   = 0;
   long  net, output, target;
   float eta = 0.5;

   initialize_weights(w, N);
   printf("\nEnter number of input vectors >>");
   gets(string);
   s = atoi(string);
   while(adjusting){
      counter++;
      adjusting = 0;
      fseek(inputs, 0L, SEEK_SET);
      for(i=0; i<s; i++){
         fread(x, (N+2)*sizeof(long), 1, inputs);
         display_inputs(x, N);
         display_weights(w, N);
         calculate_net(&net, N, w, x);
         output = calculate_output(net);
         target = x[N+1];
         printf("\nnet=%ld  output=%ld  target=%ld",
		        net, output, target);

         if(output != target){
            printf("\noutput does not equal target so train");
            train_weights(target, net, eta, w, x, N);
            display_weights(w, N);
            adjusting = 1;
         } /* ends if output != target */

      }  /* ends loop over i */
      if(counter > 20){
         printf("\n\nTOO MUCH TRAINING - quiting");
         adjusting = 0;
      }
   }  /* ends while adjusting */

   printf("\n\nwent through the training cycle %d times",counter);
   fclose(inputs);
   fwrite(w, (N+1)*sizeof(long), 1, weights);
   fclose(weights);
}  /* ends train_the_adaline */
