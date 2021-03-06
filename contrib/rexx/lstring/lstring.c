/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.3  1999/11/26 12:52:25  bnv
 * Added: Windows CE support
 * Added: Lwscpy, for unicode string copy
 * Changed: _Lisnum, it creates immediately a double number contained in
 * the string, for faster access. The value is hold in lLastScannedNumber
 *
 * Revision 1.2  1999/05/14 13:11:47  bnv
 * Minor changes
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#define __LSTRING_C__

#include <math.h>
#include <lerror.h>
#include <lstring.h>

#ifndef WIN
#	include <values.h>
#endif
/* ================= Lstring routines ================== */

/* -------------------- Linit ---------------- */
void
Linit( LerrorFunc Lerr)
{
	size_t	i;

	/* setup error function */
#ifndef WCE
	if (Lerr)
		Lerror = Lerr;
	else
		Lerror = Lstderr;
#else
	Lerror = Lerr;
#endif

	/* setup upper */
	for (i=0; i<256; i++)  u2l[i] = l2u[i] = i;
	for (i=0; clower[i]; i++) {
		l2u[ (byte)clower[i] & 0xFF ] = cUPPER [i];
		u2l[ (byte)cUPPER[i] & 0xFF ] = clower [i];
	}

	/* setup time */
	_Ltimeinit();
} /* Linit */

/* -------------- _Lfree ------------------- */
void
_Lfree(void *str)
{
	LPFREE((PLstr)str);
} /* _Lfree */

/* ---------------- Lfx -------------------- */
void
Lfx( const PLstr s, const size_t len )
{
	size_t	max;

	if (LISNULL(*s)) {
		LSTR(*s) = (char *) MALLOC( (max = LNORMALISE(len))+LEXTRA, "Lstr" );
		LLEN(*s) = 0;
		LMAXLEN(*s) = max;
		LTYPE(*s) = LSTRING_TY;
#ifdef USEOPTION
		LOPT(*s) = 0;
#endif
	} else
#ifdef USEOPTION
	if (!LOPTION(*s,LOPTFIX) && LMAXLEN(*s)<len) {
		LSTR(*s) = (char *) REALLOC( LSTR(*s), (max=LNORMALISE(len))+LEXTRA);
		LMAXLEN(*s) = max;
	}
#else
	if (LMAXLEN(*s)<len) {
		LSTR(*s) = (char *) REALLOC( LSTR(*s), (max=LNORMALISE(len))+LEXTRA);
		LMAXLEN(*s) = max;
	}
#endif
} /* Lfx */

/* ---------------- Licpy ------------------ */
void
Licpy( const PLstr to, const long from )
{
	LLEN(*to)  = sizeof(long);
	LTYPE(*to) = LINTEGER_TY;
	LINT(*to)  = from;
} /* Licpy */

/* ---------------- Lrcpy ------------------ */
void
Lrcpy( const PLstr to, const double from )
{
	LLEN(*to)  = sizeof(double);
	LTYPE(*to) = LREAL_TY;
	LREAL(*to) = from;
} /* Lrcpy */

/* ---------------- Lscpy ------------------ */
void
Lscpy( const PLstr to, const char *from )
{
	size_t	len;

	if (!from)
		Lfx(to,len=0);
	else {
		Lfx(to,len = STRLEN(from));
		MEMCPY( LSTR(*to), from, len );
	}
	LLEN(*to) = len;
	LTYPE(*to) = LSTRING_TY;
} /* Lscpy */

#ifdef WCE
/* ---------------- Lwscpy ------------------ */
void
Lwscpy(const PLstr to, const wchar_t *from )
{
	size_t	len;

	if (!from)
		Lfx(to,len=0);
	else {
		Lfx(to,len = wcslen(from));
		wcstombs(LSTR(*to), from ,len );
	}
	LLEN(*to) = len;
	LTYPE(*to) = LSTRING_TY;
} /* Lwscpy */
#endif

