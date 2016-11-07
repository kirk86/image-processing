/************************************************
*
*       file create.c
*
*       Functions: This file contains
*           main
*
*       Purpose:
*          This program creates an 8 bit tiff file
*          of size l*ROWS by w*COLS.
*
*************************************************/

#include <cips.h>


int main(int argc, char *argv[])
   int  argc;
   char *argv[];
{
   char   *cc;
   int    l, w;
   int    ok = 0;
   struct tiff_header_struct image_header;
   struct bmpfileheader      bmp_file_header;
   struct bitmapheader       bmheader;

   if(argc < 4 || argc > 4){
      printf(
      "\nusage: create file-name length width\n");
      exit(-1);
   }

   l = atoi(argv[2]);
   w = atoi(argv[3]);

   cc = strstr(argv[1], ".tif");
   if(cc != NULL){  /* create a tif */
      ok = 1;
      image_header.lsb            = 1;
      image_header.bits_per_pixel = 8;
      image_header.image_length   = l;
      image_header.image_width    = w;;
      image_header.strip_offset   = 1000;
      create_allocate_tiff_file(argv[1],
                                &image_header);
   }  /* ends tif */

   cc = strstr(argv[1], ".bmp");
   if(cc != NULL){  /* create a bmp */
      ok = 1;
      bmheader.height = l;
      bmheader.width  = w;
      create_allocate_bmp_file(argv[1],
                               &bmp_file_header,
                               &bmheader);
   }  /* ends tif */

   if(ok == 0){
      printf("\nERROR input file neither tiff nor bmp");
      exit(0);
   }

}
