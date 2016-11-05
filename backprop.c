/*PAGE Introduction

   backprop.c

   Dwayne Phillips
   August 1992
   April 1994 - resumed work

   The functions in this file implement the
   backpropogation neural network.

   NOTES: Dimensions of arrays    
      m = number of inputs
      n = number of layers of neurons
      o = number of outputs
      p = number of neurons in each layer

      input layer               x      = 1 X m
      
      The weights between
      input and first neurons   win    = m X p
      
      outputs of the hidden 
      layers                    h      = p X n

      The weights between 
      hidden layers             whid   = (n-1) X p X p
      
      The weights between last 
      neurons and output        wout   = p X o
      
      output layer              y      = o X 1
      
      the target                target = o x 1
      
      output layer error        delta  = o x 1
      
      
      Note
      Because of problems with malloc and 2 and 3
      dimensional arrays, I will use 1 dimensional
      arrays for everything.
      
      For example, if there is an a[2][3], I will
      make it a[6] with the elements stored as:
         0 1 2
         3 4 5
         
      For array[a][b], element [i][j] is:         
      array[(i*b) + j]
         
      If there is an a[3][2][5], I will make it
      a[30] with the elements stored as:
         0  1  2  3  4
         5  6  7  8  9
         
         10 11 12 13 14
         15 16 17 18 19
         
         20 21 22 23 24
         25 26 27 28 29
         
         For array[a][b][c], element [i][j][k] is:
         array[(i*b*c) + (j*c) +  k]
         

      5-18-94 Let's use Widrow's method
      which does not use the weight_delta
      arrays.  Those are used in the momentum
      variation to backpropogation.  Widrow
      also describes that technique.
      
      
      5-20-94 ASCII Text Files
      This program can read many of the user inputs
      from ASCII text files.  This makes it easier
      on the user sometimes.  Instead of typing in
      everything everytime, you type it into an
      ASCII file once using any old text editor.
      This feature works for entering the program
      parameters and also for entering arrays of
      input and target data.
      
      
      5-21-94 The data_sets variable
      In the input and training modes you have more
      than one set of input and target data.  The
      working mode only has one data set.  The tmp_x
      and tmp_target variables hold all the data sets
      and you copy one data set at a time into
      x and target.
      
      5-30-94 I needed to remove all the references
      to the h_file since it is not used.
      
      5-30-94 This program only uses the sigmoid
      function.  It does not use any of the other
      functions so I need to remove the references
      to the other function types.
         
               
 

*/
/*PAGE Includes and defines
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef float  ELEMENT;

#define FILL            33.33
#define LENGTH          80
#define TWOMU           0.7
#define MIN_ERROR       0.02
#define TRAINING_LIMIT  99999



#undef ACTIVATE_TEST
#undef INIT_TEST
#undef SUM_TEST
#undef MAT_MUL_TEST
#undef INPUT_LAYER_TEST
#undef TEST_3D_COPY
#undef TEST_1D_COPY
#undef TEST_2D_COPY
#undef INNER_TEST
#undef OUTPUT_LAYER_TEST
#undef FORWARD_TEST
#undef FILE_TEST
#define NORMAL_INPUT
#undef DEBUG


void malloc_2d();
void malloc_3d();
void vector_sum_of_products();
void initialize_2d_weights();
void initialize_3d_weights();
void display_2d_weights();
void display_3d_weights();
void matrix_multiply();
void input_layer();    
void inner_layers();
void output_layer();
void fill_array();
void zero_array();
void copy_3d_to_2d();
void copy_2d_to_1d();
void copy_1d_to_2d();
void apply_function();
void lower_case_string();
void get_program_parameters();
void get_array_from_user();
void write_to_file();
void read_from_file();
void output_layer_error();
void neuron_deltas();
void change_output_weights();
void change_hidden_weights();
void change_input_weights();
void file_input_parameters();
FILE *open_file();
ELEMENT activation_function();
ELEMENT el_abs();
int  error_is_acceptable();



/***************************************************/
/***************************************************/
/*PAGE main
*/
main(argc, argv)
   int  argc;
   char *argv[];
{
   char    in_file_name[LENGTH],
           mode[LENGTH], 
           out_file_name[LENGTH],
           target_file_name[LENGTH],
           type[LENGTH],
           whid_file_name[LENGTH],
           win_file_name[LENGTH],
           wout_file_name[LENGTH];
            
   FILE    *in_file, 
           *out_file, 
           *target_file, 
           *whid_file, 
           *win_file, 
           *wout_file;
           
   int     big_pass = 0,
           changing_weights = 1,
           data_sets = 1,
           i = 0,
           m = 0, 
           n = 0, 
           o = 0, 
           p = 0, 
           still_training = 1, 
           this_data_set = 0,
           training_all_data_sets = 1;
   
   ELEMENT *delta, 
           *h, 
           *h_delta, 
           *target, 
           *tmp_target,
           *tmp_x,
           *x, 
           *y, 
           *whid,  
           *win,  
           *wout;
           
   unsigned long training_pass = 0;           






   get_program_parameters(mode, 
                          &m, 
                          &n, 
                          &o, 
                          &p,
                          &data_sets,
                          type,
                          in_file_name,
                          out_file_name,
                          target_file_name,
                          whid_file_name,
                          win_file_name,
                          wout_file_name);

      /*********************************
      *
      *   Malloc each of the arrays
      *
      **********************************/
          
   x          = (ELEMENT  *) 
                malloc((int)(m) * sizeof(ELEMENT));
   y          = (ELEMENT  *) 
                malloc((int)(o) * sizeof(ELEMENT)); 
   delta      = (ELEMENT  *) 
                malloc((int)(o) * sizeof(ELEMENT)); 
   target     = (ELEMENT  *) 
                malloc((int)(o) * sizeof(ELEMENT)); 
   win        = (ELEMENT  *) 
                malloc((int)(m*p) * sizeof(ELEMENT));
   wout       = (ELEMENT  *) 
                malloc((int)(p*o) * sizeof(ELEMENT));
   h          = (ELEMENT  *) 
                malloc((int)(p*n) * sizeof(ELEMENT));
   h_delta    = (ELEMENT  *) 
                malloc((int)(p*n) * sizeof(ELEMENT));
   whid       = (ELEMENT  *) 
                malloc((int)
                   ((n-1)*p*p) * sizeof(ELEMENT));
   tmp_x      = (ELEMENT  *) 
                malloc((int)
                   (m*data_sets) * sizeof(ELEMENT));
   tmp_target = (ELEMENT  *) 
                malloc((int)
                   (o*data_sets) * sizeof(ELEMENT));

  
  
      /*********************************
      *
      *   Modes of operation
      *
      **********************************/ 
      
      /*********************************
      *
      *   Input
      *
      **********************************/ 
   
   if( (strncmp(mode, "input", 3) == 0)){ 
      
      in_file     = open_file(in_file_name, "w+b");
      target_file = open_file(target_file_name,"w+b");

      printf("\n\nEnter the inputs");
      get_array_from_user(tmp_x, m*data_sets);   
      
      printf("\n\nEnter the targets");
      get_array_from_user(tmp_target, o*data_sets);

      write_to_file(tmp_x, m*data_sets, in_file);
      write_to_file(tmp_target, 
                    o*data_sets, 
                    target_file);
                       
      fclose(in_file);  
      fclose(target_file);  
     
   }  /* ends if input */
    

      
      /*********************************
      *
      *   Training
      *
      **********************************/   
      
   if( (strncmp(mode, "training", 3) == 0)){

      in_file     = open_file(in_file_name, "r+b");
      target_file = open_file(target_file_name,"r+b");
      win_file    = open_file(win_file_name, "w+b");
      whid_file   = open_file(whid_file_name, "w+b");
      wout_file   = open_file(wout_file_name, "w+b");
      
      read_from_file(tmp_x, 
                     m*data_sets, 
                     in_file);
      read_from_file(tmp_target, 
                     o*data_sets, 
                     target_file);
      
      initialize_2d_weights(win, m, p);
      initialize_2d_weights(wout, p, o);
      initialize_3d_weights(whid, (n-1), p, p);


      changing_weights = 1;
      while(changing_weights){  
         printf("\n\n\nbig pass %d  training pass %ld ----------------------------", 
            big_pass++, training_pass);
         changing_weights       = 0;       
         this_data_set          = 0;
         training_all_data_sets = 1;
      
         while(training_all_data_sets){
              
            for(i=0; i<m; i++)
               x[i] = tmp_x[i + m*this_data_set];
            for(i=0; i<o; i++)
               target[i] = 
                  tmp_target[i + o*this_data_set];
                                
            still_training = 1;
            while(still_training){
      
               input_layer(h, x, win, m, p, n, type);
               inner_layers(h, whid, p, n, type);
               output_layer(h, wout, y, p, n, o,type);
               output_layer_error(y, target, delta,o);
         

#ifdef NEVER         
printf("\n\nPass %d, input is", training_pass);
display_2d_weights(x, 1, m);
printf("\n\ntarget is");
display_2d_weights(target, 1, o);
printf("\n\toutput is");
display_2d_weights(y, 1, o);
printf("\n\tdelta is");
display_2d_weights(delta, 1, o);
gets(string);     
#endif


               if( (error_is_acceptable(delta, o))  ||
                   (training_pass > TRAINING_LIMIT))
                  still_training = 0;
               else{
                  changing_weights = 1;
                  training_pass++;             
                  neuron_deltas(h, h_delta, delta, 
                                whid, wout, n, o, p);
                  change_output_weights(wout, delta, 
                                        h, n, o, p);
                  change_hidden_weights(whid, h, 
                                        h_delta, n,p);
                  change_input_weights(x, win,h_delta,
                                       m, n, p);
               }  /* ends else still_training */

            }  /* ends while still_training */

            printf("\n\nFinished training data set %d --- training pass %ld",
                    this_data_set, training_pass);
                    
printf("\nThis is the result");
display_2d_weights(y, 1, o);

printf("\nThis is the target ");
display_2d_weights(target, 1, o);
printf("\nThis is the delta");
display_2d_weights(delta, 1, o);


            this_data_set++;
            if(this_data_set >= data_sets)
               training_all_data_sets = 0;
         
         }  /* ends while training_all_data_sets */
         
      }  /* ends while changing_weights */
                      
      write_to_file(win, m*p, win_file);
      write_to_file(whid, (n-1)*p*p, whid_file);
      write_to_file(wout, p*o, wout_file);
      
      fclose(in_file);
      fclose(target_file);
      fclose(win_file);
      fclose(whid_file);
      fclose(wout_file);
      
   }  /* ends if training */
      
      
      
      /*********************************
      *
      *   Working
      *
      **********************************/

   if( (strncmp(mode, "working", 3) == 0)){
   
printf("\nHey this is the working mode!");   
      
      in_file     = open_file(in_file_name, "r+b");
      out_file    = open_file(out_file_name,"w+b");
      win_file    = open_file(win_file_name, "r+b");
      whid_file   = open_file(whid_file_name, "r+b");
      wout_file   = open_file(wout_file_name, "r+b");
   
      read_from_file(x, m, in_file);
      read_from_file(win, m*p, win_file);
      read_from_file(whid, (n-1)*p*p, whid_file);
      read_from_file(wout, p*o, wout_file);
      
      input_layer(h, x, win, m, p, n, type);
      inner_layers(h, whid, p, n, type);
      output_layer(h, wout, y, p, n, o, type);   

printf("\nThe input was");
display_2d_weights(x, 1, m);      
printf("\n\nResults: (output)");
display_2d_weights(y, 1, o);      
   
      fclose(in_file);
      fclose(out_file);
      fclose(win_file);
      fclose(whid_file);
      fclose(wout_file);
   
   }  /* ends if working */

  
      /*********************************
      *
      *   Free each of the arrays
      *
      **********************************/

   free(x);
   free(y);
   free(delta);
   free(h_delta);
   free(target);
   free(h);
   free(whid);
   free(win);
   free(wout);   
   free(tmp_x);
   free(tmp_target);
   return(1);
}  /* ends main */