/* ---------------- Lcat ------------------- */
void
Lcat( const PLstr to, const char *from )
{
	size_t	l;

	if (from==NULL) return;

	if (LLEN(*to)==0)
		Lscpy( to, from );
	else {
		L2STR(to);
		l=LLEN(*to) + STRLEN(from);
		if (LMAXLEN(*to)<l) Lfx(to,l);
		STRCPY( LSTR(*to) + LLEN(*to), from );
		LLEN(*to) = l;
	}
} /* Lcat */

/* ------------------ Lcmp ------------------- */
int
Lcmp( const PLstr a, const char *b )
{
	int	r,blen;

	L2STR(a);

	blen = STRLEN(b);
	if ( (r=MEMCMP( LSTR(*a), b, MIN(LLEN(*a),blen)))!=0 )
		return r;
	else {
		if (LLEN(*a) > blen)
			return 1;
		else
		if (LLEN(*a) == blen)
			return 0;
		else
			return -1;
	}
} /* Lcmp */

/* ---------------- Lstrcpy ----------------- */
void
Lstrcpy( const PLstr to, const PLstr from )
{
	if (LLEN(*from)==0) {
		LLEN(*to) = 0;
		LTYPE(*to) = LSTRING_TY;
	} else {
		if (LMAXLEN(*to)<=LLEN(*from)) Lfx(to,LLEN(*from));
		switch ( LTYPE(*from) ) {
			case LSTRING_TY:
				MEMCPY( LSTR(*to), LSTR(*from), LLEN(*from) );
				break;

			case LINTEGER_TY:
				LINT(*to) = LINT(*from);
				break;

			case LREAL_TY:
				LREAL(*to) = LREAL(*from);
				break;
		}
		LTYPE(*to) = LTYPE(*from);
		LLEN(*to) = LLEN(*from);
	} 
} /* Lstrcpy */

/* ----------------- Lstrcat ------------------ */
void
Lstrcat( const PLstr to, const PLstr from )
{
	size_t	l;
	if (LLEN(*from)==0) return;

	if (LLEN(*to)==0) {
		Lstrcpy( to, from );
		return;
	}

	L2STR(to);
	L2STR(from);

	l = LLEN(*to)+LLEN(*from);
	if (LMAXLEN(*to) <= l)
		Lfx(to, l);
	MEMCPY( LSTR(*to) + LLEN(*to), LSTR(*from), LLEN(*from) );
	LLEN(*to) = l;
} /* Lstrcat */

/* ----------------- _Lstrcmp ----------------- */
/* -- Low level strcmp, suppose that both of -- */
/* -- are of the same type                      */
int
_Lstrcmp( const PLstr a, const PLstr b )
{
	int	r;

	if ( (r=MEMCMP( LSTR(*a), LSTR(*b), MIN(LLEN(*a),LLEN(*b))))!=0 )
		return r;
	else {
		if (LLEN(*a) > LLEN(*b))
			return 1;
		else
		if (LLEN(*a) == LLEN(*b)) {
			if (LTYPE(*a) > LTYPE(*b))
				return 1;
			else
			if (LTYPE(*a) < LTYPE(*b))
				return -1;
			return 0;
		} else
			return -1;
	}
} /* _Lstrcmp */

/* ----------------- Lstrcmp ------------------ */
int
Lstrcmp( const PLstr a, const PLstr b )
{
	int	r;

	L2STR(a);
	L2STR(b);

	if ( (r=MEMCMP( LSTR(*a), LSTR(*b), MIN(LLEN(*a),LLEN(*b))))!=0 )
		return r;
	else {
		if (LLEN(*a) > LLEN(*b))
			return 1;
		else
		if (LLEN(*a) == LLEN(*b))
			return 0;
		else
			return -1;
	}
}  /* Lstrcmp */

