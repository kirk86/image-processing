

#include "d:\c600\include\stdio.h"
#include "d:\c600\include\stdlib.h"
#include "d:\c600\include\malloc.h"

#define GND '\0';

char *BLANKS       = "     \n";
char *SECTION      = "^*";
char *MODULE       = "^ ";
/*char *TEXT         = "^"; may delete this one ???*/
char *START_CODE   = "^c";
char *END_CODE     = "^ec";
char *START_DEFINE = "^d";
char *END_DEFINE   = "^ed";
char *START_UNIT   = "^<";
char *END_UNIT     = ">^";
char *CODE_IS      = ">^="; /* source code follows */
char *SUB_UNIT_IS  = ">^%"; /* sub unit follows */
char *END_SUB_UNIT = ">^$"; /* no more subs here */
int  LL            = 80;

struct toc_struct
{
   char   title[80];
   int    section;
   struct toc_struct *pointer;
};

struct code_struct
{
   char   name[80];
   struct code_struct *next_unit;
   struct code_struct *sub_unit;
};




main(argc, argv)
   char *argv[];
   int  argc;
{
   char code[80],
        code_int[80],
        extra[80],
        inter[80],
        lpl[80],
        text[80];

   FILE *code_file,
        *code_int_file,
        *inter_file,
        *lpl_file,
        *text_file;

   int  section = 1;
   struct toc_struct   *head;
   struct toc_struct   *create_entry();

   struct code_struct  *code_head;
   struct code_struct  *create_code_unit();

   head      = '\0';
   code_head = '\0';

   if(argc < 2){
      printf("\n\n\tusage: alpl input_file.lpl\n\n");
      exit(1);
   }

   strcpy(lpl, argv[1]);
   if((lpl_file = fopen(lpl, "rt")) == '\0'){
      printf("\n\nERROR - cannot open input file %s", lpl);
      exit(2);
   }

   create_file_name(lpl, code, "c");
   create_file_name(lpl, inter, "int");
   create_file_name(lpl, text, "txt");
   create_file_name(lpl, code_int, "cin");
printf("\ncode=%s  text=%s  inter=%s  code int=%s", 
code, text, inter, code_int);

   if((code_file = fopen(code, "wt")) == '\0'){
      printf("\n\nERROR - cannot create code file %s", code);
      exit(3);
   }
   if((inter_file = fopen(inter, "wt")) == '\0'){
      printf("\n\nERROR - cannot create inter file %s", inter);
      exit(4);
   }
   if((text_file = fopen(text, "wt")) == '\0'){
      printf("\n\nERROR - cannot create text file %s", text);
      exit(5);
   }
   if((code_int_file = fopen(code_int, "wt")) == '\0'){
      printf("\n\nERROR - cannot create code int file %s", code_int);
      exit(6);
   }

      /* FIRST PASS */
   while(fgets(lpl, LL, lpl_file)){

      if(strncmp(lpl, MODULE, strlen(MODULE)) == 0){
         module_case(lpl_file, inter_file,
                     lpl, inter, &section);
      }

      else if(strncmp(lpl, SECTION, strlen(SECTION)) == 0){
         strcpy(extra, lpl);
         remove_markers(extra, strlen(SECTION));
         head = create_entry(head, extra, section);
         section_case(lpl_file, inter_file,
                      lpl, inter, &section);
      }

      /**************
      else if(strncmp(lpl, TEXT, strlen(TEXT)) == 0){
         printf("\nmatched TEXT");
      }
      **************/

      else if(strncmp(lpl, START_CODE, strlen(START_CODE)) == 0){
         printf("\nmatched START_CODE");
      }

      else if(strncmp(lpl, END_CODE, strlen(END_CODE)) == 0){
         printf("\nmatched END_CODE");
      }

      else if(strncmp(lpl, START_DEFINE, strlen(START_DEFINE)) == 0){
         printf("\nmatched START_DEFINE");
      }
      else if(strncmp(lpl, END_DEFINE, strlen(END_DEFINE)) == 0){
         printf("\nmatched END_DEFINE");
      }

      else if(strncmp(lpl, START_UNIT, strlen(START_UNIT)) == 0){
         strcpy(extra, lpl);

		    /* remove ^< from start of line */
         remove_markers(extra, strlen(START_UNIT));

		    /* if '=' is at end of line then you will 
			   have code following */
         if(strchr(extra, '=') != '\0'){
		 }
		    /* if '%' is at end of line then you will 
			   have sub units following */
         else if(strchr(extra, '%') != '\0'){
		 }

		    /* else remove >^ from end of line */
         else if(strchr(extra, '>') != '\0'){
            extra[strlen(extra)-3] = '\n';
            extra[strlen(extra)-2]   = '\0';
	     }

         code_head = create_code_unit(code_head, extra);
	     /*start_unit_case();*/
         printf("\nmatched START_UNIT");
      }

      else if(strncmp(lpl, END_UNIT, strlen(END_UNIT)) == 0){
         printf("\nmatched END_UNIT");
      }

      /***********
      else if(strncmp(lpl, UNIT_IS, strlen(UNIT_IS)) == 0){
         printf("\nmatched UNIT_IS");
      }
      ***********/

      else{
         printf("\n\tfell all the way through");
      }

   }  /* ends while fgets lpl file */

   fclose(inter_file);

      /* SECOND PASS */
   strcpy(lpl, argv[1]);
   create_file_name(lpl, inter, "int");
   if((inter_file = fopen(inter, "rt")) == '\0'){
      printf("\n\nERROR - cannot open inter file %s", inter);
      exit(6);
   }

   list_toc(head, text_file);
list_code_units(code_head);

   while(fgets(inter, LL, inter_file)){
      sprintf(text, "%s", inter);
      fputs(text, text_file);
   }  /* ends fgets inter file */

   fclose(lpl_file);
   fclose(inter_file);
   fclose(code_file);
   fclose(text_file);
   fclose(code_int_file);

}  /* ends main */



