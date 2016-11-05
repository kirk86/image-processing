/*
     rcsutil.c
     The Revision Control System

     This file contains functions that are used by
     several of the RCS programs.  They are held here
     to avoid having multiple copies of the same
     function.

     create_rcs_file_name
     rev_number
     replace_slash
     go_to_correct_rev
     copy_rcs_to_source
     copy_latest_rcs_to_source
     get_header_lines
*/

#include "rcs.h"

/*
   create_rcs_file_name(char source_name[],
                       char rcs_name[])
*/

create_rcs_file_name(char *source_name, char *rcs_name)
{
   char *place, temp[80];
   int  i, j, slash, slot;

   slash = 47;
   place = strrchr(source_name, slash);
   slash = 92;
   if(place == '\0')
         place = strrchr(source_name, slash);
   if(place == '\0'){
      strcpy(rcs_name, "RCS/v");
      strcat(rcs_name, source_name);
   }
   else{
      slot = place - source_name;
      strncpy(temp, source_name, slot);
      temp[slot] = '\0';
      strcat(temp, "/RCS/v");
      slot++;
      j = strlen(temp);
      for(i=slot; source_name[i]!='\0'; i++){
         temp[j] = source_name[i];
         j++;
      }

      temp[j] = '\0';
      strcpy(rcs_name, temp);
   }
}



/*
   rev_number(char *string)
*/

rev_number(char *string)
{
   char  *new_string;
   int   doit = 0, i, j, result;

   i = 0;
   while(doit == 0){
      j = string[i];
      doit = isdigit(j);
      i++;
   }
   i--;

   for(j=0; string[i] != '\0'; i++, j++)
         new_string[j] = string[i];
   result = atoi(new_string);
   return(result);

}


/*
   replace_slash(char string[])
*/

replace_slash(char string[])
{
   int  slash      = 47, back_slash = 92, i, j;

   j = strlen(string);
   for(i=0; i<j; i++){
      if(string[i] == slash) string[i] = back_slash;
   }
}




/*
   go_to_correct_rev(FILE *rcs_file, int rev)
*/

go_to_correct_rev(FILE *rcs_file, int rev)
{
   char *result, string[80];
   int  found_it = 0, new_rev, reading = 1, still_reading = 1;

   while(reading){ /* read file */
      result = fgets(string, 80, rcs_file);
      if( strncmp(string, FIRST_LINE, 5) == 0){
         result = fgets(string, 80, rcs_file);
         new_rev = atoi(string);
         if(rev == new_rev){
            while(still_reading){
               result = fgets(string, 80, rcs_file);
               if( strncmp(string, DELIMETER, 5) == 0){
                  still_reading = 0;
                  reading       = 0;
                  found_it      = 1;
               }  /* ends if DELIMETER */
               if(result == '\0') still_reading = 0;
            }  /* ends while still_reading */
         }  /* ends if rev == new_rev */
      }  /* ends if FIRST_LINE */
      if(result == '\0') reading = 0;
   } /* ends while reading */

   if(found_it == 0){
      printf("\n\ncheckout.c> Did not find the"
             " desired revision\n");
      fclose(rcs_file);
      exit(-5);
   }
}



/*
   copy_rcs_to_source(FILE *rcs_file, FILE *source_file)
*/

copy_rcs_to_source(FILE *rcs_file, FILE *source_file)
{
   char string[80];
   int  reading = 1;

   while(reading){
      fgets(string, 80, rcs_file);
      if( strncmp(string, DELIMETER, 5) == 0)
         reading = 0;
      else
         fputs(string, source_file);
   }
}



/*
   copy_latest_rcs_to_source(FILE *rcs_file, FILE *source_file)
*/

copy_latest_rcs_to_source(FILE *rcs_file, FILE *source_file)
{
   char string[80];
   int  reading = 1;

   while(reading){
      fgets(string, 80, rcs_file);
         if( strncmp(string, DELIMETER, 5) == 0){
            while(reading){
               fgets(string, 80, rcs_file);
                  if(strncmp(string, DELIMETER, 5) == 0)
                     reading = 0;
                  else
                     fputs(string, source_file);
            } /* ends while reading */
         }  /* ends if DELIMETER */
   }  /* ends while reading */
}




/*
   get_header_lines(FILE *the_file, int version)
*/

get_header_lines(FILE *the_file, int version)
{
   char   string[80];
   int    entering = 1;
   time_t ltime;

   time(&ltime);

   fputs(FIRST_LINE, the_file);
   sprintf(string, "%d\n", version);
   fputs(string, the_file);
   sprintf(string, "%s", ctime(&ltime));
   fputs(string, the_file);

   printf("\n\nEnter your header lines");
   printf("\nStop the header lines by entering");
   printf("\na . on a line by itself.");
   printf("\n");
   while(entering){
      printf(">>");
      fgets(string, 80, stdin);
      if(string[0] != '.')
       fputs(string, the_file);
      else
       entering = 0;
   } /* ends while entering */

   fputs(DELIMETER, the_file);

}
