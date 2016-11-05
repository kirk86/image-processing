/*
   revdiff.c
   The Revision Control System

*/


#include "rcs.h"



main(argc, argv)
   int argc;
   char *argv[];
{
   char   output_name[80], rcs_name[80], source_name[80],
          string[80];
   FILE   *output_file, *output2_file, *rcs_file,
          *source_file;
   int    extra_file = 0, rev = 0, rev2 = 0;

   if((argc <  2) || (argc > 4)){
      printf("\n\n\tusage: revdiff [-r#] [-r#] source-file\n");
      exit(1);
   }

      /* copy latest revision to a temp file
         and run fc on the source file and temp file */
   if(argc == 2){
      strcpy(source_name, argv[1]);
      rev = 0;
   }
          /* revdiff -r# source-file */
   if(argc == 3){
      if( (strncmp(argv[1], "-r", 2) == 0)){
         strcpy(string, argv[1]);
         strcpy(source_name, argv[2]);
         rev = rev_number(string);
      }
      else{
         printf("\n\n\tusage: revdiff [-r#] [-r#] source-file\n");
         exit(1);
      }
   }

         /* revdiff -r#1 -r#2 source-file */
   if(argc == 4){
      if(  (strncmp(argv[1], "-r", 2) == 0)  &&
           (strncmp(argv[2], "-r", 2) == 0)){
         extra_file = 1;
         strcpy(string, argv[1]);
         rev  = rev_number(string);
         strcpy(string, argv[2]);
         rev2 = rev_number(string);
         strcpy(source_name, argv[3]);
      }
      else{
         printf("\n\n\tusage: revdiff [-r#] [-r#] source-file\n");
         exit(1);
      }
   }

   create_rcs_file_name(source_name, rcs_name);

   if((rcs_file = fopen(rcs_name, "r")) == '\0'){
      printf("\nrevdiff>> "
             "cannot open the rcs file >>%s<<",
             rcs_name);
      exit(-1);
   }

   if((output_file = fopen("((((", "w")) == '\0'){
      printf("\nrevdiff>> cannot open the temp file ((((");
      exit(-1);
   }

   if(extra_file == 1){
      if((output2_file = fopen("((((2", "w")) == '\0'){
         printf("\nrevdiff>> cannot open the temp file ((((2");
         exit(-1);
      }
   }

   if(extra_file == 0){
      if(rev == 0)
         copy_latest_rcs_to_source(rcs_file, output_file);
      else{
         go_to_correct_rev(rcs_file, rev);
         copy_rcs_to_source(rcs_file, output_file);
      }
   }
   else{
      go_to_correct_rev(rcs_file, rev);
      copy_rcs_to_source(rcs_file, output_file);
      fseek(rcs_file, 0L, SEEK_SET);
      go_to_correct_rev(rcs_file, rev2);
      copy_rcs_to_source(rcs_file, output2_file);
   }

   fclose(rcs_file);
   fclose(output_file);
   if(extra_file == 1)
      fclose(output2_file);

      /* Use DOS's fc to compare files */
   if(extra_file == 0){
      replace_slash(source_name);
      sprintf(string, "fc /L /N %s ((((", source_name);
      system(string);
   }
   else{
      sprintf(string, "fc (((( ((((2");
      system(string);
   }

   unlink("((((");
   if(extra_file == 1)
      unlink("((((2");

}
