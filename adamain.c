/**
    adamain.c

   This file contains the main calling
   routine for the Adaline (Adaptive
   Linear Element) program.

   Notes:

   the inputs array x has N+2 elements
     x[0] is always 1
     then there are 1 to N elements
     then the last element is the target
   the weights array w has N+1 elements
     w[0] is the bias
     then there are 1 to N elements
*/

#include <stdio.h>
#include <stdlib.h>
/* #include <malloc.h> */


int main(int argc, char *argv[])
{
   char  inputs_file[80], weights_file[80];
   FILE  *inputs, *weights;
   int   working_mode  = 0,
         training_mode = 0,
         input_mode    = 0,
         files_ok      = 1;
   long *w, *x, N;


   if(argc < 5)
   {
      printf("\n\nUsage: adaline inputs_file weights_file ");
      printf("size_of_vectors mode");
      printf("\n   where mode=t (training)");
      printf("\n   where mode=i (input data)");
      printf("\n   where mode=w (working)");
      exit(1);
   }

   strcpy(inputs_file, argv[1]);
   strcpy(weights_file, argv[2]);

   N = atoi(argv[3]);

   if(argv[4][0] == 't'  ||  argv[4][0] == 'T')
      training_mode = 1;
   if(argv[4][0] == 'i'  ||  argv[4][0] == 'I')
      input_mode = 1;
   if(argv[4][0] == 'w'  ||  argv[4][0] == 'W')
      working_mode = 1;


          /* the last element of the inputs vector will
             hold the target */
   x = (long *) malloc((N+2) * sizeof(long));
   w = (long *) malloc((N+1) * sizeof(long));




      /* INPUT   MODE */

   if(input_mode)
   {
      if( (inputs = fopen(inputs_file, "w+b")) == '\0')
      {
         printf("\n\nERROR - cannot open input vector file\n");
         exit(0);
      }
      else
         get_straight_input_vectors(inputs, x, N);
   }  /* ends if input_mode */




      /*  TRAINING   MODE */

   if(training_mode)
   {

      if( (inputs = fopen(inputs_file, "r+b")) == '\0')
      {
         printf("\n\nERROR - cannot open input vector file\n");
         files_ok = 0;
         exit(0);
      }

      if( (weights = fopen(weights_file, "w+b")) == '\0')
      {
         printf("\n\nERROR - cannot open weights vector file\n");
         files_ok = 0;
         exit(0);
      }

      if(files_ok)
         train_the_adaline(inputs, weights, x, w, N);

   } /* ends training_mode */




      /* WORKING   MODE */

   if(working_mode)
   {

      if( (weights = fopen(weights_file, "r+b")) == '\0')
      {
         printf("\n\nERROR - cannot open weights vector file\n");
         exit(0);
      }
      else
         process_new_case(weights, x, w, N);

   }  /* ends if working_mode */

   free(x);
   free(w);

   return 0;


}  /* ends main */
