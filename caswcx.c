/*
===========================================================
 $Name:         caswcx.c

 $Desc:         Open MSX CAS files

 $Author:       NataliaPC <natalia.pujol@gmail.com>
 $Revision:     1.1

 $Comments:		https://github.com/nataliapc/casMSXwcx
===========================================================
*/

#include "wcxapi.h"
#include "win2nix_binds.h"

#include <assert.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __MINGW32__
#include <unistd.h>
#endif

#include "types.h"
#include "defs.h"
#include "resource.h"


/*===========================================================
	Constants
*/

#define CAS_PACKED 0x0001
#define FIXED_DATE ((1984 - 1980) << 25 | 1 << 21 | 1 << 16 | 0 << 11 | 0 << 5 | 0/2)

#define CAS_HDR_LEN       8
const char CAS_HEADER[] = "\x1F\xA6\xDE\xBA\xCC\x13\x7D\x74";

#define HDR_LEN           10
const char HEAD_BIN[]   = "\xD0\xD0\xD0\xD0\xD0\xD0\xD0\xD0\xD0\xD0";
const char HEAD_BAS[]   = "\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3";
const char HEAD_ASC[]   = "\xEA\xEA\xEA\xEA\xEA\xEA\xEA\xEA\xEA\xEA";

const char NAME_HBIN[]  = "HEADER_BIN";
const char NAME_HASC[]  = "HEADER_ASC";
const char NAME_HBAS[]  = "HEADER_BAS";
const char NAME_DATA[]  = "DATA";

const char CMT_HEAD_BIN[] = "Standard Binary Header";
const char CMT_HEAD_BAS[] = "Standard Tokenized Basic Header";
const char CMT_HEAD_ASC[] = "Standard ASCII Basic Header";
const char CMT_DATA[]     = "Standard Data Block";

const char szCfgKey[]     = "casMSXwcx";


/*===========================================================
	Structs
*/

typedef struct FileList_s {
	char   szFileName[MAX_PATH-9];	// File name translated
	dword  dwSize;					// File size in bytes
	byte   *cData;					// File data
	dword  crc32;					// Data CRC32
	char   *szComment;				// Text Comment
	struct FileList_s *lpNext;		// Next entry
} FileList_t;

typedef struct CAShandle_s {
	WCHAR		szCASname[MAX_PATH * 16];	// CAS file name
	dword		dwCASflags;					// Flags
	byte        *cRawCAS;
	long        lSize;
	FileList_t	*lpFileListHead;
	FileList_t	*lpFileListCur;
	FileList_t	*lpFileListThis;
} CAShandle_t;


/*===========================================================
	Global variables
*/

char	szCfgFile[MAX_PATH];	//Path to .INI config file
char	mInvalidReplacer;		//Char replacement if invalid one in "Found:" name
BOOL	mbEnableWarnings;		//Enable warnings


/*===========================================================
	Internal functions
*/
void GetCfgPath(void)
{
#ifdef WINDOWS
	char path[MAX_PATH];

	if (GetModuleFileName(GetModuleHandle( NULL ), path, sizeof(path))) {
		char *p = strrchr(path, '\\');
		if (p)
			*++p = '\0';
		else
			path[0] = '\0';
	} else
		path[0] = '\0';

	strcpy(szCfgFile, path);
	strcat(szCfgFile, "plugins\\wcx\\casMSX\\casMSX.ini");
#endif
}

