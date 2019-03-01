//==============================
// wcxapi.c
// WCX DLL Interface
//==============================
#ifdef __cplusplus
extern "C" {
#endif

#include "wcxapi.h"
#include "defs.h"
#include "resource.h"

tProcessDataProc ProcessDataProc;

extern HINSTANCE	hDllInst;

extern void GetCfgPath(void);
extern HANDLE CAS_OpenArchive(tOpenArchiveData *ArchiveData);
extern int CAS_ReadHeader(HANDLE hArcData, tHeaderData *HeaderData);
extern int CAS_ProcessFile(HANDLE hArcData, int Operation, char *DestPath, char *DestName);
extern int CAS_CloseArchive(HANDLE hArcData);
extern int CAS_PackFiles (char *PackedFile, char *SubPath, char *SrcPath, char *AddList, int Flags);
extern int CAS_DeleteFiles (char *PackedFile, char *DeleteList);
extern int CAS_GetPackerCaps(void);

//------------[ DLL API ]--------------------------

BOOL APIENTRY DllMain( HANDLE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
    if (!hinstDLL)
        return FALSE;

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            DebugString( "process attach" );
			GetCfgPath();
            break;
        case DLL_PROCESS_DETACH:
            DebugString( "process detach" );
            break;
        case DLL_THREAD_ATTACH:
            DebugString( "thread attach" );
            break;
        case DLL_THREAD_DETACH:
            DebugString( "thread detach" );
            break;
    }

    return TRUE;
}

/*
	OpenArchive should perform all necessary operations when an archive is to be opened.
*/
WCX_API HANDLE STDCALL OpenArchive(tOpenArchiveData *ArchiveData)
{
	DebugString( "open archive" );
	return CAS_OpenArchive( ArchiveData );
}
/*
WCX_API HANDLE STDCALL OpenArchiveW(tOpenArchiveDataW *ArchiveData)
{
	return 0;
}*/

/*
	Totalcmd calls ReadHeader to find out what files are in the archive.
*/
WCX_API int	STDCALL ReadHeader(HANDLE hArcData, tHeaderData *HeaderData)
{
	DebugString( "read header" );
	return CAS_ReadHeader( hArcData, HeaderData );
}
/*
WCX_API int STDCALL ReadHeaderEx(HANDLE hArcData, tHeaderDataEx* HeaderData)
{
	return E_NOT_SUPPORTED;
}

WCX_API int STDCALL ReadHeaderExW(HANDLE hArcData, tHeaderDataExW* HeaderData)
{
	return E_NOT_SUPPORTED;
}*/

/*
	ProcessFile should unpack the specified file or test the integrity of the archive.
*/
WCX_API int	STDCALL ProcessFile(HANDLE hArcData, int eOperation, char *szDestPath, char *szDestName)
{
	DebugString( "process file" );
	return CAS_ProcessFile( hArcData, eOperation, szDestPath, szDestName );
}
/*
WCX_API int STDCALL ProcessFileW(HANDLE hArcData, int eOperation, WCHAR* szDestPath, WCHAR* szDestName)
{
	return E_NOT_SUPPORTED;
}*/

/*
	CloseArchive should perform all necessary operations when an archive is about to be closed.
*/
WCX_API int	STDCALL CloseArchive(HANDLE hArcData)
{
	DebugString( "close archive" );
	return CAS_CloseArchive( hArcData );
}

/*
	PackFiles specifies what should happen when a user creates, or adds files to the archive.
*/
WCX_API int	STDCALL PackFiles(char *PackedFile, char *SubPath, char *SrcPath, 
							  char *AddList, int Flags)
{
	DebugString( "pack files" );
	return CAS_PackFiles( PackedFile, SubPath, SrcPath, AddList, Flags );
}

/*
	DeleteFiles should delete the specified files from the archive.
*/
WCX_API int	STDCALL DeleteFiles(char *PackedFile, char *DeleteList)
{
	DebugString( "delete files" );
	return CAS_DeleteFiles( PackedFile, DeleteList );
}

/*
	GetPackerCaps tells Totalcmd what features your packer plugin supports.
*/
//WCX_API int STDCALL GetPackerCaps(void)
int __stdcall GetPackerCaps(void)
{
	DebugString( "get packer caps." );
	return CAS_GetPackerCaps();
}

/*
	ConfigurePacker gets called when the user clicks the Configure button from within 
	"Pack files..." dialog box in Totalcmd.
*/
WCX_API void STDCALL ConfigurePacker (HWND Parent, HINSTANCE DllInstance)
{
	// Not implemented
}

/*
	SetChangeVolProc allows you to notify user about changing a volume when packing files.
*/
WCX_API void STDCALL SetChangeVolProc(HANDLE hArcData, tChangeVolProc pChangeVolProc1)
{
	// Not implemented
}

/*
	SetProcessDataProc allows you to notify user about the progress when you un/pack files.	
*/
WCX_API void STDCALL SetProcessDataProc(HANDLE hArcData, tProcessDataProc pProcessDataProc)
{
	ProcessDataProc = pProcessDataProc;
}

#ifdef __cplusplus
}
#endif
