/* asdbg.c */

/*
 *  Copyright (C) 2003-2021  Alan R. Baldwin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 *
 *
 * Code extracted from asnoice.c
 * written by:
 *	John L. Hartman	(JLH)
 *	3-Nov-1997
 */

#include "asxxxx.h"

/*)Module	asdbg.c
 *
 *	The module asdbg.c contains the functions that
 *
 *	1) generate debug symbols for assembler code
 *	   similiar to those generated by the SDCC compiler
 *
 *	2) generate debug symbols for the NoICE
 *	   remote debugger
 *
 *	asdbg.c contains the following functions:
 *		VOID	DefineSDCC_Line()
 *		VOID	DefineNoICE_Line()
 *		char *	BaseFileName()
 *
 *	asdbg.c contains the static variables:
 *		int	prevFile
 *		char	baseName[FILSPC]
 *
 *	    used by the BaseFileName function.
 *
 * NOTE:
 *	These functions know nothing about 'include' files
 *	and do not track their inclusion.
 */

/*)Function	VOID	DefineSDCC_Line()
 *
 *	The function DefineSDCC_Line() is called to create
 *	a symbol of the form A$FILE$nnn where FILE is the
 *	Base File Name, the file name without a path or
 *	an extension, and nnn is the line number.
 *
 *	local variables:
 *		char		name[]	a temporary to build the symbol
 *	        struct sym *	pSym	pointer to the created symbol structure
 *
 *	global variables:
 *		int	srcline		array of source file line numbers
 *		a_uint	laddr		current assembler address
 *		area	dot.s_area	pointer to the current area
 *
 *	functions called:
 *		char *	BaseFileName()	asdbg.c
 *		sym *	lookup()	assym.c
 *		int	sprintf()	c_library
 *
 *	side effects:
 *		A new symbol of the form A$FILE$nnn is created.
 */

#if SDCDB
VOID
DefineSDCC_Line()
{
	char name[ NCPS ];
        struct sym *pSym;

	/*
	 * Symbol is A$FILE$nnn
	 */
        sprintf( name, "A$%s$%u", BaseFileName( asmc ), srcline );

        pSym = lookup( name );
        pSym->s_type = S_USER;
        pSym->s_area = dot.s_area;
        pSym->s_addr = laddr;
        pSym->s_flag |= S_GBL;
}
#endif


/*)Function	VOID	DefineNoICE_Line()
 *
 *	The function DefineNoICE_Line() is called to create
 *	a symbol of the form FILE.nnn where FILE is the
 *	Base File Name, the file name without a path or
 *	an extension, and nnn is the line number.
 *
 *	local variables:
 *		char		name[]	a temporary to build the symbol
 *	        struct sym *	pSym	pointer to the created symbol structure
 *
 *	global variables:
 *		int	cfile		current source file number
 *		int	srcline		array of source file line numbers
 *		a_uint	laddr		current assembler address
 *		area	dot.s_area	pointer to the current area
 *
 *	functions called:
 *		char *	BaseFileName()	asdbg.c
 *		sym *	lookup()	assym.c
 *		int	sprintf()	c_library
 *
 *	side effects:
 *		A new symbol of the form FILE.nnn is created.
 */

#if NOICE
VOID
DefineNoICE_Line()
{
	char name[ NCPS ];
        struct sym *pSym;

	/*
	 * Symbol is FILE.nnn
	 */
        sprintf( name, "%s.%u", BaseFileName( asmc ), srcline );

        pSym = lookup( name );
        pSym->s_type = S_USER;
        pSym->s_area = dot.s_area;
        pSym->s_addr = laddr;
        pSym->s_flag |= S_GBL;
}
#endif


/*)Function	char *	BaseFileName(currFile)
 *
 *	The function BaseFileName() is called to extract
 *	the file name from a string containing a path,
 *	filename, and extension.
 *
 *		currFile		is a pointer to the
 *					current assembler object
 *
 *	local variables:
 *		char	baseName[]	a place to put the file name
 *	        int	prevFile	previous assembler object
 *		char *	p1		temporary string pointer
 *		char *	p2		temporary string pointer
 *
 *	global variables:
 *		FILE *	ofp		output file handle
 *
 *	functions called:
 *		int	fprintf()	c_library
 *		char *	strcpy()	c_library
 *		char *	strrchr()	c_library
 *
 *	side effects:
 *		A FILE command of the form ';!FILE string'
 *		is written to the output file.
 */

#if (NOICE || SDCDB)
static	struct	asmf *	prevFile = NULL;
static	char	baseName[FILSPC];

char*
BaseFileName(currFile)
struct	asmf * currFile;
{
        char *p1, *p2;

	if (currFile != prevFile) {
        	prevFile = currFile;

		strcpy(baseName, afn);
                p1 = baseName;

                /*
	         * Dump a FILE command with full path and extension
		 */
                fprintf(ofp, ";!FILE %s\n", p1);

		/*
		 * The name starts after the last
		 * '/' (Unices) or
		 * ':' or '\' (DOS)
		 *
		 * and ends at the last
		 * separator 'FSEPX'
		 */
		if ((p2 = strrchr(p1,  '\\')) != NULL)  p1 = ++p2;
		if ((p2 = strrchr(p1,   '/')) != NULL)  p1 = ++p2;
		if ((p2 = strrchr(p1,   ':')) != NULL)  p1 = ++p2;
		if ((p2 = strrchr(p1, FSEPX)) != NULL) *p2 = 0;
		strcpy(baseName, p1);
	}
	return(baseName);
}
#endif


