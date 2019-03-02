#ifndef _DEFS_H
#define _DEFS_H

#ifdef assert
#undef assert
#endif

#ifdef DEBUG
#include <stdio.h>
//#define DebugString(x) OutputDebugString( __STR__(__LINE__)": " ),OutputDebugString( x ),OutputDebugString( "\n" )
#define DebugString(x) {FILE*out=fopen("Z:\\home\\natalia\\Programacion\\log.txt","a+t");fprintf(out,"%s\n",x);fclose(out);}
#define DebugStringW(x) {FILE*out=fopen("Z:\\home\\natalia\\Programacion\\log.txt","a+t");fwprintf(out,L"%s\n",x);fclose(out);}
#define __STR2__(x) #x
#define __STR__(x) __STR2__(x)
#define assert( x ) if( !(x) ) DebugString( "Assertion "#x" in line "__STR__(__LINE__) )
#define VERSIONTEXT "1.0"
#else
#define DebugString(x)
#define assert( x )
#define VERSIONTEXT "1.0"
#endif

#endif
