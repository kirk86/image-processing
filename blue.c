       /***************************************************************
       *
       *       file c:\lsu\blue.c
       *
       *       Functions: This file contains
       *           main
       *
       *       Purpose: This program sets the screen to blue background
       *	   with gray letters. It is in the Microsoft C 5.0.
       *
       *       Modifications:
       *	   13 March 1989 - created
       *
       ***************************************************************/



#include <stdio.h>
/* #include "d:\msvc\include\graph.h" */
#include <io.h>


int main(int argc, char *argv[])
{
   char string[80];
   int  i, j;

   _setvideomode(_TEXTC80);
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);

}  /* ends main */
