

       /*********************************************
       *
       *       file d:\cips\mainedge.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *          This file contains the main calling
       *          routine in an edge detection program.
       *
       *       External Calls:
       *          gin.c - get_image_name
       *          tiff.c - read_tiff_header
       *          edge.c - quick_edge
       *                   detect_edges
       *          edge2.c - homogeneity
       *                    range
       *                    variance
       *                    difference_edge
       *                    contrast_edge
       *          edge3.c - gaussian_edge
       *                    enhance_edges
       *
       *       Modifications:
       *          2 February 1991 - created
       *          1 January 1993 - added calls to
       *             range and variance functions.
       *
       ***********************************************/

#include "cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{
   char name[80], name2[80];
   int  count, i, ie, il, j, le, length, ll, size,
        t, type, v, width;


   struct   tiff_header_struct image_header;

   my_clear_text_screen();


   if(argc < 7){
    printf("\n\nNot enough parameters:");
    printf("\n");
    printf("\n   usage: mainedge   in-file   out-file   type ");
    printf("  threshold?   threshold-value   size");
    printf("\n   recall type: 1-Prewitt     2-Kirsch        3-Sobel");
    printf("\n                4-quick       5-homogeneity   6-difference");
    printf("\n                7-contrast    8-gaussian      9-enhance");
    printf("\n                10-range      11-variance              ");
    printf("\n   threshold?   1-threshold on   2-threshold off\n");
    exit(0);
   }

   strcpy(name, argv[1]);
   strcpy(name2, argv[2]);
   type = atoi(argv[3]);
   t    = atoi(argv[4]);
   v    = atoi(argv[5]);
   size = atoi(argv[6]);

   il = 1;
   ie = 1;
   ll = ROWS+1;
   le = COLS+1;

   read_tiff_header(name, &image_header);

   length = (ROWS-10 + image_header.image_length)/ROWS;
   width  = (COLS-10 +image_header.image_width)/COLS;
   count  = 1;
   printf("\nlength=%d  width=%d", length, width);

   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
        printf("\nrunning %d of %d", count, length*width);
        count++;
        if(type == 11)
           variance(name, name2, the_image, out_image,
                    il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                    le+j*COLS, t, v);
        if(type == 10)
           range(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, size, t, v);
        if(type == 9)
           enhance_edges(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, v);
        if(type == 8)
           gaussian_edge(name, name2, the_image, out_image,
                         il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                         le+j*COLS, size, t, v);
        if(type == 7)
           contrast_edge(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, t, v);
        if(type == 6)
           difference_edge(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, t, v);
        if(type == 5)
           homogeneity(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, t, v);
        if(type == 4)
           quick_edge(name, name2, the_image, out_image,
                      il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                      le+j*COLS, t, v);
        if(type == 3  ||  type == 2  ||  type == 1)
           detect_edges(name, name2, the_image, out_image,
                        il+i*ROWS, ie+j*COLS, ll+i*ROWS,
                        le+j*COLS, type, t, v);
      }
   }

}  /* ends main  */
