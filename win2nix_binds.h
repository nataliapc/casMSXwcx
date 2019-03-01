
#ifndef _WIN2NIX_BINDS_H_
#define _WIN2NIX_BINDS_H_
#include <stdint.h>

#if !defined(WIN32) && !defined(WIN64) & !defined(WINDOWS)    // [SLsgXH2E6y6a] //{
	#include <stdbool.h>

//	#define __int64 long long
	typedef int         BOOL;
	typedef uint8_t     BYTE;
	typedef uint16_t    WORD;
	typedef uint32_t    DWORD;
	typedef uint16_t    WCHAR;
	typedef void*       LPVOID;
	typedef void*       HINSTANCE;
	typedef void*       HANDLE;
	typedef void*       HWND;
//	typedef int64_t     LONGLONG;
//	typedef long        LONG;
	typedef uint32_t    UINT;

	#define STDCALL __stdcall
	#define FALSE false
	#define TRUE true
	#define MAX_PATH 260
	// cant avoid this define, wcxhead uses it.
	#define __stdcall
	#define APIENTRY
	#define WCX_API
	#define INVALID_HANDLE_VALUE ((void*)-1)

	//https://docs.microsoft.com/en-us/windows/desktop/dlls/dllmain
	enum{
		DLL_PROCESS_DETACH = 0,
		DLL_PROCESS_ATTACH,
		DLL_THREAD_ATTACH,
		DLL_THREAD_DETACH,
	};

	// // From Windows headers, MSDN.
	// // >> Contains a 64-bit value representing the number of 100-nanosecond
	// // >> intervals since January 1, 1601 (UTC).
	//typedef struct _FILETIME {
	//	DWORD dwLowDateTime;
	//	DWORD dwHighDateTime;
	//} FILETIME, *PFILETIME;

	//typedef struct _SYSTEMTIME {
	//	WORD wYear;
	//	WORD wMonth;
	//	WORD wDayOfWeek;
	//	WORD wDay;
	//	WORD wHour;
	//	WORD wMinute;
	//	WORD wSecond;
	//	WORD wMilliseconds;
	//} SYSTEMTIME, *PSYSTEMTIME;

#endif    // [SLsgXH2E6y6a] //}

#endif //_WIN2NIX_BINDS_H_