/* ----------------- Lstrset ------------------ */
void
Lstrset( const PLstr to, const size_t length, const char value)
{
	Lfx(to,length);
	LTYPE(*to) = LSTRING_TY;
	LLEN(*to) = length;
	MEMSET(LSTR(*to),value,length);
}  /* Lstrset */

/* ----------------- _Lsubstr ----------------- */
/* WARNING!!! length is size_t type DO NOT PASS A NEGATIVE value */
void
_Lsubstr( const PLstr to, const PLstr from, size_t start, size_t length )
{
	L2STR(from);

	start--;
	if ((length==0) || (length+start>LLEN(*from)))
		length = LLEN(*from) - start;

	if (start<LLEN(*from)) {
		if (LMAXLEN(*to)<length) Lfx(to,length);
		MEMCPY( LSTR(*to), LSTR(*from)+start, length );
		LLEN(*to) = length;
	} else
		LZEROSTR(*to);
	LTYPE(*to) = LSTRING_TY;
}  /* Lstrsub */

/* ------------------------ _Lisnum ----------------------- */
/* _Lisnum      - returns if it is possible to convert      */
/*               a LSTRING to NUMBER                        */
/*               a LREAL_TY or LINTEGER_TY                  */
/* There is one possibility that is missing...              */
/* that is to hold an integer number as a real in a string. */
/* ie.   '  2.0 '  this should be LINTEGER not LREAL        */
/* -------------------------------------------------------- */
int
_Lisnum( const PLstr s )
{
	bool	F, R;
	register char	*ch;

	int	sign;
	int	exponent;
	int	expsign;
	int	fractionDigits;

	lLastScannedNumber = 0.0;

/* ---
#ifdef USEOPTION
	if (LOPT(*s) & (LOPTINT | LOPTREAL)) {
		if (LOPT(*s) & LOPTINT)
			return LINTEGER_TY;
		else
			return LREAL_TY;
	}
#endif
--- */

	ch = LSTR(*s);
	if (ch==NULL) return LSTRING_TY;
	LASCIIZ(*s);	/*	///// Remember to erase LASCIIZ
				///// before all the calls to Lisnum */

	/* skip leading spaces */
	while (ISSPACE(*ch)) ch++;

	/* accept one sign */
	if (*ch=='-') {
		sign = TRUE;
		ch++;
	} else {
		sign=FALSE;
		if (*ch=='+')
			ch++;
	}

	/* skip following spaces after sign */
	while (ISSPACE(*ch)) ch++;

	/* accept many digits */
	R = FALSE;

	lLastScannedNumber = 0.0;
	fractionDigits=0;
	exponent=0;
	expsign=FALSE;

	if (IN_RANGE('0',*ch,'9')) {
		lLastScannedNumber = lLastScannedNumber*10.0 + (*ch-'0');
		ch++;
		F = TRUE;
		while (IN_RANGE('0',*ch,'9')) {
			lLastScannedNumber = lLastScannedNumber*10.0 + (*ch-'0');
			ch++;
		}
		if (!*ch) goto isnumber;
	} else
		F = FALSE;

	/* accept one dot */
	if (*ch=='.') {
		R = TRUE;
		ch++;

		/* accept many digits */
		if (IN_RANGE('0',*ch,'9')) {
			lLastScannedNumber = lLastScannedNumber*10.0 + (*ch-'0');
			fractionDigits++;
			ch++;
			while (IN_RANGE('0',*ch,'9')) {
				lLastScannedNumber = lLastScannedNumber*10.0 + (*ch-'0');
				fractionDigits++;
				ch++;
			}
		} else
			if (!F) return LSTRING_TY;

		if (!*ch) goto isnumber;
	} else
		if (!F) return LSTRING_TY;


	/* accept on 'e' or 'E' */
	if (*ch=='e' || *ch=='E') {
		ch++;
		R = TRUE;
		/* accept one sign */
		if (*ch=='-') {
			expsign = TRUE;
			ch++;
		} else
		if (*ch=='+')
			ch++;

		/* accept many digits */
		if (IN_RANGE('0',*ch,'9')) {
			exponent = exponent*10+(*ch-'0');
			ch++;
			while (IN_RANGE('0',*ch,'9')) {
				exponent = exponent*10+(*ch-'0');
				ch++;
			}
		} else
			return LSTRING_TY;
	}

	/* accept many blanks */
	while (ISSPACE(*ch)) ch++;

	/* is it end of string */
	if (*ch) return LSTRING_TY;

isnumber:
	if (expsign) exponent = -exponent;

	exponent -= fractionDigits;

	if (exponent)
#ifdef __BORLAND_C__
		lLastScannedNumber *= pow10(exponent);
#else
		lLastScannedNumber *= pow(10.0,(double)exponent);
#endif

	if (lLastScannedNumber>MAXLONG)
		R = TRUE;	/* Treat it as real number */

	if (sign)
		lLastScannedNumber = -lLastScannedNumber;

	if (R) return LREAL_TY;

	return LINTEGER_TY;
} /* _Lisnum */

