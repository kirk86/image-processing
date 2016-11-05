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
   *       External Calls:
   *          imageio.c
   *             create_allocate_tif_file
   *             create_allocate_bmp_file
   *
   *
   *************************************************/

#include "cips.h"
#include <conio.h>


main(argc, argv)
   int  argc;
   char *argv[];
{
   char   x, y, z;
   char   *cc;
   int    l, w;
   int    ok = 0;
   int    a, b;
   struct tiff_header_struct image_header;
   struct bmpfileheader      bmp_file_header;
   struct bitmapheader       bmheader;

   a = getch();
   printf("\n-%x-", a);


}
