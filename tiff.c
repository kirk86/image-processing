       /*********************************************
       *
       *   file tiff.c
       *
       *   Functions: This file contains
       *     read_tiff_header
       *     extract_long_from_buffer
       *     extract_short_from_buffer
       *
       *   Purpose:
       *     This file contains the subroutines that
       *     read the tiff files header information.
       *
       *   External Calls:
       *      none
       *
       *
       ************************************************/

#include "cips.h"


       /***********************************************
       *
       *   read_tiff_header(...
       *
       *   This function reads the header of a TIFF
       *   file and places the needed information into
       *   the struct tiff_header_struct.
       *
       ***********************************************/

read_tiff_header(file_name, image_header)
   char file_name[];
   struct tiff_header_struct *image_header;
{
   char buffer[12], response[80];

   FILE *image_file;

   int  bytes_read,
        closed,
        i,
        j,
        lsb,
        not_finished,
        position;

   long bits_per_pixel,
        image_length,
        image_width,
        length_of_field,
        offset_to_ifd,
        strip_offset,
        subfile,
        value;

   short entry_count,
         field_type,
         s_bits_per_pixel,
         s_image_length,
         s_image_width,
         s_strip_offset,
         tag_type;

   image_file = fopen(file_name, "rb");
   if(image_file != NULL){

        /*************************************
        *
        *   Determine if the file uses MSB
        *   first or LSB first
        *
        *************************************/

   bytes_read = fread(buffer, 1, 8, image_file);

   if(buffer[0] == 0x49)
      lsb = 1;
   else
      lsb = 0;

        /*************************************
        *
        *   Read the offset to the IFD
        *
        *************************************/

   extract_long_from_buffer(buffer, lsb, 4,
                            &offset_to_ifd);

   not_finished = 1;
   while(not_finished){

        /*************************************
        *
        *   Seek to the IFD and read the
        *   entry_count, i.e. the number of
        *   entries in the IFD.
        *
        *************************************/

      position   = fseek(image_file, offset_to_ifd,
                         SEEK_SET);
      bytes_read = fread(buffer, 1, 2, image_file);
      extract_short_from_buffer(buffer, lsb, 0,
                                &entry_count);

        /***************************************
        *
        *   Now loop over the directory entries.
        *   Look only for the tags we need.  These
        *   are:
        *     ImageLength
        *     ImageWidth
        *     BitsPerPixel(BitsPerSample)
        *     StripOffset
        *
        *****************************************/

      for(i=0; i<entry_count; i++){
       bytes_read = fread(buffer, 1, 12, image_file);
       extract_short_from_buffer(buffer, lsb, 0,
                                 &tag_type);

       switch(tag_type){

          case 255: /* Subfile Type */
             extract_short_from_buffer(buffer, lsb, 2,
                                       &field_type);
             extract_short_from_buffer(buffer, lsb, 4,
                                    &length_of_field);
             extract_long_from_buffer(buffer, lsb, 8,
                                      &subfile);
             break;

          case 256: /* ImageWidth */
             extract_short_from_buffer(buffer, lsb, 2,
                                       &field_type);
             extract_short_from_buffer(buffer, lsb, 4,
                                    &length_of_field);
             if(field_type == 3){
              extract_short_from_buffer(buffer, lsb, 8,
                                     &s_image_width);
              image_width = s_image_width;
             }
             else
              extract_long_from_buffer(buffer, lsb, 8,
                                       &image_width);
             break;

          case 257: /* ImageLength */
             extract_short_from_buffer(buffer, lsb, 2,
                                       &field_type);
             extract_short_from_buffer(buffer, lsb, 4,
                                    &length_of_field);
             if(field_type == 3){
              extract_short_from_buffer(buffer, lsb, 8,
                                    &s_image_length);
              image_length = s_image_length;
             }
             else
              extract_long_from_buffer(buffer, lsb, 8,
                                       &image_length);
             break;

          case 258: /* BitsPerSample */
             extract_short_from_buffer(buffer, lsb, 2,
                                       &field_type);
             extract_short_from_buffer(buffer, lsb, 4,
                                    &length_of_field);
             if(field_type == 3){
              extract_short_from_buffer(buffer, lsb, 8,
                                   &s_bits_per_pixel);
              bits_per_pixel = s_bits_per_pixel;
             }
             else
              extract_long_from_buffer(buffer, lsb, 8,
                                    &bits_per_pixel);
             break;

          case 273: /* StripOffset */
             extract_short_from_buffer(buffer, lsb, 2,
                                       &field_type);
             extract_short_from_buffer(buffer, lsb, 4,
                                    &length_of_field);
             if(field_type == 3){
              extract_short_from_buffer(buffer, lsb, 8,
                                    &s_strip_offset);
              strip_offset = s_strip_offset;
             }
             else
              extract_long_from_buffer(buffer, lsb, 8,
                                       &strip_offset);
             break;

          default:
             break;

       }  /* ends switch tag_type */

      }  /* ends loop over i directory entries */

      bytes_read = fread(buffer, 1, 4, image_file);
      extract_long_from_buffer(buffer, lsb, 0,
                               &offset_to_ifd);
      if(offset_to_ifd == 0) not_finished = 0;

   }  /* ends while not_finished */


   image_header->lsb                = lsb;
   image_header->bits_per_pixel = bits_per_pixel;
   image_header->image_length       = image_length;
   image_header->image_width        = image_width;
   image_header->strip_offset       = strip_offset;

   closed = fclose(image_file);
   }  /* ends if file opened ok */
   else{
      printf("\n\nTIFF.C> ERROR - could not open "
             "tiff file");
   }
}  /* ends read_tiff_header */





   /****************************************
   *
   *   extract_long_from_buffer(...
   *
   *   This takes a four byte long out of a
   *   buffer of characters.
   *
   *   It is important to know the byte order
   *   LSB or MSB.
   *
   ****************************************/

