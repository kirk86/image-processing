
/*
     madmain.c

     This file contains the main calling
     routine for the Madaline (Modified
     Adaptive Linear Element) program.

     Dwayne Phillips
     February 1992

   Notes:
  
   the inputs array x has N+2 elements
     x[0] is always 1
     then there are 1 to N elements
     then the last element is the target
   the weights array w has N+1 by A elements
     w[0] is the bias
     then there are 1 to N elements
   the nets array nets has A elements
   the outputs array outs has A elements
  
*/




#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


main(argc, argv)
  int argc;
  char *argv[];
{
   char  choice;
   char  inputs_file[80], weights_file[80];
   FILE  *inputs, *weights;
   int   files_ok      = 1;
   long  **w, *x, *nets, *outs, N, A;
   int i, j;


   if(argc < 7){
      printf("\n\nUsage: madaline inputs_file weights_file ");
      printf("\n       size_of_vectors #_of_adalines mode choice_type");
      printf("\n   where mode=t (training)");
      printf("\n   where mode=i (input data)");
      printf("\n   where mode=w (working)");
      printf("\n   where choice_type=m (majority vote)");
      printf("\n   where choice_type=o (logical OR)");
      printf("\n   where choice_type=a (logical AND)");
      exit(1);
   }

   strcpy(inputs_file, argv[1]);
   strcpy(weights_file, argv[2]);

   N = atoi(argv[3]);
   A = atoi(argv[4]);

   if(argv[5][0] != 'i'  &&
      argv[5][0] != 'I'  &&
      argv[5][0] != 't'  &&
      argv[5][0] != 'T'  &&
      argv[5][0] != 'w'  &&
      argv[5][0] != 'W'){
        printf("\nERROR - Did not enter a correct mode");
        exit(1);
   }

   choice = argv[6][0];
   if(choice != 'o'  &&
      choice != 'O'  &&
      choice != 'A'  &&
      choice != 'a'  &&
      choice != 'm'  &&
      choice != 'M'){
        printf("\nERROR - Did not enter a correct choice type");
        exit(1);
   }

   x    = (long  *) malloc((N+2) * sizeof(long ));
   outs = (long  *) malloc(A * sizeof(long ));
   nets = (long  *) malloc(A * sizeof(long ));

   w = malloc(A * sizeof(long  *));
   for(i=0; i<A; i++){
      w[i] = malloc((N+1) * sizeof(long ));
      printf("\n\tw[%d] = %x", i, w[i]);
      if(w[i] == '\0'){
         printf("\n\tmalloc of w[%d] failed", i);
         exit(0);
      }
   }



      /* I N P U T   M O D E */

     if(argv[5][0] == 'i'  ||  argv[5][0] == 'I'){
      if( (inputs = fopen(inputs_file, "w+b")) == '\0'){
         printf("\n\nERROR - cannot open input vector file\n");
         exit(0);
      }
      else
         get_straight_input_vectors(inputs, x, N);
   }  /* ends if input_mode */


      /*  T R A I N I N G   M O D E */

   if(argv[5][0] == 't'  ||  argv[5][0] == 'T'){
      if( (inputs = fopen(inputs_file, "r+b")) == '\0'){
         printf("\n\nERROR - cannot open input vector file\n");
         files_ok = 0;
         exit(0);
      }

      if( (weights = fopen(weights_file, "w+b")) == '\0'){
         printf("\n\nERROR - cannot open weights vector file\n");
         files_ok = 0;
         exit(0);
      }

      if(files_ok){
                printf("files ok");
         train_the_madaline(inputs, weights, x, w,
                            nets, outs, N, A, choice);
        }

    }  /* ends training mode */



      /* W O R K I N G   M O D E */
   if(argv[5][0] == 'w'  ||  argv[5][0] == 'W'){

      if( (weights = fopen(weights_file, "r+b")) == '\0'){
         printf("\n\nERROR - cannot open weights vector file\n");
         exit(0);
      }
      else
         process_new_madaline(weights, x, w, nets, outs, choice, N, A);

   }  /* ends if working_mode */



      /* free up the allocated space */

   for(i=0; i<A; i++)
      free(w[i]);

   free(w);
   free(x);
   free(nets);
   free(outs);

}  /* ends main */
