    /*********************************************
    *
    *   file mainfilt.c
    *
    *   Functions: This file contains
    *      main
    *
    *   Purpose:
    *      This file contains the main calling
    *      routine in an image filtering program.
    *
    *   External Calls:
    *      gin.c - get_image_name
    *      tiff.c - read_tiff_header
    *      filter.c - filter_image
    *                 median_filter
    *                 high_pixel
    *                 low_pixel
    *
    ***********************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char     name[80], name2[80], low_high[80];
   int      count, i, ie, il, j, le, length, ll, lw,
            type, width;
   short    filter[3][3];
   struct   tiff_header_struct image_header;

   my_clear_text_screen();

       /*********************************************
       *
       *   Interpret the command line parameters.
       *
       **********************************************/

   if(argc < 5){
    printf(
    "\n\nNot enough parameters:"
     "\n"
     "\n usage: mainfilt in-file out-file type "
     " low-or-high-pass"
     "\n"
     "\n   recall type: 6, 9, 10, 16, 32 for low pass"
     "\n   recall type: 1, 2, 3 for high pass"
     "\n   recall type: is the size of the nxn area "
     "\n                for median, high pixel and"
     "\n                low pixel filtering"
     "\n   low-or-high-pass l=low pass h=high pass "
     "\n                    m=median filter"
     "\n                    i=high pixel"
     "\n                    o=low pixel"
     "\n");
    exit(0);
   }

   strcpy(name, argv[1]);
   strcpy(name2, argv[2]);
   type = atoi(argv[3]);
   strcpy(low_high, argv[4]);

   il = 1;
   ie = 1;
   ll = ROWS+1;
   le = COLS+1;

       /*********************************************
       *
       *   Read the input image header and setup
       *   the looping counters.
       *
       *   If high or low pass filtering, setup
       *   the filter mask array.
       *
       **********************************************/

   read_tiff_header(name, &image_header);

   length = (ROWS-10 + image_header.image_length)/ROWS;
   width  = (COLS-10 + image_header.image_width)/COLS;
   count  = 1;
   lw     = length*width;
   printf("\nlength=%d  width=%d", length, width);

   if(low_high[0] == 'l' ||
      low_high[0] == 'L' ||
      low_high[0] == 'h' ||
      low_high[0] == 'H')
         setup_filters(type, low_high, filter);

       /*********************************************
       *
       *   Loop over the input image and filter it
       *   using either the high or low pass filters
       *   using a mask OR using the median filter.
       *
       **********************************************/

   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
         printf("\nrunning %d of %d", count, lw);
         count++;

         if(low_high[0] == 'l' ||
            low_high[0] == 'L' ||
            low_high[0] == 'h' ||
            low_high[0] == 'H')
              filter_image(name, name2, the_image,
                           out_image, il+i*ROWS,
                           ie+j*COLS, ll+i*ROWS,
                           le+j*COLS, filter, type);

         if(low_high[0] == 'm' ||
            low_high[0] == 'M')
              median_filter(name, name2, the_image,
                            out_image, il+i*ROWS,
                            ie+j*COLS, ll+i*ROWS,
                            le+j*COLS, type);

         if(low_high[0] == 'i' ||
            low_high[0] == 'H')
              high_pixel(name, name2, the_image,
                         out_image, il+i*ROWS,
                         ie+j*COLS, ll+i*ROWS,
                         le+j*COLS, type);

         if(low_high[0] == 'o' ||
            low_high[0] == 'O')
              low_pixel(name, name2, the_image,
                        out_image, il+i*ROWS,
                        ie+j*COLS, ll+i*ROWS,
                        le+j*COLS, type);


      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends main  */