/* ------------------ L2str ------------------- */
void
L2str( const PLstr s )
{
	if (LTYPE(*s)==LINTEGER_TY) {
#if defined(WCE) || defined(__BORLANDC__)
		LTOA(LINT(*s),LSTR(*s),10);
#else
		sprintf(LSTR(*s), "%ld", LINT(*s));
#endif
		LLEN(*s) = STRLEN(LSTR(*s));
	} else {	/* LREAL_TY */
/*////		sprintf(LSTR(*s), lFormatStringToReal, LREAL(*s)); */
		GCVT(LREAL(*s),lNumericDigits,LSTR(*s));
#ifdef WCE
		{
			/* --- remove the last dot from the number --- */
			size_t	len = STRLEN(LSTR(*s));
			if (LSTR(*s)[len-1] == '.') len--;
			LLEN(*s) = len;
		}
#else
		LLEN(*s) = STRLEN(LSTR(*s));
#endif
	}
	LTYPE(*s) = LSTRING_TY;
} /* L2str */

/* ------------------ L2int ------------------- */
void
L2int( const PLstr s )
{
	if (LTYPE(*s)==LREAL_TY) {
		if ((double)((long)LREAL(*s)) == LREAL(*s))
			LINT(*s) = (long)LREAL(*s);
		else
			Lerror(ERR_INVALID_INTEGER,0);
	} else { /* LSTRING_TY */
		LASCIIZ(*s);
		switch (_Lisnum(s)) {
			case LINTEGER_TY:
				/*///LINT(*s) = atol( LSTR(*s) ); */
				LINT(*s) = (long)lLastScannedNumber;
				break;

			case LREAL_TY:
				/*///LREAL(*s) = strtod( LSTR(*s), NULL ); */
				LREAL(*s) = lLastScannedNumber;
				if ((double)((long)LREAL(*s)) == LREAL(*s))
					LINT(*s) = (long)LREAL(*s);
				else
					Lerror(ERR_INVALID_INTEGER,0);
				break;

			default:
				Lerror(ERR_INVALID_INTEGER,0);
		}
	}
	LTYPE(*s) = LINTEGER_TY;
	LLEN(*s) = sizeof(long);
} /* L2int */

