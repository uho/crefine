#line 1 "getargs.cr"
/*************************************************************************
Module  : getargs -- command line option processor
Author  : Lutz Prechelt, Karlsruhe
Date    : 15.11.91  Version 3
Compiler: should be portable (ANSI-C or K&R-C)
**************************************************************************/
/*
    Copyright (C) 1988,91 by Lutz Prechelt, Karlsruhe

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
*/


/************************************************************************
*********************** g e t a r g s ***********************************
*************************************************************************/
#if 0

Varianten:
#define deutsch  1   waehlt deutsche Meldungen statt englische
#define __STDC__ 1   waehlt Funktionsdefinitionen mit Parameterliste
                     statt ohne
#define ms_dos   1   erlaubt "/" als Optionszeichen zusaetzlich zu "-"

Dies ist ein Utility zum bequemeren Auswerten von Optionen auf der
Kommandozeile (d.h. mit argc, argv).

Es wird dazu vom Benutzer eine Tabelle ("argtab") aufgestellt, in der zu
jeder Option der Optionsbuchstabe, ein Argumententyp
(BOOL, CHAR, INT, STRING) und eine Variable angegeben werden,
in der das Resultat abgelegt werden soll.
Ferner kann man hier noch einen Kommentarstring zu jeder Option angeben.

Die Funktion getargs (&argc, argv, argtab, argtabsize) bearbeitet nun den
argv Vektor derart, dass alle Optionen, die in argtab angegeben sind, gemaess
ihrem Eintrag behandelt werden und aus argv entfernt (dazu wird argc und argv
modifiziert, jedoch nur Pointer veraendert, keine argv-Eintraege selbst)
Es duerfen beim Aufruf die Optionen beliebig mit den uebrigen Parametern
vermischt und mehrere Optionen hinter einem Optionssymbol angegeben
werden.

Es werden unbekannte Optionen festgestellt und angemeckert (nach stderr)
Als Resultat wird die Anzahl angemeckerter Optionen geliefert.

Die Funktion print_usage (program_name, usage_text, argtab, argtabsize)
gibt unter Benutzung der Kommentarstrings aus argtab eine Kurzbeschreibung zum
korrekten Aufruf aller Optionen und des Gesamtprogramms nach stderr aus.

Beispiel:
#include <getargs.h>
  int a = 1, b = 'B', c;
  char *d = "";
  ARG argtab[] = { {'a', BOOLEAN,  &a,       "use alternate mode" },
                   {'b', CHARACTER,&b,       "Character for boldface" },
                   {'c', INTEGER,  &c,       "count of pages to print" },
                   {'d', STRING  , (int*)&d, "File description" } };
  void main (int argc, char **argv) {
     if (getargs (&argc, argv, argtab, ARGTABSIZE (argtab)))
        print_usage (argv[0], "[options] filetoprint [options]", argtab,
                     ARGTABSIZE (argtab));
  }

Enthalte argtab also die Liste der Optionen fuer das Programm "fileprt", das
(wie in print_usage angegeben) noch einen Dateinamen als weiteren Parameter
verlangt.

Dann waeren korrekte Aufrufe etwa:

fileprt -a file
fileprt -a+ file -bB
fileprt -c14 file -dMyFile
fileprt file -abBc14

Verkehrt waere dagegen zum Beispiel:

fileprt -dMy File file       weil <File> ein eigenes Argument ist
fileprt -p file              weil es die Option p nicht gibt
fileprt -bx28 file           weil CHAR immer nur ein Zeichen umfasst

Dieser letzte Aufruf haette im Beispiel ungefaehr folgenden Output nach
stderr zur Folge:

Unknown option : -bx28
usage: fileprt [options] filetoprint
valid options are:
-a<+/-> use alternate mode                       (Wert: TRUE)
-b<ch>  Character for boldface                   (Wert: B)
-c<num> count of pages to print                  (Wert: 0)
-d<str> File description                         (Wert: "")

Der genaue Grund fuer die Fehlermeldung ist, dass der Interpreter der Option
b den Wert x zuweist und dann nach der (nicht auffindbaren) Option 2 sucht !

Die "Wert"-Angaben beim print_usage entstehen aus den Vorbelegungen der
Variablen a, b, c, d;

#endif

/**************************************************************************/

#include "std.h"
#include <stdio.h>
#include <ctype.h>
#include "getargs.h"

#if deutsch
#define ERRMSG         "Unbekannte oder falsch benutzte Option"
#define USAGE          "Aufruf"
#define VALID_OPT_ARE  "Erlaubte Optionen sind"
#else
#define ERRMSG         "unknown or malformed option"
#define USAGE          "usage"
#define VALID_OPT_ARE  "valid options are"
#endif

#define is_option(a) (*(a) == '-' && *(a+1) != 0)

static int  set_argument A((ARG* entry, char **optiontext, char **next_arg));
static ARG* find_argument A((int optionname, ARG *argtab, int argtablength));
static int  stoi A((char **linep, int *result));

/*---------------------------------------------------------
 *    getargs
 *---------------------------------------------------------*/

