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
extern HANDLE CAS_OpenArchive(tOpenArchiveDataW *ArchiveData);
extern int CAS_ReadHeader(HANDLE hArcData, tHeaderDataExW *HeaderData);
extern int CAS_ProcessFile(HANDLE hArcData, int Operation, WCHAR *DestPath, WCHAR *DestName);
extern int CAS_CloseArchive(HANDLE hArcData);
extern int CAS_PackFiles (WCHAR *PackedFile, WCHAR *SubPath, WCHAR *SrcPath, WCHAR *AddList, int Flags);
extern int CAS_DeleteFiles (WCHAR *PackedFile, WCHAR *DeleteList);
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
	ArchiveData->OpenResult = E_NOT_SUPPORTED;
	return 0;
}

WCX_API HANDLE STDCALL OpenArchiveW(tOpenArchiveDataW *ArchiveData)
{
	DebugString( "open archiveW" );
	return CAS_OpenArchive(ArchiveData);
}

/*
	Totalcmd calls ReadHeader to find out what files are in the archive.
*/
WCX_API int	STDCALL ReadHeader(HANDLE hArcData, tHeaderData *HeaderData)
{
	DebugString( "read header" );
	return E_NOT_SUPPORTED;
}

WCX_API int STDCALL ReadHeaderExW(HANDLE hArcData, tHeaderDataExW* HeaderData)
{
	DebugString( "read headerExW" );
	return CAS_ReadHeader(hArcData, HeaderData);
}

/*
	ProcessFile should unpack the specified file or test the integrity of the archive.
*/
WCX_API int	STDCALL ProcessFile(HANDLE hArcData, int eOperation, char *szDestPath, char *szDestName)
{
	DebugString( "process file" );
	return E_NOT_SUPPORTED;
}

WCX_API int STDCALL ProcessFileW(HANDLE hArcData, int eOperation, WCHAR* szDestPath, WCHAR* szDestName)
{
	DebugString( "process fileW" );
	return CAS_ProcessFile(hArcData, eOperation, szDestPath, szDestName);
}

/*
	CloseArchive should perform all necessary operations when an archive is about to be closed.
*/
WCX_API int	STDCALL CloseArchive(HANDLE hArcData)
{
	DebugString( "close archive" );
	return CAS_CloseArchive(hArcData);
}

/*
	PackFiles specifies what should happen when a user creates, or adds files to the archive.
*/
WCX_API int	STDCALL PackFiles(char *PackedFile, char *SubPath, char *SrcPath, 
							  char *AddList, int Flags)
{
	DebugString( "pack files" );
	return E_NOT_SUPPORTED;
}

WCX_API int	STDCALL PackFilesW(WCHAR *PackedFile, WCHAR *SubPath, WCHAR *SrcPath, 
							  WCHAR *AddList, int Flags)
{
	DebugString( "pack filesW" );
	return CAS_PackFiles(PackedFile, SubPath, SrcPath, AddList, Flags);
}

/*
	DeleteFiles should delete the specified files from the archive.
*/
WCX_API int	STDCALL DeleteFiles(char *PackedFile, char *DeleteList)
{
	DebugString( "delete files" );
	return E_NOT_SUPPORTED;
}

WCX_API int	STDCALL DeleteFilesW(WCHAR *PackedFile, WCHAR *DeleteList)
{
	DebugString( "delete files" );
	return CAS_DeleteFiles(PackedFile, DeleteList);
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
