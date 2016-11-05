/*  msd_dir.c - portable directory routines
    Copyright (C) 1990 by Thorsten Ohl, td12@ddagsi3.bitnet

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    $Header: e:/gnu/lib/RCS/ndir.c 1.0 90/09/10 23:16:19 tho Stable $
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dos.h>

#include "d:\gnuftu\ndir.h"

static void free_dircontents (struct _dircontents *);

/* find ALL files! */
#define ATTRIBUTES	(_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_SUBDIR)




dirdir (name)
   char name[80];
{
  static DIR *directory;
  struct direct *entry = (struct direct *)0;

  directory = opendir (name);

  if (!directory)
    {
      fprintf (stderr, "can't open directory `%s'.\n", name);
      exit (2);
    }

  while (entry = readdir (directory))
    printf ("> %s\n", entry->d_name);

  printf ("done.\n");
  closedir(directory);
}

DIR *
opendir (char *name)
{
  struct find_t find_buf;
  DIR *dirp;
  struct _dircontents *dp;
  char name_buf[_MAX_PATH + 1];
  char *slash = "";

  if (!name)
    name = "";
  else if (*name)
    {
      char *s;
      int l = strlen (name);

      s = name + l - 1;
      if ( !(l == 2 && *s == ':') && *s != '\\' && *s != '/')
	slash = "/";	/* save to insert slash between path and "*.*" */
    }

  strcat (strcat (strcpy (name_buf, name), slash), "*.*");

  dirp = (DIR *) malloc (sizeof (DIR));
  if (dirp == (DIR *)0)
    return (DIR *)0;

  dirp->dd_loc = 0;
  dirp->dd_contents = dirp->dd_cp = (struct _dircontents *) 0;

  if (_dos_findfirst (name_buf, ATTRIBUTES, &find_buf))
    {
      free (dirp);
      return (DIR *)0;
    }

  do
    {
      dp = (struct _dircontents *) malloc (sizeof (struct _dircontents));
      if (dp == (struct _dircontents *)0)
	{
	  free_dircontents (dirp->dd_contents);
	  return (DIR *)0;
	}

      dp->_d_entry = malloc (strlen (find_buf.name) + 1);
      if (dp->_d_entry == (char *)0)
	{
	  free (dp);
	  free_dircontents (dirp->dd_contents);
	  return (DIR *)0;
	}

      if (dirp->dd_contents)
	dirp->dd_cp = dirp->dd_cp->_d_next = dp;
      else
	dirp->dd_contents = dirp->dd_cp = dp;

      strcpy (dp->_d_entry, find_buf.name);

      dp->_d_next = (struct _dircontents *)0;

    } while (! _dos_findnext (&find_buf));

  dirp->dd_cp = dirp->dd_contents;

  return dirp;
}


void
closedir (DIR *dirp)
{
  free_dircontents (dirp->dd_contents);
  free ((char *) dirp);
}


struct direct *
readdir (DIR *dirp)
{
  static struct direct dp;

  if (dirp->dd_cp == (struct _dircontents *)0)
    return (struct direct *)0;
  dp.d_namlen = dp.d_reclen =
    strlen (strcpy (dp.d_name, dirp->dd_cp->_d_entry));
  strlwr (dp.d_name);		/* JF */
  dp.d_ino = 0;
  dirp->dd_cp = dirp->dd_cp->_d_next;
  dirp->dd_loc++;

  return &dp;
}


void
seekdir (DIR *dirp, long off)
{
  long i = off;
  struct _dircontents *dp;

  if (off < 0)
    return;
  for (dp = dirp->dd_contents; --i >= 0 && dp; dp = dp->_d_next)
    ;
  dirp->dd_loc = off - (i + 1);
  dirp->dd_cp = dp;
}


long
telldir (DIR *dirp)
{
  return dirp->dd_loc;
}


/* Garbage collection */

static void
free_dircontents (struct _dircontents *dp)
{
  struct _dircontents *odp;

  while (dp)
    {
      if (dp->_d_entry)
	free (dp->_d_entry);
      dp = (odp = dp)->_d_next;
      free (odp);
    }
}