/***************************************************/
/*PAGE input_layer

   void input_layer(...

   This function performs the calculations
   between the input array and the input weights
   to produce the first layer of neurons.
   
   x[1][m] win[m][p] = first column of h[p][n]
   
   Use a tmp array in the call to matrix_multiply
   then copy the answer back to the first column
   of h.
*/

void input_layer(ELEMENT *h, 
                 ELEMENT *x, 
                 ELEMENT *win, 
                 int     m,
                 int     p,
                 int     n,
                 char    type[])
{
   ELEMENT *tmp;
   tmp = (ELEMENT  *) 
         malloc((int)(p) * sizeof(ELEMENT));

   zero_array(tmp, p);      
   matrix_multiply(x, 1, m, win, p, tmp);
   apply_function(tmp, p, type);
   copy_1d_to_2d(tmp, h, p, n, 0, "col");
   free(tmp);      
   
}  /* ends input_layer */

/***************************************************/
/*PAGE output_layer

   void output_layer(...

   This function performs the calculations
   between the last layer of neurons and the output
   weights to produce the outputs of the network.
   
   last column of h[p][n] wout[p][o] = y[o][1]
   
*/

void output_layer(ELEMENT *h,
                  ELEMENT *wout, 
                  ELEMENT *y, 
                  int     p,
                  int     n,
                  int     o,
                  char    type[])
{
   ELEMENT *tmp;
   
   tmp = (ELEMENT  *) 
         malloc((int)(p) * sizeof(ELEMENT));
   copy_2d_to_1d(tmp, h, p, n, (n-1), "col");   
   zero_array(y, o);      
   matrix_multiply(tmp, 1, p, wout, o, y);
   apply_function(y, o, type);
   free(tmp);      
   
}  /* ends output_layer */