extern int getargs (argc, argv, tabp, tabsize)
  int    *argc;    /* changed */
  char ***argv;    /* changed */
  ARG    *tabp;
  int     tabsize;
{
  /* Main routine. Evaluates all arguments in argv up to argc:
     Options (known from first letter as given by macro is_option)
     are followed by other arguments.
     -- forces end of options.
     options are searched for in tabp and are removed from argv by
     shifting the remaining arguments left.
     options are handled according to their tabp entry,
     illegal options or missing or illegal values are complained.
     Otherwise the corresponding variable is set according to the value
     given with the option.
     Only non-options are still in argv after this procedure, their
     number is given back in *argc.
     The return value is the number of errors found.
  */
  char  *p, *argv0 = **argv, *nilarg = "";
  int    errors = 0, error;
  ARG   *argp;
  for ((*argv)++; --(*argc) > 0; (*argv)++)
    {   /* handle_this_arg  (Level 1) */
#line 166 "getargs.cr"
    if ((
#line 196 "getargs.cr"
    /* the argument "--" forces end of option processing */
    (*argv)[0][0] == '-'  &&  (*argv)[0][1] == '-'  &&  (*argv)[0][2] == 0)
#line 166 "getargs.cr"
    ) {
       **argv = argv0;  /* restore program name */
       return (errors);
    }
    else if (!is_option (**argv)) {
       (*argv)--; (*argc)++;
       **argv = argv0;   /* restore program name */
       return (errors);
    }
    else {
       char **next_arg = *argc > 1 ? (*argv)+1 : &nilarg;
       p = (**argv) + 1;         /* Option -> handle it */
       while (*p) {
         error = 0;
         /* One Optionsign can have multiple Options following */
         if (argp = find_argument ((int)*p, tabp, tabsize)) /* if exists */
           {   /* read_option_value  (Level 2) */
#line 200 "getargs.cr"
           error = !set_argument (argp, &p, next_arg);
           }
#line 183 "getargs.cr"
         if (!argp || error) {   /* if not exists or invalid value */
            fprintf (stderr, "%s : %s\n", ERRMSG, **argv);   /* then #$% */
            errors++;
            break;
         }
         if (*next_arg == 0) {  /* Next argv element already used up */
           (*argv)++;
           (*argc)--;
         }
       }
     }
    }
#line 161 "getargs.cr"
  (*argv)--;
  **argv = argv0;   /* restore program name */
  return (errors);
#line 202 "getargs.cr"
}

/*---------------------------------------------------------
 *    set_argument
 *---------------------------------------------------------*/

static int set_argument (argp, linep, next_arg)
  ARG  *argp;
  char **linep, **next_arg;
{
  /* Gets the argument for the current option into the corresponding
     variable as given in argtab.
     linep is incremented as much as necessary (as much as data is
     used for the value).
     If no value can be found in linep, next_arg is searched for it,
     and, if found, set to 0.
     Returns 1 (or 0 on errors, i.e. illegal or missing values)
  */
#define p (argp->variable)
  char *old_linep, *old_next_arg = *next_arg;
  ++(*linep);   /* skip Optionname */
  old_linep = *linep;
  switch (argp->type) {
        case INTEGER:    
        {   /* get_integer  (Level 1) */
#line 233 "getargs.cr"
        stoi (linep, p);
        if (old_linep == *linep) {  /* no integer found in linep */
          stoi (next_arg, p);
          if (old_next_arg != *next_arg)
            *next_arg = 0;
        }
        return (old_linep != *linep || *next_arg == 0);
        }
#line 226 "getargs.cr"
        case BOOLEAN:    
        {   /* get_bool  (Level 1) */
#line 242 "getargs.cr"
        if ((
#line 257 "getargs.cr"
        **linep == '-' || **linep == '+')
#line 242 "getargs.cr"
        ) {  /* + or - given right behind */
           *p = (**linep == '+');
           ++(*linep);
        }
        else if ((
#line 260 "getargs.cr"
        **linep != 0)
#line 246 "getargs.cr"
        )  /* no value given */
           *p = 1;   /* assume true */
        else if ((
#line 263 "getargs.cr"
        (**next_arg == '-' || **next_arg == '+') &&
        (*next_arg)[1] == 0)
#line 248 "getargs.cr"
        ) {
           *p = **next_arg == '+';
           *next_arg = 0;
        }
        else                        /* else assume TRUE */
           *p = 1;
        return (1);
        }
#line 227 "getargs.cr"
        case CHARACTER:  
        {   /* get_char  (Level 1) */
#line 267 "getargs.cr"
        *p = (int)**linep;
        if (*p != 0) {
          ++(*linep);      /* go on one char */
          return (1);
        }
        /* we must get character from next_arg, if possible */
        if ((
#line 282 "getargs.cr"
        (*next_arg)[0] != 0  &&  (*next_arg)[1] == 0)
#line 273 "getargs.cr"
        ) {
          *p = **next_arg;
          *next_arg = 0;
          return (1);
        }
        else
          return (0);
        }
#line 228 "getargs.cr"
        case STRING:     
        {   /* get_string  (Level 1) */
#line 285 "getargs.cr"
        if (**linep != 0) {
          *(char **)p = *linep;
          *linep = "";   /* take all the rest */
          return (1);
        }
        /* we must get string from next_arg */
        *(char **)p = *next_arg;
        *next_arg = 0;
        return (1);

#undef p
        }
#line 229 "getargs.cr"
  }
  return (0);  /* just to keep certain compilers quiet */
#line 296 "getargs.cr"
}