/* ------------------ L2real ------------------- */
void
L2real( const PLstr s )
{
	if (LTYPE(*s)==LINTEGER_TY)
		LREAL(*s) = (double)LINT(*s);
	else { /* LSTRING_TY */
		LASCIIZ(*s);
		if (_Lisnum(s)!=LSTRING_TY)
			/*/////LREAL(*s) = strtod( LSTR(*s), NULL ); */
			LREAL(*s) = lLastScannedNumber;
		else
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
	LTYPE(*s) = LREAL_TY;
	LLEN(*s) = sizeof(double);
} /* L2real */

/* ------------------- _L2num -------------------- */
/* this function is used when we know to what type */
/* we should change the string                     */
void
_L2num( const PLstr s, const int type )
{
	LASCIIZ(*s);
	switch (type) {
		case LINTEGER_TY:
			/*////LINT(*s) = atol( LSTR(*s) ); */
			LINT(*s) = (long)lLastScannedNumber;
			LTYPE(*s) = LINTEGER_TY;
			LLEN(*s) = sizeof(long);
			break;

		case LREAL_TY:
			/*////LREAL(*s) = strtod( LSTR(*s), NULL ); */
			LREAL(*s) = lLastScannedNumber;
			if ((double)((long)LREAL(*s)) == LREAL(*s)) {
				LINT(*s) = (long)LREAL(*s);
				LTYPE(*s) = LINTEGER_TY;
				LLEN(*s) = sizeof(long);
			} else {
				LTYPE(*s) = LREAL_TY;
				LLEN(*s) = sizeof(double);
			}
			break;
		default:
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
} /* _L2num */

/* ------------------ L2num ------------------- */
void
L2num( const PLstr s )
{
	switch (_Lisnum(s)) {
		case LINTEGER_TY:
			/*//LINT(*s) = atol( LSTR(*s) ); */
			LINT(*s) = (long)lLastScannedNumber;
			LTYPE(*s) = LINTEGER_TY;
			LLEN(*s) = sizeof(long);
			break;

		case LREAL_TY:
			/*///LREAL(*s) = strtod( LSTR(*s), NULL ); */
			LREAL(*s) = lLastScannedNumber;
/*
//// Numbers like 2.0 should be treated as real and not as integer
//// because in cases like factorial while give an error result
////			if ((double)((long)LREAL(*s)) == LREAL(*s)) {
////				LINT(*s) = (long)LREAL(*s);
////				LTYPE(*s) = LINTEGER_TY;
////				LLEN(*s) = sizeof(long);
////			} else {
*/
				LTYPE(*s) = LREAL_TY;
				LLEN(*s) = sizeof(double);
/*
////			}
*/
			break;

		default:
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
} /* L2num */

/* ----------------- Lrdint ------------------ */
long
Lrdint( const PLstr s )
{
	if (LTYPE(*s)==LINTEGER_TY) return LINT(*s);

	if (LTYPE(*s)==LREAL_TY) {
		if ((double)((long)LREAL(*s)) == LREAL(*s))
			return (long)LREAL(*s);
		else
			Lerror(ERR_INVALID_INTEGER,0);
	} else { /* LSTRING_TY */
		LASCIIZ(*s);
		switch (_Lisnum(s)) {
			case LINTEGER_TY:
				/*///return atol( LSTR(*s) ); */
				return (long)lLastScannedNumber;

			case LREAL_TY:
				/*///d = strtod( LSTR(*s), NULL );
				//////if ((double)((long)d) == d)
				//////	return (long)d; */
				if ((double)((long)lLastScannedNumber) == lLastScannedNumber)
					return (long)lLastScannedNumber;
				else
					Lerror(ERR_INVALID_INTEGER,0);
				break;

			default:
				Lerror(ERR_INVALID_INTEGER,0);
		}
	}
	return 0;	/* never gets here but keeps compiler happy */
} /* Lrdint */

/* ----------------- Lrdreal ------------------ */
double
Lrdreal( const PLstr s )
{
	if (LTYPE(*s)==LREAL_TY) return LREAL(*s);

	if (LTYPE(*s)==LINTEGER_TY)
		return (double)LINT(*s);
	else { /* LSTRING_TY */
		LASCIIZ(*s);
		if (_Lisnum(s)!=LSTRING_TY)
			/*///// return strtod( LSTR(*s), NULL ); */
			return lLastScannedNumber;
		else
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
	return 0.0;
} /* Lrdreal */
