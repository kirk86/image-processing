/*
   checkin.c
   The Revision Control System

   Dwayne Phillips
   November 1991
*/


#include "rcs.h"


main(argc, argv)
   int argc;
   char *argv[];
{
   char   rcs_name[80], source_name[80], string[80];
   FILE   *rcs_file, *source_file, *xxx_file;
   int    leave_source = 0, one = 1, result, rev;
   struct stat rcs_file_status;

   if( (argc <  2) || (argc > 3)){
      printf("\n\n\tusage: checkin [-l] source-file\n");
      exit(1);
   }

   if(argc == 2){
      strcpy(source_name, argv[1]);
      if((source_file = fopen(source_name, "r")) == '\0'){
         printf("\ncheckin>> "
                "cannot open the source file >>%s<<",
                source_name);
         exit(-1);
      }
   }

   if(argc == 3){
      if( (strncmp("-l", argv[1], 2) == 0)){
         leave_source = 1;
         strcpy(source_name, argv[2]);
         if((source_file = fopen(source_name, "r")) == '\0'){
            printf("\ncheckin>> "
                   "cannot open the source file >>%s<<",
                   source_name);
            exit(-1);
         }
      }
      else{
         printf("\n\n\tusage: checkin [-l] source-file\n");
         exit(3);
      }
   }  /* ends if argc == 3 */

   create_rcs_file_name(source_name, rcs_name);

      /*  does an rcs file already exist? */

   result = stat(rcs_name, &rcs_file_status);

   if(result == -1){ /* rcs file does not yet exist */
      if((rcs_file = fopen(rcs_name, "w")) == '\0'){
         printf("\ncheckin>> "
                "cannot create the rcs file >>%s<<",
                rcs_name);
         exit(-1);
      }
      else{
         get_header_lines(rcs_file, one);
         copy_source_to_rcs(source_file, rcs_file);
         fclose(rcs_file);
         fclose(source_file);
         if(leave_source == 0)
            unlink(source_name);
      }  /* ends else we can create the rcs file */
   }  /* ends if result == -1 */

   if(result == 0){ /* rcs file already exists */
      if((rcs_file = fopen(rcs_name, "r")) == '\0'){
         printf("\ncheckin>> "
                "cannot open the rcs file >>%s<<",
                rcs_name);
         exit(-1);
      }
      get_latest_revision_number(rcs_file, &rev);
      rev++;
      fseek(rcs_file, 0L, SEEK_SET);

      if((xxx_file = fopen("RCS/((((", "w")) == '\0'){
         printf("\ncheckin>> "
                "cannot create the temp file ((((");
         exit(-1);
      }
      get_header_lines(xxx_file, rev);

      copy_source_to_rcs(source_file, xxx_file);
      while( fgets(string, 80, rcs_file) != '\0')
         fputs(string, xxx_file);

      fclose(xxx_file);
      fclose(rcs_file);
      fclose(source_file);
      unlink(rcs_name);
      if(leave_source == 0)
         unlink(source_name);

      sprintf(string, "copy RCS/(((( %s", rcs_name);
      replace_slash(string);
      system(string);
      unlink("RCS/((((");

   }  /* ends if result == 0 */

}  /* ends main  */




/*
   copy_source_to_rcs(FILE *source_file, FILE *rcs_file)
*/

copy_source_to_rcs(FILE *source_file, FILE *rcs_file)
{
   char line[80];
   while( fgets(line, 80, source_file) != '\0'){
    fputs(line, rcs_file);
   }
   fputs(DELIMETER, rcs_file);
}



/*
   get_latest_revision_number(FILE *rcs_file, int *rev)
*/


get_latest_revision_number(FILE *rcs_file, int *rev)
{
   char *line;
   fgets(line, 80, rcs_file);
   fgets(line, 80, rcs_file);
   *rev = atoi(line);
}
