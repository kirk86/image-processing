   /**************************************************
   *
   *   file pi.c
   *
   *   Functions: This file contains
   *       print_image
   *       print_image_array
   *       perform_printing
   *       print_column_header
   *
   *   Purpose - These functions print an image out
   *       to the line printer.  The parameters in
   *       this function are defined differently
   *       than in most other CIPS functions.
   *       The parameters il, ie, ll, le are
   *       coordinates inside the 100x100 image array.
   *       The parameters first_line and first_element
   *       are coordinates for the entire image file.
   *       So, if you want to start printing at row 10
   *       and column 10 of the image file you would
   *       call:
   *
   *       read_tiff_image(name, the_image, 10, 10,
   *                       110, 110);
   *       print_image(the_image, name, 1, 1, 1,
   *                   100, 18, 10, 10);
   *
   *       In normal print mode you can only print 17
   *       columns.
   *       le - ie = 17.
   *
   *
   *   External Calls:
   *       none
   *
   *
   **************************************************/

#include "cips.h"
#define  FORMFEED  '\014'

   /*********************************************
   *
   *   printf_image(...
   *
   *********************************************/

print_image(the_image, name, channel, il, ie, ll, le,
            first_line, first_element)
   char  name[];
   int   channel, il, ie, ll, le,
         first_line, first_element;
   short the_image[ROWS][COLS];
{
   char printer_name[MAX_NAME_LENGTH];
   FILE *printer;

   strcpy(printer_name, "prn");
   if( (printer = fopen(printer_name, "w")) == NULL)
      printf("\nPI> Could not open printer");
   else{
      printf("\nPI> The print file is opened");

           /*****************************************
           *
           *   If your printer has some form of
           *   condensed printing, you can send those
           *   commands via software using the fputc
           *   function.  For example, if your printer
           *   needs to sequence X Y Z to start
           *   condensed printing, insert the following
           *   three calls right here:
           *   fputc('X', printer);
           *   fputc('Y', printer);
           *   fputc('Z', printer);
           *
           ******************************************/

      perform_printing(printer, the_image, name,
                       channel, il, ll, ie, le,
                       first_line, first_element);
      fclose(printer);
   }  /* ends else print  */
}     /* ends print_image  */




   /*********************************************
   *
   *   perform_printing(...
   *
   *********************************************/

perform_printing(printer, the_image, name, channel,
                 il, ll, ie, le, first_line,
                 first_element)
   char  name[];
   FILE  *printer;
   int   channel, il, ie, ll, le,
         first_line, first_element;
   short the_image[ROWS][COLS];
{
   char output[3*COLS],
        response[80],
        string[3*COLS];
   int  i,
        j,
        k,
        line_counter;


   printf("\nPI> Print header");
   line_counter = 0;
   pi_long_clear_buffer(string);
   sprintf(string, "     This image is -- %s --\n",
           name);
   fputs(string, printer);
   ++line_counter;

   pi_long_clear_buffer(string);
   sprintf(string, "     The parameters are:\n");
   fputs(string, printer);
   ++line_counter;

   pi_long_clear_buffer(string);
   sprintf(string,
           "     channel=%d il=%d ll=%d ie=%d le=%d\n",
           channel, first_line, first_line+ll-2,
           first_element, first_element+le-2);
   fputs(string, printer);
   ++line_counter;

   pi_long_clear_buffer(string);
   sprintf(string, " \n");
   fputs(string, printer);
   ++line_counter;

   print_column_header(&line_counter, first_element,
                       ie, le, output, string,
                       printer);

   for(i=il; i<ll; i++){

      pi_long_clear_buffer(string);

        /* now print the image  */
      sprintf(string, "      ");
      pi_long_clear_buffer(output);
      sprintf(output, "%3d-", i+first_line-1);
      strcat(string, output);
      for(j=ie; j<le; j++){
         pi_long_clear_buffer(output);
         sprintf(output,"%4d",the_image[i][j]);
         strcat(string, output);
      }  /* ends loop over j columns */

      fputs(string, printer); fputc('\n', printer);
      line_counter = line_counter + 1;
      if(line_counter >= 53){
         line_counter = 0;
         putc(FORMFEED, printer);
         print_column_header(&line_counter,
                             first_element, ie,
                             le, output, string,
                             printer);
      }  /* ends if line_counter >=65  */
   }  /* ends loop over i rows */

   for(i=line_counter; i<66; i++){
      pi_long_clear_buffer(string);
      sprintf(string, " \n");
      fputs(string, printer);
   }

}     /* ends perform_printing  */




   /*********************************************
   *
   *   print_column_header(...
   *
   *********************************************/

print_column_header(line_counter, first_element,
                    ie, le, output,
                    string, printer)
   char string[], output[];
   FILE *printer;
   int  first_element, ie, le, *line_counter;
{
   int k;

   pi_long_clear_buffer(string);
   sprintf(string, "          ");


   for(k=first_element;k<(first_element+(le-ie));k++){
      pi_long_clear_buffer(output);
      sprintf(output, "-%3d", k);
      strcat(string, output);
   }  /* ends loop over k  */
   fputs(string, printer); fputc('\n', printer);
   *line_counter = *line_counter + 1;
}  /* ends print_column_header  */




   /*************************************************
   *
   *   print_image_array(...
   *
   *   This function prints a 100x100 image array.
   *
   **************************************************/

print_image_array(the_image)
   short the_image[ROWS][COLS];
{
   char response[80];
   printf("\nPIA> Enter a comment line\n--");
   gets(response);
                                /*     il  ie  ll    le    */
   print_image(the_image, response, 0,  1,  1, 100,  17, 0, 0);
   print_image(the_image, response, 0,  1, 18, 100,  35, 0, 0);
   print_image(the_image, response, 0,  1, 36, 100,  53, 0, 0);
   print_image(the_image, response, 0,  1, 54, 100,  71, 0, 0);
   print_image(the_image, response, 0,  1, 72, 100,  89, 0, 0);
   print_image(the_image, response, 0,  1, 90, 100, 100, 0, 0);

}  /* ends print_image_array  */




   /*********************************************
   *
   *   pi_long_clear_string(...
   *
   *********************************************/

pi_long_clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<300; i++)
      string[i] = ' ';
}