/***************************************************/
/*PAGE inner_layers

   void inner_layers(...
   
   This function performs the forward calculations
   for the inner layers.
   h[p][n]  whid[(n-1)][p][p]
   
   Perform the multiplications by using the matrix
   multiplication function and temporary arrays.
   
   for i=0; i<n-1; i++
      Copy the ith column of h to a tmp1 array
      Copy the ith pXp array of whid to a tmp2 array
      matrix_multiply tmp1 X tmp2 = tmp3
      Copy tmp3 to the i+1 column of h
   end loop over i   
*/

void inner_layers(ELEMENT *h,
                  ELEMENT *whid,
                  int     p,
                  int     n,
                  char    type[])
{        
   ELEMENT *tmp1, *tmp2, *tmp3;
   int i;
   
   tmp1 = (ELEMENT  *) 
          malloc((int)(p) * sizeof(ELEMENT));
   tmp2 = (ELEMENT  *) 
          malloc((int)(p*p) * sizeof(ELEMENT));
   tmp3 = (ELEMENT  *) 
          malloc((int)(p) * sizeof(ELEMENT));
     
   for(i=0; i<(n-1); i++){
      zero_array(tmp3, p);
      copy_2d_to_1d(tmp1, h, p, n, i, "col");
      copy_3d_to_2d(whid, tmp2, (n-1), p, p, i);
      matrix_multiply(tmp1, 1, p, tmp2, p, tmp3);
      apply_function(tmp3, p, type);
      copy_1d_to_2d(tmp3, h, p, n, (i+1), "col");
   }  /* ends loop over n-1 inner layers */
   
   free(tmp1);
   free(tmp2);
   free(tmp3);
}  /* ends inner_layers */                     

