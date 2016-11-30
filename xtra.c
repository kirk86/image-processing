/***********************************************
*
*    file txtrsubs.c
*
*    Functions: This file contains
*
*    Purpose:
*       These functions calculate measures
*       that help distinguish textures.
*
*    External Calls:
*       wtiff.c - round_off_image_size
*                 create_file_if_needed
*                 write_array_into_tiff_image
*       tiff.c - read_tiff_header
*       rtiff.c - read_tiff_image
*       edge.c - fix_edges
*
*
*
*
*************************************************/
#include <cips.h>
/*******************************************
*
*   hurst(..
*
*   This routine performs the hurst operation.
*   Let's do this for a 7x7 area only first.
*
*   3x3 case
*       c b c
     *     d b a b d
*       c b c
*
*   5x5 case
*     f e d e f
*     e c b c e
     *     d b a b d
*     e c b c e
*     f e d e f
*
*   7x7 case
*       h g h
*     f e d e f
*   h e c b c e h
     *   g d b a b d g
*   h e c b c e h
*     f e d e f
*       h g h
*
*******************************************/

hurst(in_name, out_name, the_image, out_image,
      il, ie, ll, le, size)
   char   in_name[], out_name[];
   int    il, ie, ll, le, size;
	short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   float  x[8], y[8], sig[8];
   float  aa, bb, siga, sigb, chi2, q;
	int    ndata, mwt;

   int    a, b, count, i, j, k,
          new_hi, new_low, length,
          number, sd2, sd2p1, ss, width;
   short  *elements, max, prange;
   struct tiff_header_struct image_header;

      /**********************************************
      *
      *   Initialize the ln's of the distances.
		*   Do this one time to save computations.
      *
      **********************************************/

   x[1] = 0.0;        /* ln(1)        */
   x[2] = 0.34657359; /* ln(sqrt(2))  */
   x[3] = 0.69314718; /* ln(2)        */
   x[4] = 0.80471896; /* ln(sqrt(5))  */
   x[5] = 1.03972077; /* ln(sqrt(8))  */
   x[6] = 1.09861229; /* ln(3)        */
   x[7] = 1.15129255; /* ln(sqrt(10)) */

	sig[1] = 1.0;
	sig[2] = 1.0;
	sig[3] = 1.0;
	sig[4] = 1.0;
	sig[5] = 1.0;
	sig[6] = 1.0;
	sig[7] = 1.0;

	sd2 = size/2;

      /**********************************
      *
      *   Create out file and read
      *   input file.
      *
      ***********************************/

   create_file_if_needed(in_name, out_name, out_image);

   read_tiff_image(in_name, the_image, il, ie, ll, le);

	max = 255;
   if(image_header.bits_per_pixel == 4){
	   max = 16;
   }

      /***************************
      *
      *   Loop over image array
      *
      ****************************/

   printf("\n");
   for(i=sd2; i<ROWS-sd2; i++){
      if( (i%2) == 0) printf("%d ", i);
      for(j=sd2; j<COLS-sd2; j++){

            /*************************************
            *
            *   Go through each pixel class, set
            *   the elements array, sort it, get
            *   range, and take the ln of the
            *   range.
            *
            *************************************/

            /* b pixel class */
			number      = 4;
         elements    = (short *)
			              malloc(number * sizeof(short));
			elements[0] = the_image[i-1][j];
			elements[1] = the_image[i+1][j];
			elements[2] = the_image[i][j-1];
			elements[3] = the_image[i][j+1];
			sort_elements(elements, &number);
			prange = elements[number-1] - elements[0];
			if(prange < 0) prange = prange*(-1);
			if(prange == 0) prange = 1;
			y[1]   = log(prange);

            /* c pixel class */
			elements[0] = the_image[i-1][j-1];
			elements[1] = the_image[i+1][j+1];
			elements[2] = the_image[i+1][j-1];
			elements[3] = the_image[i-1][j+1];
			sort_elements(elements, &number);
			prange = elements[number-1] - elements[0];
			if(prange < 0) prange = prange*(-1);
			if(prange == 0) prange = 1;
			y[2]   = log(prange);

            /* d pixel class */
			elements[0] = the_image[i-2][j];
			elements[1] = the_image[i+2][j];
			elements[2] = the_image[i][j-2];
			elements[3] = the_image[i][j+2];
			sort_elements(elements, &number);
			prange = elements[number-1] - elements[0];
			if(prange < 0) prange = prange*(-1);
			if(prange == 0) prange = 1;
			y[3]   = log(prange);

            /* f pixel class */
			if(size == 5  ||  size == 7){
			elements[0] = the_image[i-2][j-2];
			elements[1] = the_image[i+2][j+2];
			elements[2] = the_image[i+2][j-2];
			elements[3] = the_image[i-2][j+2];
			sort_elements(elements, &number);
			prange = elements[number-1] - elements[0];
			if(prange < 0) prange = prange*(-1);
			if(prange == 0) prange = 1;
			y[5]   = log(prange);
			}  /* ends if size == 5 */

            /* g pixel class */
			if(size == 7){
			elements[0] = the_image[i-4][j];
			elements[1] = the_image[i+4][j];
			elements[2] = the_image[i][j-4];
			elements[3] = the_image[i][j+4];
			sort_elements(elements, &number);
			prange = elements[number-1] - elements[0];
			if(prange < 0) prange = prange*(-1);
			if(prange == 0) prange = 1;
			y[6]   = log(prange);
			}  /* ends if size == 7 */

			free(elements);

            /* e pixel class */
			if(size == 5  ||  size == 7){
			number      = 8;
         elements    = (short *)
			              malloc(number * sizeof(short));
			elements[0] = the_image[i-1][j-2];
			elements[1] = the_image[i-2][j-1];
			elements[2] = the_image[i-2][j+1];
			elements[3] = the_image[i-1][j+2];
			elements[4] = the_image[i+1][j+2];
			elements[5] = the_image[i+2][j+1];
			elements[6] = the_image[i+2][j-1];
			elements[7] = the_image[i+1][j-2];
			sort_elements(elements, &number);
			prange = elements[number-1] - elements[0];
			if(prange < 0) prange = prange*(-1);
			if(prange == 0) prange = 1;
			y[4]   = log(prange);
			}  /* ends if size == 5 */

            /* h pixel class */
			if(size == 7){
			elements[0] = the_image[i-1][j-3];
			elements[1] = the_image[i-3][j-1];
			elements[2] = the_image[i-3][j+1];
			elements[3] = the_image[i-1][j+3];
			elements[4] = the_image[i+1][j+3];
			elements[5] = the_image[i+3][j+1];
			elements[6] = the_image[i+3][j-1];
			elements[7] = the_image[i+1][j-3];
			sort_elements(elements, &number);
			prange = elements[number-1] - elements[0];
			if(prange < 0) prange = prange*(-1);
			if(prange == 0) prange = 1;
			y[7]   = log(prange);
			}  /* ends if size == 7 */

			free(elements);

            /*************************************
            *
            *   Call the fit routine to fit the
            *   data to a straight line. y=mx+b
            *   The answer you want is returned
            *   in the parameter bb.
            *
            *************************************/

         ndata = size;
         mwt   = 1;
         fit(x, y, ndata, sig, mwt, &aa, &bb,
			    &siga, &sigb, &chi2, &q);

			out_image[i][j] = bb*64;
			if(out_image[i][j] > max)
			   out_image[i][j] = max;

      }  /* ends loop over j */
   }  /* ends loop over i */


   fix_edges(out_image, sd2);

   write_array_into_tiff_image(out_name, out_image,
                               il, ie, ll, le);


}  /* ends hurst */
