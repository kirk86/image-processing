/*
   revup.c
   The Revision Control System

*/


#include "rcs.h"


main(argc, argv)
   int argc;
   char *argv[];
{
   char   rcs_name[80], source_name[80], string[80];
   FILE   *output_file, *rcs_file;
   int    leave_source = 0, one = 1, result, rev;
   struct stat rcs_file_status;

   if(argc <  3   ||   argc > 3){
      printf("\n\n\tusage: revup source-file #\n");
      exit(1);
   }
   strcpy(source_name, argv[1]);
   create_rcs_file_name(source_name, rcs_name);
   rev = atoi(argv[2]);

   if((rcs_file = fopen(rcs_name, "r")) == '\0'){
      printf("\nrevup>> "
               "cannot open the rcs file >>%s<<",
               rcs_name);
      exit(-1);
   }

  if((output_file = fopen("((((", "w")) == '\0'){
      printf("\nrevup>> cannot open the temp file ((((");
      exit(-1);
   }

   get_header_lines(output_file, rev);
   copy_latest_rcs_to_source(rcs_file, output_file);
   sprintf(string, "%s", DELIMETER);
   fputs(string, output_file);

   fseek(rcs_file, 0L, SEEK_SET);
   while(fgets(string, 80, rcs_file)){
      fputs(string, output_file);
   }

   fclose(rcs_file);
   fclose(output_file);

   sprintf(string, "copy (((( %s", rcs_name);
   replace_slash(string);
   system(string);
   unlink("((((");

} /* ends main */
