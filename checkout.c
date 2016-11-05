/*
   checkout.c
   The Revision Control System

   Dwayne Phillips
   November1991
*/


#include "rcs.h"


main(argc, argv)
   int argc;
   char *argv[];
{
   char   output_name[80], rcs_name[80], source_name[80],
          string[80];
   FILE   *output_file, *rcs_file, *source_file;
   int    extra_file = 0, rev = 0;

   if(argc <  2   ||   argc > 4){
      printf("\n\n\tusage: checkout [-r#] source-file"
             " [output-file]\n");
      exit(1);
   }

   if(argc == 2){
      strcpy(source_name, argv[1]);
      rev = 0;
   }

   if(argc == 3){
          /* checkout -r# source-name */
      if( (strncmp(argv[1], "-r", 2) == 0)){
         strcpy(string, argv[1]);
         strcpy(source_name, argv[2]);
         rev = rev_number(string);
      }

          /* checkout source-name output-name */
      if( (strncmp(argv[1], "-r", 2) != 0)){
         extra_file = 1;
         rev        = 0;
         strcpy(source_name, argv[1]);
         strcpy(output_name, argv[2]);
      }
   }  /* ends if argc == 3 */


      /* checkout -r# source-name output-name */
   if(argc == 4){
      if( (strncmp(argv[1], "-r", 2) != 0)){
         printf("\n\n\tusage: checkout [-r#] source-file"
                " [output-file]\n");
         exit(1);
      }
      else{
         extra_file = 1;
         rev        = rev_number(argv[1]);
         strcpy(source_name, argv[2]);
         strcpy(output_name, argv[3]);
      }
   }

   create_rcs_file_name(source_name, rcs_name);

   if((rcs_file = fopen(rcs_name, "r")) == '\0'){
      printf("\ncheckout>> "
             "cannot open the rcs file >>%s<<",
             rcs_name);
      exit(-1);
   }

   if(extra_file == 1){ /* open output_name for writing */
      if((output_file = fopen(output_name, "w")) == '\0'){
         printf("\ncheckout>> "
                "cannot open the output file >>%s<<",
                output_name);
         exit(-1);
      }
   }  /* ends if extra_file == 1 */

   else{ /* else open source_name for writing */
      if((source_file = fopen(source_name, "w")) == '\0'){
         printf("\ncheckout>> "
                "cannot open the source file >>%s<<",
                source_name);
         exit(-1);
      }
   }  /* ends else extra_file == 0 */

   if(extra_file == 1){  /* use output file */
      if(rev == 0)
         copy_latest_rcs_to_source(rcs_file, output_file);
      else{
         go_to_correct_rev(rcs_file, rev);
         copy_rcs_to_source(rcs_file, output_file);
      }
   }
   else{  /* else use source_file */
      if(rev == 0)
         copy_latest_rcs_to_source(rcs_file, source_file);
      else{
         go_to_correct_rev(rcs_file, rev);
         copy_rcs_to_source(rcs_file, source_file);
      }
   }

   fclose(rcs_file);
   if(extra_file == 1)
      fclose(output_file);
   else
      fclose(source_file);

}
