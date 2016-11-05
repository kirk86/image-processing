

/*
   madaline.c

   Dwayne Phillips
   February 1992

   The functions in this file implement the multiple
   adaptive linear element - Madaline - and the
   Madaline I learning law.

   Contents:
   display_2d_weights
   initialize_2d_weights
   madaline_output
   process_new_madaline
   train_the_madaline
   which_adaline

*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>




/*
   void initialize_2d_weights(w, N, A)

   This function initializes the weights in th
   2-dimensional weight vector.  They are set
   to 0 2 4 6 0 2 4 6 ...
*/

initialize_2d_weights(w, N, A)
   long  **w, N, A;
{
   int i, j;
   for(i=0; i<A; i++)
      for(j=0; j<N+1; j++)
         w[i][j] = ((i+j+1) % 4) * 2;
}  /* ends initialize_2d_weights */




/*
   long  display_2d_weights(w, N, A)

   This function displays the 2-dimemsional
   weights to the screen.
*/

display_2d_weights(w, N, A)
   long  **w, N, A;
{
   int i, j;
   for(i=0; i<A; i++){
      printf("\n>>");
      for(j=0; j<N+1; j++)
         printf("%10ld ", w[i][j]);
   }
}  /* ends display_2d_weights */




/*
   long  train_the_madaline(inputs, weights, x, w,
                            nets, outs, N, A, choice)

   x[N+2]  nets[A]  outs[A]  w[A][N+1]

   This function trains the Madaline network using
   the Madaline I training law.  It loops through
   the input vectors.  If an output is wrong, it
   trains the weights for that input.  If a single
   output is wrong from all of the inputs, then
   the training loop repeats.  This continues until
   all of the answers are correct, or until the training
   goes beyond a limit.
*/

train_the_madaline(inputs, weights, x, w,
                         nets, outs, N, A, choice)
   char  choice;
   FILE  *inputs, *weights;
   long  A, N, nets[], outs[], x[], **w;
{
   char  string[80];
   float eta = 0.5;
   int   adjusting = 1,
         counter   = 0,
         i,
         j,
         s;

   long  net,
         one_net,
         output,
         target,
         this_one;

   initialize_2d_weights(w, N, A);
   printf("\nEnter number of input vectors >>");
   gets(string);
   s = atoi(string);

   while(adjusting){
      printf("\nwhile adjusting counter=%d", counter);
      counter++;
      adjusting = 0;
      fseek(inputs, 0L, SEEK_SET);

      for(i=0; i<s; i++){
         fread(x, (N+2)*sizeof(long ), 1, inputs);

         for(j=0; j<A; j++){
            calculate_net(&one_net, N, w[j], x);
            nets[j] = one_net;
            outs[j] = calculate_output(nets[j]);
         }

         output = madaline_output(outs, choice, A);
         target = x[N+1];

         if(output != target){
            printf(" %d-t ", i);
            adjusting = 1;
            this_one  = which_adaline(target, outs, nets, A);
            printf("%ldx ", this_one);
            train_weights(target, nets[this_one], eta,
                             w[this_one], x, N);
         } /* ends if output != target */
         else
             printf(" %d-n ", i);

      } /* ends loop over s vectors */

      if(counter > 299){
         printf("\n\nTOO MUCH TRAINING - quiting");
         adjusting = 0;
      }


   }  /* ends while adjusting */

   printf("\n\nwent through the training cycle %d times",counter);
   fclose(inputs);
   for(i=0; i<A; i++)
      fwrite(w[i], (N+1)*sizeof(long ), 1, weights);
   fclose(weights);

   display_2d_weights(w, N, A);

}  /* ends train_the_madaline */





/*
   long  madaline_output(outputs, choice, A)

   This function sets the single output for the
   Madaline network.  It can use either the
   AND, OR, or Majority vote.
*/


madaline_output(outputs, choice, A)
   long  A, outputs[];
   char  choice;
{
   int   i,
         minus = 0,
         plus  = 0;
   long  result = -1;


      /* use the AND method */
   if(choice == 'a' ||
      choice == 'A'){
      result = 1;
      for(i=0; i<A; i++)
         if(outputs[i] == -1)
            result = -1;
   }

      /* use the OR method */
   if(choice == 'o' ||
      choice == 'O'){
      for(i=0; i<A; i++)
         if(outputs[i] == 1)
            result = 1;
   }

      /* use the Majority vote method */
   if(choice == 'm' ||
      choice == 'M'){
      for(i=0; i<A; i++){
         if(outputs[i] ==  1) plus++;
         if(outputs[i] == -1) minus++;
      }
      if(plus > minus) result = 1;
   }

   return(result);
}  /* ends madaline_output */




/*
   long  which_adaline(target, outs, nets, A)

   Find the nets that has the smallest absolute value
   and the corresponding outs does not equal the target.
*/

which_adaline(target, outs, nets, A)
   long  A, nets[], outs[], target;
{
   int   i;
   long  result, sum;

   sum = 32000;
   for(i=0; i<A; i++){
      if(outs[i] != target){
         if(abs(nets[i]) < sum){
            result = i;
            sum    = abs(nets[i]);
         }
      }
   }

   return(result);
     }  /* ends which_adaline */




/*
   long  process_new_madaline(weights, x, w, nets,
                              outs, choice, N, A)
   This function implements the working mode of the
   network.  It takes in a new input vector and
   calculates the answer.
*/

process_new_madaline(weights, x, w, nets,
                           outs, choice, N, A)
   char   choice;
   FILE   *weights;
   long   A, N, nets[], outs[], x[], **w;
{
   int   i, j;
   long  one_net, output;

   for(i=0; i<A; i++)
      fread(w[i], (N+1)*sizeof(long ), 1, weights);
   fclose(weights);
   display_2d_weights(w, N, A);

   get_straight_input_from_user(x, N);

   for(j=0; j<A; j++){
      calculate_net(&one_net, N, w[j], x);
      nets[j] = one_net;
      outs[j] = calculate_output(nets[j]);
   }

   output = madaline_output(outs, choice, A);
   printf("\n\noutput is %ld \n", output);
}  /* ends process_new_madaline */