/*---------------------------------------------------------
 *    find_argument
 *---------------------------------------------------------*/

static ARG* find_argument (optionname, tabp, tabsize)
  int  optionname;
  ARG *tabp;
  int  tabsize;
{
  for (; --tabsize >= 0; tabp++)
      if (tabp->arg == optionname)
         return (tabp);
  return (0);  /* not found */
}

/*---------------------------------------------------------
 *    print_usage
 *---------------------------------------------------------*/

extern void print_usage (progname, usage, tabp, tabsize)
  char *progname, *usage;
  ARG  *tabp;
  int  tabsize;
{
  /* Druckt die Optionsbeschreibungen laut tabp incl. momentaner Werte der
     zugeh. Variablen sowie zuvor eine "usage:" Zeile mit den Texten
     progname und usage.
  */
  char *p;
  int  i;
  fprintf (stderr, "\n%s: %s %s\n%s:\n", USAGE, progname,
           usage, VALID_OPT_ARE);
  for (i = 0; i < tabsize; i++, tabp++) {
      fprintf (stderr, "-%c", tabp->arg);
      p = tabp->errmsg;
      switch (tabp->type) {
            case INTEGER:
                  fprintf (stderr, "<num> %-45s (%d)\n",
                           p, *(tabp->variable));
                  break;
            case BOOLEAN:
                  fprintf (stderr, "<+/-> %-45s (%s)\n",
                           p, *(tabp->variable) ? "TRUE" : "FALSE");
                  break;
            case CHARACTER:
                  fprintf (stderr, "<ch>  %-45s (%c)\n",
                           p, (char)*(tabp->variable));
                  break;
            case STRING:
                  fprintf (stderr, "<str> %-45s (\"%s\")\n",
                           p, *(char **)tabp->variable);
                  break;
            }
      }
}


/********************* Hilfsfunktionen ************************************/


/*---------------------------------------------------------
 *    stoi
 *---------------------------------------------------------*/

static int stoi (instr, result)
  char **instr;
  int   *result;
{
  /* tries to read a decimal, octal or hexadecimal number from *instr
     and advances *instr accordingly.
     returns whether a number could successfully be read.
     the number read is returned in *result.
  */
  int  error = 1;
  int  num = 0;
  int  sign = 0;  /* Betrag und neg. Vorzeichen des Resultats */
  char *str = *instr;
  while (isspace (*str))
        str++;
  if (*str == '-') {
     sign = -1;
     str++;
  }
  if (*str == '0')
    {   /* read_octal_or_hex  (Level 1) */
#line 392 "getargs.cr"
    ++str;
    error = 0;
    if (toupper (*str) == 'X') {
       str++;
       while(isxdigit(*str)) {
             num *= 16;
             num += isdigit (*str) ? *str - '0'
                             : toupper (*str) - 'A'+ 10;
             str++;
       }
    }
    else {
       while ('0' <= *str && *str <= '7') {
             num *= 8;
             num += *str++ - '0';
       }
    }
    }
#line 383 "getargs.cr"
  else
    {   /* read_decimal  (Level 1) */
#line 411 "getargs.cr"
    while (isdigit (*str)) {
      error = 0;
      num *= 10;
      num += *str++ - '0';

    }
    }
#line 385 "getargs.cr"
  if (error)
    return (error);
  *instr = str;
  *result = sign ? -num : num;
  return (0);
#line 418 "getargs.cr"
}

/************* Hauptprogramm zum Testen **********************************/

#if 0

  int a = 0, b = 'b', c = 3;
  char *d = "";
  ARG argtab[] = { {'a', BOOLEAN,     &a,  "Option a" },
                   {'b', CHARACTER,   &b,  "Option b" },
                   {'c', INTEGER,     &c,  "Option c" },
                   {'d', STRING, (int*)&d,  "Option d" } };

void main (int argc, char **argv)
{
  int i, n;
  printf ("Argumentpointer argv[i]: ");
  for (i = 0; i < argc; i++)
      printf ("%d ", (int)argv[i]);
  printf ("\n");
  n = getargs (&argc, argv, argtab, ARGTABSIZE (argtab));
  if (n)
     print_usage (argv [0], "rabarber", argtab, ARGTABSIZE (argtab));
  printf ("\n RESULTATE (%d): a = %d, b = '%c', c = %d, d = \"%s\"\n",
          n, a, (char)b, c, d);
  printf ("ARGV: ");
  for (i = 0; i < argc; i++)
      printf ("<%s> ", argv[i]);
}

#endif

