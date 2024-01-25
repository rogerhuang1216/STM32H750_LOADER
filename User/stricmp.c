/*
FUNCTION
    <<stricmp>>---character string compare, upper case and low case are same
    
INDEX
    strcmp

ANSI_SYNOPSIS
    #include <string.h>
    int strcmp(const char *<[a]>, const char *<[b]>);

TRAD_SYNOPSIS
    #include <string.h>
    int strcmp(<[a]>, <[b]>)
    char *<[a]>;
    char *<[b]>;

DESCRIPTION
    <<strcmp>> compares the string at <[a]> to
    the string at <[b]>.

RETURNS
    If <<*<[a]>>> sorts lexicographically after <<*<[b]>>>,
    <<strcmp>> returns a number greater than zero.  If the two
    strings match, <<strcmp>> returns zero.  If <<*<[a]>>>
    sorts lexicographically before <<*<[b]>>>, <<strcmp>> returns a
    number less than zero.

PORTABILITY
<<strcmp>> is ANSI C.

<<strcmp>> requires no supporting OS subroutines.

QUICKREF
    stricmp ansi pure
*/

#include <ctype.h>
#include <string.h>
#include <limits.h>

int isascii(int c)
{
	return ((c>=0 && c<=127)?1:0);
}

int stricmp( const char* s1 , const char* s2 )
{
    while ( toupper( *s1 ) != '\0' && toupper( *s1 ) == toupper( *s2 ) )
    {
        s1++;
        s2++;
    }
    return ( *( unsigned char * ) s1 ) - ( *( unsigned char * ) s2 );
}

int strnicmp(const char *s1, const char *s2, int len)
{
	/* Yes, Virginia, it had better be unsigned */
	unsigned char c1, c2;
	int flag_w=0;	// word or not

	c1 = 0;	c2 = 0;
	if (len) {
		do {
			c1 = *s1; c2 = *s2;
			s1++; s2++;
			if (!c1)
				break;
			if (!c2)
				break;

			if (flag_w==0)
			{
				c1 = tolower(c1);
				c2 = tolower(c2);
			}

			if (c1 != c2)
				break;

			if (flag_w==1)
				flag_w=0;
			else if (!isascii(c1))
				flag_w=1;	// it is word, next byte need not tolower

		} while (--len);
	}
	return (int)c1 - (int)c2;
}
