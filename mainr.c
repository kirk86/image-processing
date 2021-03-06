   /***********************************************
   *
   *   file mainr.c
   *
   *   Functions: This file contains
   *      main
   *
   *   Purpose:
   *      This file contains the main calling
   *      routine for a program which rotates an
   *      entire 300x300 image by 90
   *      degrees.
   *
   *   External Calls:
   *      gin.c - get_image_name
   *      numcvrt.c - get_integer
   *                  int_convert
   *      tiff.c - read_tiff_header
   *      rotate.c - rotate_flip_image_array
   *
   *
   *************************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char     name[80], name2[80];
   int      count, length, width;
   struct   tiff_header_struct image_header;

   my_clear_text_screen();

       /********************************************
       *
       *   Interpret the command line parameters.
       *
       ********************************************/

   if(argc < 3 || argc > 3){
    printf(
     "\n"
     "\n usage: mainr in-file out_file "
     "\n");
    exit(0);
   }

   strcpy(name, argv[1]);
   strcpy(name2, argv[2]);

   create_file_if_needed(name, name2, out_image);

       /**********************************************
       *
       *   Read, rotate, and write each ROWSxCOLS array
       *   in the input image.  Write them to the new
       *   locations in the output image.
       *
       ***********************************************/

   count = 1;

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 1, 1, 101, 101,
                      1, 201, 101, 301, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 1, 101, 101, 201,
                      101, 201, 201, 301, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 1, 201, 101, 301,
                      201, 201, 301, 301, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 101, 1, 201, 101,
                      1, 101, 101, 201, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 101, 101, 201, 201,
                      101, 101, 201, 201, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 101, 201, 201, 301,
                      201, 101, 301, 201, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 201, 1, 301, 101,
                      1, 1, 101, 101, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 201, 101, 301, 201,
                      101, 1, 201, 101, 1);

   printf(" %d", count++);
   rotate_flip_image_array(name, name2, the_image,
                      out_image, 201, 201, 301, 301,
                      201, 1, 301, 101, 1);

}  /* ends main  */