/***************************************************/
/*PAGE matrix_multiply

   void matrix_multiply(...
   
   This function performs basic matrix multiplication.
   A[m][n] * B[n][p] = C[m][p]
*/
void matrix_multiply(ELEMENT *A, int m, int n, 
                     ELEMENT *B, int p, 
                     ELEMENT *C)
{
   int i, j, k;
   for(i=0; i<m; i++){
      for(j=0; j<p; j++){
         for(k=0; k<n; k++){
            /** C[i][j] = 
                C[i][j] + A[i][k] * B[k][j] **/
            C[i*p + j] = 
               C[i*p + j] + A[i*n + k]*B[k*p + j];
         }  /* ends loop over k */
      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends matrix_multiply */                 
/***************************************************/
/*PAGE activation_function

   ELEMENT activation_function(...
   
   This function applies the sigmoid
   to an input ELEMENT.  It returns the
   result of the function.

*/

ELEMENT activation_function(input, type)
   char    type[];
   ELEMENT input;
{          
   double  a, b, c;
   ELEMENT result = (ELEMENT)(0.0);

   if( strcmp(type, "sigmoid") == 0){
      a      = (double)(input);
      b      = exp(-1.0*a);
      c      = 1.0/(1.0 + b);
      result = (ELEMENT)(c);
   }
   else{
      printf("\nERROR: must use sigmoid function");
      exit(2);
   }

   return(result);
   
}  /* ends activation_function */
/***************************************************/
/*PAGE apply_function

   void apply_function(...
   
   This function applies the selected activation 
   function to each element in an array.
*/

void apply_function(ELEMENT *array, 
                    int     size, 
                    char    type[])   
{
   ELEMENT tmp;
   int i;
   for(i=0; i<size; i++){
      tmp = activation_function(array[i], type);
      array[i] = tmp;
   }
}  /* ends apply_function */                    
/***************************************************/
/*PAGE initialize_2d_weights

   void initialize_2d_weights(...

   This function initializes the weights in th
   2-dimensional array.  They are set
   to 0 2 4 6 0 2 4 6 ...
*/

void initialize_2d_weights(ELEMENT *array, 
                           int a, 
                           int b)
{
   ELEMENT count = (ELEMENT)(0.0);
   int i, j, odd = 0;;
   for(i=0; i<a; i++){
      for(j=0; j<b; j++){
         odd++;
         count = (ELEMENT)(rand());
         while(count > 1.0) 
            count = count/(ELEMENT)(10.0);
         if( (odd % 2) == 1) 
            count = count*(ELEMENT)(-1.0);
         /****array[(i*a) + j] = 
              (ELEMENT)((i*a) + j);****/
         array[(i*b) + j] = count;
      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends initialize_2d_weights */

/***************************************************/
/*PAGE initialize_3d_weights

   void initialize_3d_weights(...

   This function initializes the weights in the
   2-dimensional array.  They are set
   to 0 2 4 6 0 2 4 6 ...
*/

void initialize_3d_weights(ELEMENT *array, 
                           int a, 
                           int b, 
                           int c)
{
   ELEMENT count = (ELEMENT)(0.0);
   int i, j, k, odd = 0;;
   for(i=0; i<a; i++){
      for(j=0; j<b; j++){
         for(k=0; k<c; k++){
            odd++;  
            count = (ELEMENT)(rand());
            while(count > 1.0) 
               count = count/(ELEMENT)(10.0);
            if( (odd % 2) == 1) 
               count = count*(ELEMENT)(-1.0);
            array[(i*b*c) + (j*c) + k] = count;
         }  /* ends loop over k */
      }  /* ends loop over j */
   }  /* ends loop over i */
            
}  /* ends initialize_3d_weights */
/***************************************************/
/*PAGE display_2d_weights

   void display_2d_weights(...

   This function displays the 2-dimemsional
   weights to the screen.  array[a][b]
*/

void display_2d_weights(ELEMENT *array, int a, int b)
{  
   int i, j;
   printf("\n  %d X %d", a, b);
   for(i=0; i<a; i++){
      printf("\n2D - %d>>", i);
      for(j=0; j<b; j++){
         printf("%.4f ", array[(i*b) + j]);
      }
   }
}  /* ends display_2d_weights */


/***************************************************/
/*PAGE display_3d_weights

   void display_3d_weights(...

   This function displays the 3-dimemsional
   weights to the screen.  array[a][b][c]
*/

void display_3d_weights(ELEMENT *array, 
                        int a, 
                        int b, 
                        int c)
{
   int i, j, k; 
   printf("\n\n%d X %d X %d", a, b, c);
   for(i=0; i<a; i++){
      printf("\n\n3D>>");
      for(j=0; j<b; j++){
         printf("\n%d>>", i);
         for(k=0; k<c; k++){            
            printf("%.4f ", 
                   array[(i*b*c) + (j*c) + k]);
         }
      }
   }
}  /* ends display_3d_weights */

/***************************************************/
/*PAGE fill_array

   void fill_array(...
   
   This function sets all the elements of an array
   to the FILL value.
*/

void fill_array(ELEMENT *array, int size)
{
   int i;
   for(i=0; i<size; i++)
      array[i] = (ELEMENT)(FILL);
}  /* ends fill_array */
/***************************************************/
/*PAGE zero_array

   void zero_array(...
   
   This function sets all the elements of an array
   to zero.
*/

void zero_array(ELEMENT *array, int size)
{
   int i;
   for(i=0; i<size; i++)
      array[i] = (ELEMENT)(0.0);
}  /* ends zero_array */
/***************************************************/
/*PAGE copy_3d_to_2d

   void copy_3d_to_2d(...
   
   This function copies a 2D array from a big 3D
   array down into a simple 2D array.
   three[a][b][c] copies this 2D array down into
   two[b][c]
*/

void copy_3d_to_2d(ELEMENT *three,
                   ELEMENT *two,
                   int a,
                   int b,
                   int c,
                   int this)
{
   int i, j;
   for(i=0; i<b; i++){
      for(j=0; j<c; j++){
         /** two[i][j] = three[this][i][j] **/
         two[i*c + j] = three[this*b*c + i*c + j];
      }
   }
}  /* ends copy_3d_to_2d */                   
/***************************************************/
/*PAGE copy_1d_to_2d

   void copy_1d_to_2d(...
   
   This function copies a 1D array into a 2D array.
   You can copy the 1D array into either this row 
   of the 2D array (option = "row") or this column
   of the 2D array (option = "col").
   one[1][a] or one [1][b]
   two[a][b]
*/

void copy_1d_to_2d(ELEMENT *one,
                   ELEMENT *two,
                   int     a,
                   int     b,
                   int     this,
                   char    option[])
{
   int i; 
   
   if(strncmp(option, "row", 3) == 0){
      for(i=0; i<b; i++){
         /** two[this][i] = one[i] **/
         two[this*b + i] = one[i];
      }
   }  /* ends if row */

   if(strncmp(option, "col", 3) == 0){
      for(i=0; i<a; i++){
         /** two[i][this] = one[i] **/
         two[i*b + this] = one[i];
      }
   }  /* ends if col */

}  /* ends copy_1d_to_2d */                   
/***************************************************/ 
/*PAGE copy_2d_to_1d

   void copy_2d_to_1d(...
   
   This function copies this row or column from a 2D
   array into a 1D array.  The option parameter must
   equal either "row" or "col".
   one[1][a] or one [1][b]
   two[a][b]
*/

void copy_2d_to_1d(ELEMENT *one,
                   ELEMENT *two,
                   int     a,
                   int     b,
                   int     this,
                   char    option[])
{
   int i; 
   
   if(strncmp(option, "row", 3) == 0){
      for(i=0; i<b; i++){
         /** one]i] = two[this][i] **/
         one[i] = two[this*b + i];
      }
   }  /* ends if row */

   if(strncmp(option, "col", 3) == 0){
      for(i=0; i<a; i++){
         /** one[i] = two[i][this] **/
         one[i] = two[i*b + this];
      }
   }  /* ends if col */

}  /* ends copy_2d_to_1d */                   
/***************************************************/
/*PAGE lower_case_string

   void lower_case_string(string)
   
   This function converts the characters in the
   string to lower case.
   
*/

void lower_case_string(char string[])
{
   int i, c;
   for(i=0; i<(int)(strlen(string)); i++){
      c         = string[i];
      string[i] = tolower(c);
   }
}  /* ends lower_case_string */   
/***************************************************/
/*PAGE get_program_parameters

   void get_program_parameters(...
   
   This function gets all the parameters for 
   the program.  You can either enter the parameters
   interactively or have the program read them
   from an ASCII file.
*/

void get_program_parameters(char mode[], 
          int *m, 
          int *n, 
          int *o, 
          int *p,
          int *data_sets,
          char type[],
          char in_file_name[],
          char out_file_name[],
          char target_file_name[],
          char whid_file_name[],
          char win_file_name[],
          char wout_file_name[])
          
{
   char string[LENGTH];
   int  choice;

   printf("\nEntering the program parameters");
   printf("\nEnter input from");
   printf("\n   1. keyboard");
   printf("\n   2. file");
   printf("\n   :");
   gets(string);
   choice = atoi(string);
   
   if(choice == 1){  /* keyboard input */
   
      printf("\nEnter the mode ");
      printf("(input, training, working)");
      gets(mode);
      lower_case_string(mode);
   
      printf("\nEnter m (# of inputs): ");
      gets(string);
      *m = atoi(string);
   
      printf("\nEnter n (# of hidden layers): ");
      gets(string);
      *n = atoi(string);
   
      printf("\nEnter o (# of outputs): ");
      gets(string);
      *o = atoi(string);
   
      printf("\nEnter p (# of neurons per ");
      printf("hidden layer): ");
      gets(string);
      *p = atoi(string);

      printf("\nEnter data_sets (# of data sets): ");
      gets(string);
      *data_sets = atoi(string);
      
      printf("\nEnter the input file name:");
      gets(in_file_name);
      
      printf("\nEnter the output file name:");
      gets(out_file_name);
      
      printf("\nEnter the target file name:");
      gets(target_file_name);
      
      printf("\nEnter the input weights file name:");
      gets(win_file_name);
      
      printf("\nEnter the hidden weights file name:");
      gets(whid_file_name);
      
      printf("\nEnter the output weights file name:");
      gets(wout_file_name);
      
   }  /* ends if choice == 1, keyboard */
   
   else{  /* file input */
      printf("\nEnter the name of the file");
      printf("\ncontaining the program parameters");
      printf("\n   :");
      gets(string);
      file_input_parameters(string, 
                            mode, 
                            m, 
                            n, 
                            o, 
                            p,
                            data_sets,
                            type,
                            in_file_name,
                            out_file_name,
                            target_file_name,
                            whid_file_name,
                            win_file_name,
                            wout_file_name);
      
   }  /* ends else file input */

}  /* ends get_program_parameters */          

/***************************************************/
/*PAGE get_array_from_user

   void get_array_from_user(...
   
   This function interacts with the user so the 
   user can type in the elements of an array. 
   
   The user can choose to have the program read
   the array from an ASCII text file.  This makes it
   easier to enter and change the numbers using an
   editor instead of interacting with the program.
   The numbers in the ASCII file are kept one
   number per line.
*/

void get_array_from_user(ELEMENT *array, int size)
{
   char string[LENGTH];
   FILE *array_file;
   int  choice, i;  
   
   printf("\nEnter input from");
   printf("\n   1. keyboard");
   printf("\n   2. file");
   printf("\n   :");
   gets(string);
   choice = atoi(string);
   
   if(choice == 1){  /* keyboard input */   
   
      for(i=0; i<size; i++){
         printf("\nEnter element %d: ", i);
         gets(string);
         array[i] = (ELEMENT)(atof(string));
      }  /* ends loop over i */
   }  /* ends keyboard input */
   
   else{  /* read array from an ASCII file */ 
   
      printf("\nEnter the name of the file");
      printf("\ncontaining the array elements");
      printf("\n   :");
      gets(string);
      array_file = open_file(string, "r");
      i = 0;
      while(fgets(string, LENGTH, array_file)){
         array[i] = (ELEMENT)(atof(string));       
         i++;
      }  /* ends while fgets */
      fclose(array_file);

   }  /* ends else read from an ASCII file */
   
}  /* ends get_array_from_user */   
/***************************************************/
/*PAGE open_file

   void open_file(...
   
   This function opens a file using the attributes
   passed to it for the file.
*/

FILE *open_file(file_name, attributes)
   char *file_name, *attributes;
{  
   FILE *file_pointer;
   if((file_pointer = 
          fopen(file_name, attributes)) == '\0'){
      printf(
         "\n\nERROR - cannot open file %s\n",
         file_name);
      exit(0);
   }
   
   return(file_pointer);

}  /* ends open_file */               
/***************************************************/
/*PAGE write_to_file

   void write_to_file(...
   
   This function writes an array of ELEMENT 
   to a file.
*/

void write_to_file(ELEMENT *array,
                   int     size,
                   FILE    *file_pointer)
{
   int written = -1;
   written = fwrite(array,
                    size*sizeof(ELEMENT), 
                    1, 
                    file_pointer); 

}  /* ends write_to_file */                      
/***************************************************/
/*PAGE read_from_file

   void read_from_file(...
   
   This function reads an array of ELEMENT 
   from a file.
*/

void read_from_file(ELEMENT *array,
                   int     size,
                   FILE    *file_pointer)
{
   int read = -1;
   read = fread(array,
                size*sizeof(ELEMENT), 
                1, 
                file_pointer); 

}  /* ends read_from_file */                      
/***************************************************/
/*PAGE output_layer_error

   void output_layer_error(...
   
   This function computes the error found in the 
   output layer and places the error in the delta
   array.
*/
   
void output_layer_error(ELEMENT *y, 
                        ELEMENT *target, 
                        ELEMENT *delta, 
                        int     o)
{                        
   ELEMENT one = (ELEMENT)(1.0);
   int i;
   
   for(i=0; i<o; i++){
      delta[i] = y[i]*(one - y[i])*(target[i] - y[i]);
   }
}  /* ends output_layer_error */
/***************************************************/ 
/*PAGE neuron_deltas

   void neuron_deltas(...
   
   This function calculates the deltas for the 
   neurons in the network.  It starts are the
   output stage of the network and works its
   way back through the network to the input
   layer.
*/   

void neuron_deltas(ELEMENT *h, 
                   ELEMENT *h_delta, 
                   ELEMENT *delta, 
                   ELEMENT *whid,
                   ELEMENT *wout, 
                   int     n, 
                   int     o,
                   int     p)
{                   
   ELEMENT *tmp, *tmph, *tmpw;
   ELEMENT one = (ELEMENT)(1.0);
   int     i, j;
   
   
   tmp  = (ELEMENT  *) 
          malloc((int)(p) * sizeof(ELEMENT));
   tmph = (ELEMENT  *) 
          malloc((int)(p) * sizeof(ELEMENT));
   tmpw = (ELEMENT  *) 
          malloc((int)(p*p) * sizeof(ELEMENT));

      /**********************************
      *
      *   First, find the deltas for the
      *   last layer of neurons using the
      *   delta and wout arrays.
      *
      *************************************/   
      
   zero_array(tmp, p);
   matrix_multiply(wout, p, o, delta, 1, tmp);

   for(i=0; i<p; i++){
      /**h_delta[i][n-1] = 
         h[i][n-1] * (one - h[i][n-1]) * tmp[i];**/
      h_delta[i*n + n-1] = 
         h[i*n + n-1] * (one - h[i*n + n-1]) * tmp[i];
      
   }                                            

      /************************************
      *
      *   Now, find the deltas for all the
      *   other layers of neurons.
      *
      *************************************/   

   for(i=(n-1); i>0; i--){

      copy_3d_to_2d(whid, tmpw, (n-1), p, p, (i-1));
      copy_2d_to_1d(tmph, h_delta, p, n, i, "col");

      zero_array(tmp, p);
      matrix_multiply(tmpw, p, p, tmph, 1, tmp);
      
      for(j=0; j<p; j++){
         /**h_delta[j][i-1] = 
            h[j][i-1]*(one - h[j][i-1])*tmp[j];**/
         h_delta[j*n + i-1] = 
            h[j*n + i-1]*(one - h[j*n + i-1])*tmp[j];
      }  /* ends loop over j */
   }  /* ends loop over i */
      
   free(tmp);
   free(tmph);
   free(tmpw);
}  /* ends neuron_deltas */
/***************************************************/
/*PAGE change_output_weights

   void change_output_weights(...
   
   This function modifies the weights in the wout
   matrix.
*/
   
void change_output_weights(ELEMENT *wout, 
                           ELEMENT *delta, 
                           ELEMENT *h, 
                           int     n,
                           int     o, 
                           int     p)
{
   int i, j;
   
   for(i=0; i<p; i++){
      for(j=0; j<o; j++){
         /**wout[i][j] = 
            wout[i][j] + TWOMU*h[i][n-1]*delta[j];**/
         wout[i*o + j] = wout[i*o + j] +
           (ELEMENT)(TWOMU) * h[i*n + n-1] * delta[j];
      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends change_output_weights */
/***************************************************/
/*PAGE change_hidden_weights

   void change_hidden_weights(...
   
   This function modifies the weights in the whid
   matrix.
*/
   
void change_hidden_weights(ELEMENT *whid, 
                           ELEMENT *h, 
                           ELEMENT *h_delta, 
                           int     n,
                           int     p)
{
   int i, j, k;
   
   for(i=(n-1); i>0; i--){
      for(j=0; j<p; j++){
         for(k=0; k<p; k++){
            /**whid[i-1][j][k] = wout[i-1][j][k]
                  + TWOMU*h[j][i-1]*h_delta[k][i];**/
                  
            whid[(i-1)*p*p + j*p + k] = 
               whid[(i-1)*p*p + j*p + k] +
               (ELEMENT)(TWOMU) *
               h[j*n + i-1] *
               h_delta[k*n + i];
               
         }  /* ends loop over k */
      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends change_hidden_weights */
/***************************************************/
/*PAGE change_input_weights

   void change_input_weights(...
   
   This function modifies the weights in the 
   win matrix.
*/

void change_input_weights(ELEMENT *x, 
                          ELEMENT *win, 
                          ELEMENT *h_delta, 
                          int     m, 
                          int     n, 
                          int     p)
{                          
   int i, j;
   
   for(i=0; i<m; i++){
      for(j=0; j<p; j++){
         /**win[i][j] = win[i][j] + 
                        TWOMU*x[i]*h_delta[j][0];**/
         win[i*p + j] = win[i*p + j] +              
                        (ELEMENT)(TWOMU) *
                        x[i]*h_delta[j*n + 0];
      }   /* ends loop over j */
   }  /* ends loop over i */
}  /* ends change_input_weights */
/***************************************************/
/*PAGE error_is_acceptable
       
   int error_is_acceptable(...
   
   This function looks at the error array.  If all
   elements are smaller than the MIN_ERROR, return
   1, else return 0.
*/
   
int error_is_acceptable(ELEMENT *errors, int size)
{
   int i, result = 1;
   for(i=0; i<size; i++){   
      if(el_abs(errors[i]) > (ELEMENT)(MIN_ERROR))
         result = 0;
   }  /* ends loop over i */
   return(result);
}  /* ends error_is_acceptable */
/***************************************************/
/*PAGE el_abs

   ELEMENT el_abs
   
   This function returns the absolute value of
   an ELEMENT number.
*/

ELEMENT el_abs(number)
   ELEMENT number;
{
   ELEMENT result;
   if(number < (ELEMENT)(0.0))
      result = (ELEMENT)(-1.0) * (ELEMENT)(number);
   else
      result = (ELEMENT)(number);
   return(result);      
}  /* ends el_abs */         
/***************************************************/
/*PAGE file_input_parameters

   void file_input_parameters(...
   
   This function gets all the parameters for 
   the program by reading an input text file.
   This allows the user to type these parameters
   one time in an ASCII file and use them over
   and over quickly.
   
   You store the program information in the ASCII
   file in the following format.  The parameters
   are kept on separate lines.  Each line begins
   with a keyword, then a space, then the value.
   You can put the lines in any order.  You
   YOU MUST USE THE CORRECT KEYWORD OR THIS ROUTINE
   WILL GET LOST!
   
   mode xxxx
   m xxxx
   n xxxx
   o xxxx 
   p xxxx
   data-sets xxxx
   type xxxx
   in-file xxxx
   out-file xxxx
   target-file xxxx
   whid-file xxxx
   win-file xxxx
   wout-file xxxx
*/

void file_input_parameters(char file_name[],
          char mode[], 
          int *m, 
          int *n, 
          int *o, 
          int *p,
          int *data_sets,
          char type[],
          char in_file_name[],
          char out_file_name[],
          char target_file_name[],
          char whid_file_name[],
          char win_file_name[],
          char wout_file_name[])
          
{
   char keyword[LENGTH], 
        string[LENGTH], 
        value[LENGTH];
        
   FILE *parameters_file;
   
   parameters_file = open_file(file_name, "r");
   
   while(fgets(string, LENGTH, parameters_file)){
      sscanf(string, "%s %s", keyword, value);  
      
      if(strcmp(keyword, "mode") == 0){
         strcpy(mode, value);
      }  /* ends if */

      if(strcmp(keyword, "m") == 0){
         *m = atoi(value);
      }  /* ends if */

      if(strcmp(keyword, "n") == 0){
         *n = atoi(value);
      }  /* ends if */

      if(strcmp(keyword, "o") == 0){  
         *o = atoi(value);
      }  /* ends if */

      if(strcmp(keyword, "p") == 0){
         *p = atoi(value);
      }  /* ends if */

      if(strcmp(keyword, "data-sets") == 0){
         *data_sets = atoi(value);
      }  /* ends if */

      if(strcmp(keyword, "type") == 0){
         strcpy(type, value);
      }  /* ends if */

      if(strcmp(keyword, "in-file") == 0){
         strcpy(in_file_name, value);
      }  /* ends if */

      if(strcmp(keyword, "out-file") == 0){
         strcpy(out_file_name, value);
      }  /* ends if */

      if(strcmp(keyword, "target-file") == 0){
         strcpy(target_file_name, value);
      }  /* ends if */

      if(strcmp(keyword, "whid-file") == 0){
         strcpy(whid_file_name, value);
      }  /* ends if */

      if(strcmp(keyword, "win-file") == 0){
         strcpy(win_file_name, value);
      }  /* ends if */

      if(strcmp(keyword, "wout-file") == 0){
         strcpy(wout_file_name, value);
      }  /* ends if */
      
   }  /* ends while fgets */
   
   fclose(parameters_file);
   
}  /* ends file_input_parameters */
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/
/*PAGE ifdef test cases
*/




#ifdef MAT_MUL_TEST
   /* try h = x * y 
      x[1][3] y[3][2] = h[1][2] */

   x    = (ELEMENT  *) malloc((int)(3) * sizeof(ELEMENT));
   y    = (ELEMENT  *) malloc((int)(6) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(2) * sizeof(ELEMENT));
   x[0] = 2; x[1] = 3; x[2] = 4;
   h[0] = 0; h[1] = 0;
   initialize_2d_weights(y, 3, 2);
   matrix_multiply(x, 1, 3, y, 2, h);
   display_2d_weights(x, 1, 3);
   display_2d_weights(y, 3, 2);
   display_2d_weights(h, 1, 2);
   exit(0);      
#endif


#ifdef NEVER
   initialize_3d_weights(h, n, p, p);
   initialize_3d_weights(wh, (n-1), n, m);
   display_3d_weights(h, n, p, p);
   display_3d_weights(wh, (n-1), n, m);
#endif                      


#ifdef TEST_3D_COPY
   n = 4;
   p = 3;
   m = 2;
   whid = (ELEMENT  *) malloc((int)((n-1)*p*m) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(p*m) * sizeof(ELEMENT));
   initialize_3d_weights(whid, (n-1), p, m);
   copy_3d_to_2d(whid, h, (n-1), p, m, 2);
   display_2d_weights(h, p, m);
   display_3d_weights(whid, (n-1), p, m);
   free(whid);
   free(h);
#endif

#ifdef TEST_1D_COPY
   n = 4;
   p = 3;
   m = 2;
   whid = (ELEMENT  *) malloc((int)(p*m) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(p) * sizeof(ELEMENT));
   initialize_2d_weights(whid, p, m);
   fill_array(h, m);
   display_2_weights(h, 1, p);
   copy_1d_to_2d(h, whid, p, m, 0, "row");
   display_2d_weights(whid, p, m);
   free(whid);
   free(h);
   exit(0);
#endif           
 

#ifdef TEST_2D_COPY
   n = 4;
   p = 3;
   m = 2;
   whid = (ELEMENT  *) malloc((int)(p*m) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(p) * sizeof(ELEMENT));
   initialize_2d_weights(whid, p, m);
   fill_array(h, p);
   display_2d_weights(h, 1, p);
   copy_2d_to_1d(h, whid, p, m, 1, "col");
   display_2d_weights(whid, p, m);
   display_2d_weights(h, 1, p);
   free(whid);
   free(h);
   exit(0);
#endif           

#ifdef INNER_TEST
   p = 4;
   n = 3;
   h    = (ELEMENT  *) malloc((int)(p*n) * sizeof(ELEMENT));
   whid = (ELEMENT  *) malloc((int)((n-1)*p*p) * sizeof(ELEMENT));
   initialize_2d_weights(h, p, n);
   initialize_3d_weights(whid, (n-1), p, p);
   display_2d_weights(h, p, n);
   display_3d_weights(whid, (n-1), p, p);
   inner_layers(h, whid, p, n);
   display_2d_weights(h, p, n);
   display_3d_weights(whid, (n-1), p, p);      
   exit(0);
#endif


#ifdef INPUT_LAYER_TEST
   n = 3;
   m = 3;
   p = 4;
   x    = (ELEMENT  *) malloc((int)(m) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(p*n) * sizeof(ELEMENT));
   win  = (ELEMENT  *) malloc((int)(m*p) * sizeof(ELEMENT));
   
   initialize_2d_weights(win, m, p);
   x[0] = 2; x[1] = 3; x[2] = 4;
   fill_array(h, (p*n));
   display_2d_weights(h, p, n);   
   input_layer(h, x, win, m, p, n);
   display_2d_weights(x, 1, m);
   display_2d_weights(win, m, p);
   display_2d_weights(h, p, n);
   exit(0);
#endif


#ifdef OUTPUT_LAYER_TEST
   n = 3;
   o = 5;
   p = 4;
   y    = (ELEMENT  *) malloc((int)(o) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(p*n) * sizeof(ELEMENT));
   wout = (ELEMENT  *) malloc((int)(p*o) * sizeof(ELEMENT));
   initialize_2d_weights(wout, p, o);
   initialize_2d_weights(h, p, n);
   output_layer(h, wout, y, p, n, o);   
   display_2d_weights(h, p, n);   
   display_2d_weights(wout, p, o);
   display_2d_weights(y, 1, o);
   exit(0);
#endif

#ifdef INIT_TEST
   p = 3;
   m = 4;
   o = 5;
   n = 6;
   win  = (ELEMENT  *) malloc((int)(m*p) * sizeof(ELEMENT));
   wout = (ELEMENT  *) malloc((int)(p*o) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(p*n) * sizeof(ELEMENT));
   whid = (ELEMENT  *) malloc((int)((n-1)*p*p) * sizeof(ELEMENT));   
   initialize_2d_weights(win, p, m);
   initialize_2d_weights(wout, o, p);
   display_2d_weights(win, p, m);
   display_2d_weights(wout, o, p);
   initialize_2d_weights(h, n, p);
   initialize_3d_weights(whid, (n-1), p, p);
   display_2d_weights(h, p, n);
   display_3d_weights(whid, (n-1), p, p);
   exit(0);
#endif                      

#ifdef ACTIVATE_TEST
   ELEMENT aa, bb;
   char    type[LENGTH];
   float   aaa;
   
   /********
   printf("%.4f", logistic(3.0));
   printf("%.4f", logistic(-3.0));
   printf("%.4f", logistic(33.0));
   printf("%.4f", logistic(23.0));
   printf("%.4f", logistic(.33));
   printf("%.4f", logistic(0.0));
   **********/

      
   strcpy(type, "sigmoid");
   aa = (ELEMENT)(3.0);
   bb = activation_function(aa, type);
   printf("\n the %s of %.4f = %.4f", type, aa, bb);   

   strcpy(type, "sigmoid");
   aa = (ELEMENT)(-3.0);
   bb = activation_function(aa, type);
   printf("\n the %s of %.4f = %.4f", type, aa, bb);   
   
   strcpy(type, "sigmoid");
   aa = (ELEMENT)(.3);
   bb = activation_function(aa, type);
   printf("\n the %s of %.4f = %.4f", type, aa, bb);   

   strcpy(type, "sigmoid");
   aa = (ELEMENT)(-.3);
   bb = activation_function(aa, type);
   printf("\n the %s of %.4f = %.4f", type, aa, bb);   
   
   exit(0);   
#endif




#ifdef FORWARD_TEST
   strcpy(type, "sigmoid");
   m = 3;
   n = 5;
   o = 2;
   p = 4; 
   
   x    = (ELEMENT  *) malloc((int)(m) * sizeof(ELEMENT));
   y    = (ELEMENT  *) malloc((int)(o) * sizeof(ELEMENT)); 
   win  = (ELEMENT  *) malloc((int)(m*p) * sizeof(ELEMENT));
   wout = (ELEMENT  *) malloc((int)(p*o) * sizeof(ELEMENT));
   h    = (ELEMENT  *) malloc((int)(p*n) * sizeof(ELEMENT));
   whid = (ELEMENT  *) malloc((int)((n-1)*p*p) * sizeof(ELEMENT));
   
   x[0] = 3; 
   x[1] = -4; 
   x[2] = 5;
   initialize_2d_weights(win, m, p);
   initialize_2d_weights(wout, p, o);
   initialize_3d_weights(whid, (n-1), p, p);

   input_layer(h, x, win, m, p, n, type);
   inner_layers(h, whid, p, n, type);
   output_layer(h, wout, y, p, n, o, type);   
   
   printf("\nThis is the input array");
   display_2d_weights(x, 1, m);
   printf("\n\nThis is the win array");
   display_2d_weights(win, m, p);
   printf("\n\nThis is the h array");
   display_2d_weights(h, p, n);      
   printf("\n\nThis is the whid array");
   display_3d_weights(whid, (n-1), p, p);
   printf("\n\nThis is the output array");
   display_2d_weights(y, 1, o);
   
   exit(0);
   
#endif


#ifdef FILE_TEST 
   m = 3;
   n = 5;
   o = 2;
   p = 4; 

   strcpy(in_file_name, "input.xxx");
   strcpy(out_file_name, "output.xxx");
   strcpy(whid_file_name, "whid.xxx");
   strcpy(win_file_name, "win.xxx"); 
 
   if( (in_file = fopen(in_file_name, "w+b")) == '\0'){
      printf("\n\nERROR - cannot open input vector file\n");
      exit(0);
   }
   
   x    = (ELEMENT  *) malloc((int)(m) * sizeof(ELEMENT));

   x[0] = 3; 
   x[1] = -4; 
   x[2] = 5;
   
   fwrite(x, (m)*sizeof(ELEMENT), 1, in_file);
   
   fclose(in_file);  
   
   x[0] = FILL;
   x[1] = FILL;
   x[2] = FILL; 
   
   if( (in_file = fopen(in_file_name, "r+b")) == '\0'){
      printf("\n\nERROR - cannot open input vector file\n");
      exit(0);
   }
   
   fread(x, (m)*sizeof(ELEMENT), 1, in_file); 
   fclose(in_file);
   
   printf("\nThis is the input array");
   display_2d_weights(x, 1, m);
   
   
   free(x);
   exit(0);
   
#endif

