/*
   prunerev.c
   The Revision Control System

*/


#include "rcs.h"


main(argc, argv)
   int argc;
   char *argv[];
{
   char rcs_name[80], source_name[80], string[80];
   FILE *rcs_file, *source_file, *xxx_file;
   int  prune = 0, leave_source = 0, one = 1, result,
        rev;

   if(argc < 3    ||   argc > 3){
      printf("\n\n\tusage: "
             "prunerev source-file rev-number\n");
      exit(-1);
   }

   prune = print_warning();
   if(prune == 0){
      printf("\nExiting prunerev\n");
      exit(1);
   }

   strcpy(source_name, argv[1]);
   strcpy(string, argv[2]);
   rev = rev_number(string);

   create_rcs_file_name(source_name, rcs_name);

   if((rcs_file = fopen(rcs_name, "r")) == '\0'){
      printf("\nprunerev>> "
             "cannot open the rcs file >>%s<<",
        rcs_name);
      exit(-1);
   }

   if((xxx_file = fopen("((((", "w")) == '\0'){
      printf("\nprunerev>> "
             "cannot open the temp file ((((");
      exit(-1);
   }

   copy_wanted_revs_to_temp_file(rcs_file, xxx_file, rev);

   fclose(rcs_file);
   fclose(xxx_file);

   unlink(rcs_name);
   sprintf(string, "copy (((( %s", rcs_name);
   replace_slash(string);
   system(string);
   unlink("((((");

}


/*
   copy_wanted_revs_to_temp_file(rcs_file, xxx_file, rev)
*/

copy_wanted_revs_to_temp_file(FILE *rcs_file,
               FILE *xxx_file,
               int rev)
{
   char *result, second_string[80], string[80];
   int  found_it = 0, new_rev, reading = 1,
        still_reading = 1;

   reading = 1;
   while(reading){ /* read file */
      result = fgets(string, 80, rcs_file);

      if( strncmp(string, FIRST_LINE, 5) == 0){
         result  = fgets(second_string, 80, rcs_file);
         new_rev = atoi(second_string);

         if(rev == new_rev){
            still_reading = 1;

            while(still_reading){
               found_it = 1;
               result   = fgets(string, 80, rcs_file);
               if( strncmp(string, FIRST_LINE, 5) == 0){
                  fputs(string, xxx_file);
                  still_reading = 0;
                  found_it      = 1;
               }  /* ends if FIRST_LINE*/
               if(result == '\0') still_reading = 0;
            }  /* ends while still_reading */

         }  /* ends if rev == new_rev */
         else{ /* else rev != new_rev */
            fputs(string, xxx_file);
            fputs(second_string, xxx_file);
         }

      }  /* ends if FIRST_LINE */
      else /* not FIRST_LINE so fputs it out */
         fputs(string, xxx_file);

      if(result == '\0') reading = 0;
   } /* ends while reading */

   if(found_it == 0){
      printf("\n\nprunerev>> Did not find the"
        " desired revision\n");
      fclose(rcs_file);
      exit(-5);
   }

}  /* ends copy_wanted_revs_to_temp_file */


/*
         print_warning()
*/

print_warning()
{
   char *string;
   printf("\n"
   "\nWARNING: If you continue, you will delete part of"
   "\n         your RCS file.  You will never be able to"
   "\n         retrieve those deleted revisions."
   "\n"
   "\n         Do you want to continue (y/n) ? _\b");
   fgets(string, 10, stdin);
   if(string[0] == 'y'  || string[0] == 'Y')
      return(1);
   else
      return(0);
}
