/*
//Copyright (C) 2002 Peter Bakota
========================================
 $Name:         wcxapi.h

 $Desc:         Windows Commander API definitions

 $Author:       Peter Bakota <bakota@tippnet.co.yu>
 $Revision:     1
 $Date:         13.01.2002 8:38:08 PM
 $Comments:     This code based on Christian Ghisler (support@ghisler.com) sources
========================================
*/
#ifndef __WCXAPI_H
#define __WCXAPI_H


#ifdef __cplusplus
extern "C" {
#endif

#include "cunicode.h"
#include "wcxhead.h"

#ifdef WINDOWS
	#define WCX_API __declspec(dllexport)
	#define STDCALL __stdcall
#endif



// Windows Commander Interface
// Mandatory
WCX_API HANDLE	STDCALL OpenArchive( tOpenArchiveData *ArchiveData);
WCX_API HANDLE	STDCALL OpenArchiveW (tOpenArchiveDataW *ArchiveData);
WCX_API int		STDCALL ReadHeader (HANDLE hArcData, tHeaderData *HeaderData);
WCX_API int		STDCALL ReadHeaderEx (HANDLE hArcData, tHeaderDataEx* HeaderData);
WCX_API int		STDCALL ReadHeaderExW (HANDLE hArcData, tHeaderDataExW* HeaderData);
WCX_API int		STDCALL ProcessFile (HANDLE hArcData, int eOperation, char *szDestPath, char *szDestName);
WCX_API int		STDCALL ProcessFileW (HANDLE hArcData, int eOperation, WCHAR* szDestPath, WCHAR* szDestName);
WCX_API int		STDCALL CloseArchive (HANDLE hArcData);
WCX_API void	STDCALL ConfigurePacker (HWND Parent, HINSTANCE DllInstance);
WCX_API void	STDCALL SetChangeVolProc (HANDLE hArcData, tChangeVolProc pChangeVolProc1);
WCX_API void	STDCALL SetProcessDataProc (HANDLE hArcData, tProcessDataProc pProcessDataProc);
// Optional
WCX_API int		STDCALL PackFiles (char *PackedFile, char *SubPath, char *SrcPath, char *AddList, int Flags);
WCX_API int		STDCALL PackFilesW (WCHAR *PackedFile, WCHAR *SubPath, WCHAR *SrcPath, WCHAR *AddList, int Flags);
WCX_API int		STDCALL DeleteFiles (char *PackedFile, char *DeleteList);
WCX_API int		STDCALL DeleteFilesW (WCHAR *PackedFile, WCHAR *DeleteList);
WCX_API BOOL	STDCALL CanYouHandleThisFile (char* szFileName);
//WCX_API BOOL	STDCALL CanYouHandleThisFileW (WCHAR* szFileName);
//WCX_API int		STDCALL GetPackerCaps (void);
int __stdcall GetPackerCaps (void);

#ifdef __cplusplus
}
#endif

#endif //!__WCX_API
