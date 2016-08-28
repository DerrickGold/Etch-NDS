#ifndef _File_Browser
#define _File_Browser

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

//text buffer
char InfoText[256];
//last file loaded/saved
char LastFile[128];
char LastFont[128];

//file browser size scale in terms of scrolling text size
extern s8 FBScale;
extern s8 FB_FilesPerScreen;

s8 FB_OpenFile;//1 for open, -1 for delete
typedef enum{
	FB_Default,
	FB_Spaced,
	FB_Large
}FileBrowserStyle; 

typedef enum {
	FB_Image,
	FB_IntFont,
	FB_Font
}FileBrowserMode;

typedef enum{
	Not_Supported= (1<<0),
	Is_Folder= (1<<1)
}FileFlags;
//file browser
#define ScrollBarPal 0
typedef struct{
	char filename[128];
	//char name[128];
	char ext[4];
	s8 Flags;
	//bool Unsupported;
	//bool Folder;
}  FileList;
FileList *files;

typedef struct{
	u32 Width, Height, Bits;
	u32 Size, UnZipped;
}FileInformation;
FileInformation FileInfo;

typedef struct{
	char filename[128];
}  AudioList;
AudioList *Audio;

extern s8 LoadBmpPreview(char *filename);
extern s8 LoadEpfPreview(char *filename);
extern s8 BAG_FontFatLoad(char *filename,s8 slot,bool Preview);
extern void ShowFileBrowser(s8 mode);
extern void ScanFileSizes(int filenumber,s8 mode);

#endif