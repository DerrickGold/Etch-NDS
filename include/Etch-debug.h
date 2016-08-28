#ifndef _ETCHDEBUG
#define _ETCHDEBUG


//debug
typedef enum{
	Error_ImageAlloc = 1,
	Error_LayerAlloc,
	Error_LinesToWrite,
	Error_FontAlloc,
	Error_UndoAlloc,
	Error_BmpLoad,
	Error_EpfLoad,
	Error_RamInit,
	Error_GenAlloc,
	Error_OldIni,
	Error_Fat,
	Error_CreateFile,
	Error_FileExist,
	Error_BucketAlloc,
	Error_ShapeAlloc,
	Error_BrowserAlloc,
	Error_OggListAlloc,
	Error_UserSpecified,
	Error_RamError,
	Error_FadeError,
	Error_FontLoadAlloc,
	Error_NotSupportedImage
}ErrorTypes;

typedef enum{
	Location_CanvasSize=1,
	Location_Main,
	Location_Save,
	Location_ImgBrowser,
	Location_FontBrowser,
	Location_Color
}ErrorLocations;

extern void UserDumpDebug(void);
extern void BAG_Error_Print(s8 ErrorType);
#endif