extract_long_from_buffer(buffer, lsb, start, number)
   char  buffer[];
   int       lsb, start;
   long  *number;
{
   int i;
   union long_char_union lcu;

   if(lsb == 1){
      lcu.l_alpha[0] = buffer[start+0];
      lcu.l_alpha[1] = buffer[start+1];
      lcu.l_alpha[2] = buffer[start+2];
      lcu.l_alpha[3] = buffer[start+3];
   }  /* ends if lsb = 1 */

   if(lsb == 0){
      lcu.l_alpha[0] = buffer[start+3];
      lcu.l_alpha[1] = buffer[start+2];
      lcu.l_alpha[2] = buffer[start+1];
      lcu.l_alpha[3] = buffer[start+0];
   }  /* ends if lsb = 0      */

   *number = lcu.l_num;


}  /* ends extract_long_from_buffer */






extract_ulong_from_buffer(buffer, lsb, start, number)
   char  buffer[];
   int       lsb, start;
   unsigned long  *number;
{
   int i;
   union ulong_char_union lcu;

   if(lsb == 1){
      lcu.l_alpha[0] = buffer[start+0];
      lcu.l_alpha[1] = buffer[start+1];
      lcu.l_alpha[2] = buffer[start+2];
      lcu.l_alpha[3] = buffer[start+3];
   }  /* ends if lsb = 1 */

   if(lsb == 0){
      lcu.l_alpha[0] = buffer[start+3];
      lcu.l_alpha[1] = buffer[start+2];
      lcu.l_alpha[2] = buffer[start+1];
      lcu.l_alpha[3] = buffer[start+0];
   }  /* ends if lsb = 0      */

   *number = lcu.l_num;
}  /* ends extract_ulong_from_buffer */




   /****************************************
   *
   *   extract_short_from_buffer(...
   *
   *   This takes a two byte short out of a
   *   buffer of characters.
   *
   *   It is important to know the byte order
   *   LSB or MSB.
   *
   ****************************************/

extract_short_from_buffer(buffer, lsb, start, number)
   char  buffer[];
   int       lsb, start;
   short *number;
{

   int i;
   union short_char_union lcu;

   if(lsb == 1){
      lcu.s_alpha[0] = buffer[start+0];
      lcu.s_alpha[1] = buffer[start+1];
   }  /* ends if lsb = 1 */

   if(lsb == 0){
      lcu.s_alpha[0] = buffer[start+1];
      lcu.s_alpha[1] = buffer[start+0];
   }  /* ends if lsb = 0      */

   *number = lcu.s_num;


}  /* ends extract_short_from_buffer */





extract_ushort_from_buffer(buffer, lsb, start, number)
   char  buffer[];
   int       lsb, start;
   unsigned short *number;
{

   int i;
   union ushort_char_union lcu;

   if(lsb == 1){
      lcu.s_alpha[0] = buffer[start+0];
      lcu.s_alpha[1] = buffer[start+1];
   }  /* ends if lsb = 1 */

   if(lsb == 0){
      lcu.s_alpha[0] = buffer[start+1];
      lcu.s_alpha[1] = buffer[start+0];
   }  /* ends if lsb = 0      */

   *number = lcu.s_num;
}  /* ends extract_ushort_from_buffer */
