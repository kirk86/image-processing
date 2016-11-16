/***************************************************************
*
*       file blue.c
*
*       Functions: This file contains
*           main
*
***************************************************************/

#include <stdio.h>
/* #include "d:\msvc\include\graph.h" */
#ifdef __APPLE__
    #include <sys/uio.h>
#else
    #include <sys/io.h>
#endif

int main(int argc, char *argv[])
{
   char string[80];
   int  i, j;

   _setvideomode(_TEXTC80);
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);

}  /* ends main */