dword crc32b(unsigned char *data, unsigned int len) {
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (len--) {
      byte = data[i];               // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

static void GetFoundString(FileList_t *pfl, char pInvalidReplacer, byte *pnameOut)
{
	int pos=10, lastValidPos=10;

	pnameOut[0] = '\0';
	for (pos=10; pos<16; pos++) {
		if (isalnum(pfl->cData[pos])) {
			pnameOut[pos-10] = pfl->cData[pos];
			lastValidPos = pos;
		} else {
			pnameOut[pos-10] = pInvalidReplacer;
		}
	}
	pnameOut[lastValidPos-9] = '\0';
}

static void FreeCAShandler(CAShandle_t *lpH)
{
	FileList_t *lpNext;

	free(lpH->cRawCAS);
	while (lpH->lpFileListHead) {
		lpNext = lpH->lpFileListHead->lpNext;
		free(lpH->lpFileListHead);
		lpH->lpFileListHead = lpNext;
	}
	free(lpH);
}

static int MakeCASlist(CAShandle_t *lpH)
{
	FILE *pFile;
	int rc = 0, idx = 1;
	long pos = 0, pos2;
	byte foundName[7];
	FileList_t *fl;
	char *pName, *pCmt;

	// Read CAS file
	pFile = _wfopen (lpH->szCASname, L"rb");
	if (pFile==NULL) return E_EOPEN;
	// Get file size
	fseek (pFile , 0 , SEEK_END);
	lpH->lSize = ftell (pFile);
	rewind (pFile);
	// Read CAS file
	lpH->cRawCAS = (byte*)malloc(lpH->lSize);
	if (lpH->cRawCAS==NULL) return E_NO_MEMORY;
	if (fread (lpH->cRawCAS, 1, lpH->lSize, pFile) != lpH->lSize) rc = E_EREAD;
	if (fclose(pFile)) rc = E_ECLOSE;
	if (strncmp(CAS_HEADER, (char*)lpH->cRawCAS, CAS_HDR_LEN) && lpH->lSize) rc = E_UNKNOWN_FORMAT;

	if (!rc) {
		// Search for CAS blocks
		while (!rc && pos < lpH->lSize) {
			pos += CAS_HDR_LEN;
			if (pos >= lpH->lSize) { rc = E_BAD_DATA; break; }
			// Create new block
			fl = (FileList_t*)malloc(sizeof(FileList_t));
			if (fl==NULL) { rc = E_NO_MEMORY; break; }
			memset(fl, 0, sizeof(FileList_t));
			// Search block end
			pos2 = pos;
			while (pos2 < lpH->lSize && memcmp(CAS_HEADER, &lpH->cRawCAS[pos2], CAS_HDR_LEN)) {
				pos2++;
			}
			// Found & Fill struct
			fl->dwSize = pos2-pos;
			fl->cData = lpH->cRawCAS + pos;
			fl->crc32 = crc32b(fl->cData, fl->dwSize);
			pName = pCmt = NULL;
			if (fl->dwSize==16) {
				GetFoundString(fl, mInvalidReplacer, foundName);
				if (!memcmp(HEAD_BIN, fl->cData, HDR_LEN)) {
					pName = (char*)NAME_HBIN;
					pCmt = (char*)CMT_HEAD_BIN;
				}
				if (!memcmp(HEAD_ASC, fl->cData, HDR_LEN)) {
					pName = (char*)NAME_HASC;
					pCmt = (char*)CMT_HEAD_ASC;
				}
				if (!memcmp(HEAD_BAS, fl->cData, HDR_LEN)) {
					pName = (char*)NAME_HBAS;
					pCmt = (char*)CMT_HEAD_BAS;
				}
				sprintf(fl->szFileName, "%02u_%s[%s].%08lx", idx, pName, foundName, fl->crc32);
				fl->szComment = pCmt;
			}
			if (pName==NULL) {
				sprintf(fl->szFileName, "%02u_%s.%08lx", idx, NAME_DATA, fl->crc32);
				fl->szComment = (char*)CMT_DATA;
			}

			// Set pointers
			if (lpH->lpFileListHead==NULL) lpH->lpFileListHead = fl;
			if (lpH->lpFileListCur) lpH->lpFileListCur->lpNext = fl;
			lpH->lpFileListCur = fl;
			fl->lpNext = NULL;
			// Pos to next block
			idx++;
			pos = pos2;
		}
	}

	if (rc) FreeCAShandler(lpH);
	return rc;
}

/*
=====================================================================
	Function name	: STDCALL OpenArchive
	Description	    : Main entry point to Windows Commander
	Return type		: WCX_API HANDLE
	Argument        : tOpenArchiveDataW *ArchiveData
*/
HANDLE CAS_OpenArchive(tOpenArchiveDataW *ArchiveData)
{
	CAShandle_t *lpH = (CAShandle_t *)malloc(sizeof(CAShandle_t));
	char szBuf[256] __attribute__((unused));

	// Check for out of memory
	if(lpH==NULL) {
		ArchiveData->OpenResult = E_NO_MEMORY;
		return 0;
	}
	memset(lpH, 0, sizeof(CAShandle_t));

#ifdef WINDOWS
	GetCfgPath();
	GetPrivateProfileString(szCfgKey, "EnableWarning", "1", szBuf, sizeof(szBuf), szCfgFile);
	mbEnableWarnings = atoi(szBuf);
	GetPrivateProfileString(szCfgKey, "InvalidCharReplacer", "_", szBuf, sizeof(szBuf), szCfgFile);
	mInvalidReplacer = szBuf[0];
#else
	mbEnableWarnings = 1;
	mInvalidReplacer = '_';
#endif

	// Copy archive name to handler
	wcslcpy(lpH->szCASname, ArchiveData->ArcName, sizeof(lpH->szCASname));

	// Default responding
	ArchiveData->OpenResult = E_BAD_ARCHIVE;

	// For Packer support
#ifdef PACKER_SUPPORT
	lpH->dwCASflags = CAS_PACKED;
#endif

	// Read and create CAS list
	if ((ArchiveData->OpenResult = MakeCASlist(lpH))) {
		free(lpH);
		return 0;
	}

	// Reset entry list
	lpH->lpFileListCur = lpH->lpFileListHead;

	return (HANDLE)lpH;
}

/*
=====================================================================
	Function name	: STDCALL ReadHeader
	Description	    : Totalcmd find out what files are in the archive.
	Return type		: int (error code or 0)
	Argument        : WCX_API HANDLE hArcData
	Argument        : tHeaderDataExW *HeaderData
*/
int CAS_ReadHeader(HANDLE hArcData, tHeaderDataExW *HeaderData)
{
	CAShandle_t *lpH = (CAShandle_t *)hArcData;
	FileList_t *fl;

	if (lpH->lpFileListCur) {
		fl = lpH->lpFileListCur;

		awfilenamecopy(HeaderData->FileName, fl->szFileName);

		HeaderData->FileTime = FIXED_DATE;
		HeaderData->FileAttr = 0x20;
		HeaderData->FileCRC  = fl->crc32;

		HeaderData->PackSize  = fl->dwSize + strlen(CAS_HEADER);
		HeaderData->UnpSize  = fl->dwSize;

		HeaderData->CmtBuf   = fl->szComment;
		HeaderData->CmtSize  = strlen(fl->szComment);
		HeaderData->CmtBufSize = 80;

		lpH->lpFileListThis = lpH->lpFileListCur;
		lpH->lpFileListCur = lpH->lpFileListCur->lpNext;

		return 0;
	}

	return E_END_ARCHIVE;
}

/*
=====================================================================
	Function name	: STDCALL ProcessFile
	Description	    : Should unpack the specified file or test the 
	                  integrity of the archive.
	Return type		: int (error code or 0)
	Argument        : WCX_API HANDLE hArcData
	Argument        : int Operation
	Argument        : char *DestPath
	Argument        : char *DestName
*/
int CAS_ProcessFile(HANDLE hArcData, int Operation, WCHAR *DestPath, WCHAR *DestName)
{
	FileList_t *fl = ((CAShandle_t *)hArcData)->lpFileListThis;
	FILE *pFile;

	switch (Operation) {
		case PK_SKIP:
		case PK_TEST:
			return 0;
		case PK_EXTRACT:
			if (DestName != NULL) {
				// Write Raw Block file
				if ((pFile=_wfopen (DestName, L"wb"))==NULL) return E_EOPEN;
				if (fwrite(fl->cData, 1, fl->dwSize, pFile) != fl->dwSize) return E_EWRITE;
				if (fclose(pFile)) return E_ECLOSE;
			} else {
				return E_EOPEN;
			}
			return 0;
	}
	return E_NOT_SUPPORTED;
}

/*
=====================================================================
	Function name	: STDCALL CloseArchive
	Description	    : Perform all necessary operations when archive 
	                  is about to be closed.
	Return type		: int (error code or 0)
	Argument        : WCX_API HANDLE hArcData
*/
int CAS_CloseArchive(HANDLE hArcData)
{
	CAShandle_t *lpH = (CAShandle_t *)hArcData;

	// If packed then delete temp file!
#ifdef PACKER_SUPPORT
	if (lpH->dwCASflags & CAS_PACKED)
		unlink(lpH->szCASname);
#endif

	//Free all inner Handler data
	FreeCAShandler(lpH);

	return 0;
}

/*
=====================================================================
	Function name	: PackFiles
	Description	    : Specifies what should happen when a user creates,
	                  or adds files to the archive.
	Return type		: int (error code or 0)
	Argument        : WCHAR *PackedFile
	Argument        : WCHAR *SubPath
	Argument        : WCHAR *SrcPath
	Argument        : WCHAR *AddList
	Argument        : int Flags
*/
int CAS_PackFiles (WCHAR *PackedFile, WCHAR *SubPath, WCHAR *SrcPath, WCHAR *AddList, int Flags)
{
	return E_NOT_SUPPORTED;
}

/*
=====================================================================
	Function name	: DeleteFiles
	Description	    : Delete file(s) from CAS file
	Return type		: STDCALL
	Argument        : WCHAR *PackedFile
	Argument        : WCHAR *DeleteList
*/
int CAS_DeleteFiles (WCHAR *PackedFile, WCHAR *DeleteList)
{
	CAShandle_t *lpH = (CAShandle_t *)malloc(sizeof(CAShandle_t));
	FileList_t *fl;
	char buff[MAX_PATH];
	unsigned int rc = 0;
	BOOL anyFileDeleted = FALSE;
	FILE *pFile;

	memset(lpH, 0, sizeof(CAShandle_t));
	wcslcpy(lpH->szCASname, PackedFile, sizeof(lpH->szCASname));

	if ((rc = MakeCASlist(lpH))) {
		free(lpH);
	}

	if (!rc) {
		while (*DeleteList) {
			wafilenamecopy(buff, DeleteList);
			// Search file to delete
			fl = lpH->lpFileListHead;
			do {
				if (!strcmp(buff, fl->szFileName)) {
					// Reset found node
					lpH->lSize -= fl->dwSize;
					fl->szFileName[0] = '\0';
					fl->cData = NULL;
					fl->dwSize = 0;
					anyFileDeleted = TRUE;
				}
				fl = fl->lpNext;
			} while (fl!=NULL);

			// Go to next file to delete
			while(*DeleteList) DeleteList++;
			DeleteList++;
		}
		if (anyFileDeleted) {
			// Save file without deleted blocks
			pFile = _wfopen(PackedFile, L"wb");
			fl = lpH->lpFileListHead;
			do {
				if (fl->cData != NULL) {
					fwrite(CAS_HEADER, 1, CAS_HDR_LEN, pFile);
					fwrite(fl->cData, 1, fl->dwSize, pFile);
				}
				fl = fl->lpNext;
			} while (fl!=NULL);
			fclose(pFile);
		}

		FreeCAShandler(lpH);
	}
/*
typedef struct FileList_s {
	char   szFileName[MAX_PATH-9];	// File name translated
	dword  dwSize;					// File size in bytes
	byte   *cData;					// File data
	dword  crc32;					// Data CRC32
	char   *szComment;				// Text Comment
	struct FileList_s *lpNext;		// Next entry
} FileList_t;
typedef struct CAShandle_s {
	WCHAR		szCASname[MAX_PATH * 16];	// CAS file name
	dword		dwCASflags;					// Flags
	byte        *cRawCAS;
	long        lSize;
	FileList_t	*lpFileListHead;
	FileList_t	*lpFileListCur;
	FileList_t	*lpFileListThis;
} CAShandle_t;
*/
	return rc;
}

int CAS_GetPackerCaps(void)
{
	// Return capabilities 
	return PK_CAPS_MULTIPLE|		// Archive can contain multiple files
	       PK_CAPS_DELETE;			// Can delete files
}