create_file_name(old, new, extension)
   char old[], new[], extension[];
{
   int i,j;
   for(i=0; old[i] != '.'; i++)
      new[i] = old[i];

   new[i] = '.';
   i++;

   for(j=0; extension[j] != '\0'; j++)
      new[i+j] = extension[j];
   new[i+j] = '\0';
}




remove_markers(string, skip)
   char string[];
   int  skip;
{
   int i;
   for(i=0; i<LL-skip; i++)
      string[i] = string[i+skip];
}




section_case(lpl_file, inter_file, lpl, inter, section)
   char   lpl[], inter[];
   FILE   *lpl_file, *inter_file;
   int    *section;
{
   int reading = 1;

   printf("\nmatched SECTION");
   remove_markers(lpl, strlen(SECTION));
   sprintf(inter, "%d.  %s", *section, lpl);
   *section = *section + 1;
   fputs(inter, inter_file);
   while(reading){
      fgets(lpl, LL, lpl_file);
      if(strchr(lpl, '^') != '\0'){
         lpl[strlen(lpl)-2] = '\n';
         lpl[strlen(lpl)-1]   = '\0';
         reading = 0;
      }
      sprintf(inter, "%s", lpl);
      fputs(inter, inter_file);
   }
   sprintf(inter, "%s", BLANKS);
   fputs(inter, inter_file);
   fputs(inter, inter_file);
}


module_case(lpl_file, inter_file, lpl, inter, section)
   char   lpl[], inter[];
   FILE   *lpl_file, *inter_file;
   int    *section;
{
   int reading = 1;

   printf("\nmatched MODULE");
   remove_markers(lpl, strlen(MODULE));
   sprintf(inter, "%d.  %s", *section, lpl);
   *section = *section + 1;
   fputs(inter, inter_file);
   while(reading){
      fgets(lpl, LL, lpl_file);
      if(strchr(lpl, '^') != '\0'){
         lpl[strlen(lpl)-2] = '\n';
         lpl[strlen(lpl)-1]   = '\0';
         reading = 0;
      }
      sprintf(inter, "%s", lpl);
      fputs(inter, inter_file);
   }
   sprintf(inter, "%s", BLANKS);
   fputs(inter, inter_file);
   fputs(inter, inter_file);
}




struct code_struct  *create_code_unit(p, string)
   char   string[];
   struct code_struct *p;
{
    if(p == '\0'){
        p = (struct code_struct *) malloc(sizeof(struct code_struct));
        strcpy(p->name, string);
        p->next_unit = '\0';
    }  /* ends if p == GND */
    else{
        p->next_unit = create_code_unit(p->next_unit, string);
    }
    return(p);
}




list_code_units(h)
   struct code_struct *h;
{

   while(h != '\0'){
      printf("\n ----- %s", h->name);
      h = h->next_unit;
   }

}




struct toc_struct *create_entry(p, string, number)
   char   string[];
   int    number;
   struct toc_struct *p;
{
    if(p == '\0'){
        p = (struct toc_struct *) malloc(sizeof(struct toc_struct));
        strcpy(p->title, string);
        p->section = number;
        p->pointer = '\0';
    }  /* ends if pointer == GND */
    else{
        p->pointer = create_entry(p->pointer, string, number);
    }
    return(p);
}


list_toc(h, text_file)
   struct toc_struct *h;
   FILE   *text_file;
{
   char string[80];

   sprintf(string, "\t\tTable of Contents");
   fputs(string, text_file);

   while(h != '\0'){
      sprintf(string, "\n%d ----- %s", h->section, h->title);
      fputs(string, text_file);
      h = h->pointer;
   }

   sprintf(string, BLANKS);
   fputs(string, text_file);
   fputs(string, text_file);
   sprintf(string, "--------------------------------");
   fputs(string, text_file);
   sprintf(string, BLANKS);
   fputs(string, text_file);
}
