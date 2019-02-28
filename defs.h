#ifndef _DEFS_H
#define _DEFS_H

#ifdef assert
#undef assert
#endif

#ifdef DEBUG
#define DebugString(x) OutputDebugString( __STR__(__LINE__)": " ),OutputDebugString( x ),OutputDebugString( "\n" )
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
