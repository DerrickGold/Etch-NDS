#ifndef _BG_DATA
#define _BG_DATA

#include "ram.h"
#include "Etch-debug.h"
#include "FileBrowser.h"
#include "minIni.h"
#include "all_gfx.h"
#include "ToolBar.h"
#include "FontBAG.h"
#include <fat.h>

//zip stuff
#include <zlib.h>
#include "zip.h"
#include "unzip.h"


#include "oggplay.h"

//#define USE_BGET //use memory allocator for ram

#ifdef USE_BGET
	#include "bget.h"
#endif


//keep defined for emulator use
#define AllowBrowseNitro 


#define UNDO_Width 400
#define UNDO_Height 300

#ifdef UNDO_Width
	#ifdef UNDO_Height
		#define UNDO_Buf_Size UNDO_Width*UNDO_Height*2
	#endif
#else 
	#define UNDO_Width 256
	#define UNDO_Height 192
#endif

//for custom controls
#define DrawFuncs 18
#define MaxControlChars 20

#define ColorSquarePals 0,1
#define TextHeight 8
#define LargeTextHeight 16
#define ClrMenuSprStrt 10
#define LayerButtonData 6,60

//functions
#define SET_FLAG(Var, Flag) (Var|=Flag)
#define RESET_FLAG(Var, Flag) (Var &= ~Flag)
#define CLEAR_FLAGS(Var,Val) (Var=Val)
#define GET_FLAG(Var, Flag) ((Var & Flag)!=0)

#define SCREEN_INDEX(x,y) ((x) +((y)<<8))
#define CANVAS_INDEX(x,y,w) ((x) + (w* (y)))

#define Trans24bit 1
#define Trans8bit 256
#define TransLim 32768


extern char Control_List[DrawFuncs][MaxControlChars];

FILE * bgfile;
//one for bottom screen and one for top
u16 bg_array[2][SCREEN_WIDTH*SCREEN_HEIGHT];
bool transLines[SCREEN_WIDTH*SCREEN_HEIGHT];
//screen buffers
s8 LayerOrder[10];
u16 *bglayer[10];
u16 *UndoBuf[10];
u8 *alphalayer[10];
u32 *ScreenBuf16c;

//saving
bool *LinesToWrite;
//24/16bit palettes
u16 Palettes[10];
//for loading 256 col pals
char* temp_pal;
u16 palette[256];
u8 OldGridVal;


typedef enum{
	//fading in status
	FADE_IN = (1<<0),
	FADING_IN = (1<<1),
	FADED_IN = (1<<2),
	
	//fading out status
	FADE_OUT = (1<<3),
	FADING_OUT = (1<<4),
	FADED_OUT = (1<<5),
	//set default alpha values for desired fade
	FADE_RESET = (1<<6)
}FADE_STATUS;

typedef struct{
	s8 Alpha1[2],Alpha2[2],Flags[2];
	s8 Style[2], ColWheel[2];
}FADE_STRUCT;
FADE_STRUCT Fade;


typedef enum{
	Layer_Menu=2,
	Layer_Grid=0,
	Layer_Draw=3
}MENULAYER;

typedef enum{
	SCROLL=1,
	COLMEN
}ACTIONDISP;

typedef struct{
	s32 Flag;
	u8 * Button;
}ACTIONS;
ACTIONS Display;
	

struct EPFheader{
	char Header[4];//read the EPF header
	s8 Bits;//image bits; 8,16 or 24
	u8 Layers;//number of layers in project
	s16 Width;//image width
	s16 Height;//image height
	u8 NColors;
};

typedef struct{
	u16 oldcolor;
	u16 newcolor;
	u32 nextfill;
	s32 *nextx;
	s32 *nexty;
	s32 Index, IndexStart,IndexEnd;
}fillinfo;
fillinfo Bucket;

typedef void (*function)(u8,s16,s16,s16,s16,u16,u8,bool);

typedef struct{
	s16 UndoX,UndoY,UndoLayer;
	fpos_t SlotPos[10];
	u16 * UndoBuf;
	FILE * UNDO;
}UNDOCACHE;
UNDOCACHE Cache;

typedef struct{
	s8 ToolUsed;
	s16 StylusX,StylusY;
	u16 OrigCol;
}UndoStats;
UndoStats UndoInfo[10];

typedef struct{
	s16 Height, Width;
	//scrolling information and canvas dimensions
	s32 ScrollX,ScrollY, CanvasX, CanvasY,HWScrollX,HWScrollY, TempCanvasX, TempCanvasY;
	//layers
	s8 NumOfLayers, LayerSelected, NumOfUndos, UndoNum;
	//undos
	s8 UndoStart,UndoEnd, UndoLayer[10],toolused;
	s16 UndoX[10],UndoY[10];
	//mini view stuff
	int XScale,YScale;
	s16 MiniViewX,MiniViewY, MiniViewTimer, UpdateView;
	//file browser
	s16 NumOfFiles,NumOfFolders, PreviewFolderCount;
	bool InitialSave;
	s32 CenterX,CenterY;
	s8 Bits;
	vu16 * ExtRam;
	//debug
	s8 PrgmLocation;
	
	u8 EraserPal;// color for eraser in 8bit
	
	s16 NColors;
}DSBMPINFO;
DSBMPINFO DSBmp;

typedef struct{
	s16 Zoom,Alpha;
	u16 StylusColor,Color[2];
	s16 X1,Y1,X2,Y2;
	s16 BGX1,BGY1,BGX2,BGY2;
	s8 Bits;
}DrawingInfo;
DrawingInfo Draw;


typedef enum{
	GRID_ON = (1<<0),
	GRID_DELETED = (1<<1),
	GRID_ZOOMED = (1<<2)
}GRID_STATUS;

typedef struct{
	bool FastScroll, ShowMiniView,keepAspect,FastSaving;
	s8 MaxBuffers, MaxLayers;
	s16 MiniviewX,MiniviewY,MiniviewWidth,MiniviewHeight, UpdateTimer;
	s16 PreviewWd,PreviewHt;
	int MaxBmpSize, MaxMemory,DefaultX,DefaultY;
	s8 BDRed,BDGreen,BDBlue;
	s8 TxtRed, TxtGreen, TxtBlue, LrgTxtR,LrgTxtG,LrgTxtB;
	char folderpath[256];//test file to load
	char imagepath[256];//test file to load
	char fontpath[256];
	char audiopath[256];
	char iniPath[256];
	s16 ZoomLevel;
	u32 MaxFiles;
	s8 HighlightRed,HighlightGreen,HighlightBlue;	
	u16 KeyboardCol[4];
	bool ScanIMGData;
	s8 IniVersion;
	u8 FontMinSize,FontMaxSize;
	u32 FontFileSize;
	bool ExternalRam;
	s8 RamSpeed;
	bool TestRam;
	bool EnableAudio;
	s16 Flood_Depth;
	bool EnableMenuFade;
	bool AutoGrid,GridFlags;
	bool SDCache;
}inisettingsinfo;
inisettingsinfo Settings;

typedef struct{
	char R_Button[MaxControlChars],L_Button[MaxControlChars],
	Right_Dpad[MaxControlChars],Left_Dpad[MaxControlChars],Down_Dpad[MaxControlChars],Up_Dpad[MaxControlChars],
	A_Button[MaxControlChars],B_Button[MaxControlChars],X_Button[MaxControlChars],Y_Button[MaxControlChars];
}customcontrols;
customcontrols Custom;


//drawing
extern void StylusScrollBg(u8* buttonHeld);
bool RedrawScreen;
extern void Fill16bitBlock(s16 x,s16 y,u32 height,u32 width, u16 color,u16* buffer);
extern u16 ColorBrightness(u16 color, s8 brightness);
extern u16 TransparentColor(bool screen,u16 color, s16 transp, u16 Color2);
extern void ViewSquare(s16 x1,s16 y1,s16 xfactor, s16 yfactor, u16 color);
extern bool New_Canvas(int width, int height);
extern void BAG_16bitDraw(u8 screen,s16 x,s16 y ,u16 color,bool blit);

extern void BAG_16bitCircleDraw(u8 screen,s16 x,s16 y ,u16 color);

extern void LineTool(u8 screen,u16 color);
extern void Rectangle(u8 screen,u16 color);
extern void LineRectangle(u8 screen,u16 color);
extern void SelectRectangle(u8 screen);
extern void ClearSelectBox(u8 screen);
extern void Circle(u8 screen,u16 color);
extern void UnfilledCircle(u8 screen,u16 color);
extern void EyeDropper(u8 screen);
extern void Draw16cGrid(u8 screen,u8 GridSizeX, u8 GridSizeY, u16 color1, u16 color2);
extern void LineRect16c(s8 screen, s16 x1, s16 y1, s16 x2, s16 y2);
extern void CheckGrid(void);
extern void DeleteGrid(void);
extern void UpdateZoom(u8 tool);
extern void BAG_Put16bitPixel(u8 screen, s16 x, s16 y, u16 color,bool Blitz,u16 alpha,bool disableAlphaCheck);
extern s8 BAG_OutputText(s8 screen,s16 x1,s16 y1,s16 x2, s16 y2,const char *string,s8 AA,int fontsize,s8 slot,u16 color);
extern void RefreshDrawing(s32 scrollx, s32 scrolly,bool Fast);
extern void UpdateBothScreens(void);
inline void ClearScreen(bool screen);
extern void CanvasMenu(void);
extern void CheckFill(s32 x, s32 y, u16 color);
//loading/saving
extern void CalcAvailableLayers(int width, int height);
extern void SaveMenu(void);
extern s8 LoadBmp(char *filename);
extern s8 LoadEPF(char *filename);
extern int screenshotbmp(char* filename, s8 Bits);
extern void SaveEPFFile(char *filename);
extern void CreateSaveScreen(char *filename,bool write);
extern void DeleteSaveScreen(void);

extern void CalculateScale(int imgWidth,int imgHeight,s16 windowWd,s16 windowHt);
extern void TimedReloadMiniView(s16 time);
extern void ReloadMiniView(s16 x1,s16 y1, s16 windowWd, s16 windowHt);
extern void MiniViewSection(s16 x1,s16 y1);
extern void ShowMemory(void);
//controls
extern void ScrollBg(void);
extern void Controls(void);
extern void ToolBoxControls(void);
//undos
extern void CheckSaveUndo(void);
extern void CheckUndo(void);
extern void UndoTool(void);

//audio
extern void UpdateOgg(void);
extern void PlayOgg(const char* folder,s32 track);

//interface
typedef struct{
	u8 X,Y;
	s8 Screen,Sprite, Pal;
}guispriteinfo;
guispriteinfo ColorSquares[2];
guispriteinfo FBScrollBar;
guispriteinfo ColorWheel[4];
guispriteinfo PalSliders[2];
guispriteinfo LayerBar[2];
guispriteinfo FFCancel;

typedef struct{
	s16 X,Y;
	s8 Type, Palette,Sprite,Screen,Created;
}styluscurs;
styluscurs Cursor;

typedef struct{
	u8 x,y;
}palettesquareinfo;
palettesquareinfo PalPos[256];

extern void BAG_16cClearZone(u8 screen, s16 x1, s16 y1, s16 x2, s16 y2, u32 *buffer);
extern void BAG_16cPutPixel(u8 screen, s16 x, s16 y, u32 color, u32* buffer);
extern void LoadStatsBar(void);
extern void UpdateStatsBar(void);
extern void LoadMainInterFace(void);
extern void FastMainInterFace(void);
extern void RunColorInterface(u8* ButtonHeld);
extern void HideSprite(bool screen,int sprite,bool hide);
extern void UpdateColorSquares(bool swap);
extern void FadeInterface(s8 InOut, bool cw);
extern void FadeInInterface(bool cw);
extern void FadeOutInterface(bool cw);
extern void CreateKBMenu(void);
extern void LoadingBar(bool screen, s16 x,s16 y,s16 width,s16 height,u16 color1,u16 color2,int count, int total);
extern void BitModeCheckBox(s8 bitmode);
extern void RunToolBarInterFace(void);
extern guispriteinfo CreateTextButton(s8 screen, u8 obj_num, u8 pal_num, s16 x, s16 y, char *text,s8 textX, s8 textY,u16 color);
//cursor stuff
extern void LoadStylusCursor(s8 screen,s8 palette, s8 sprite,s8 tool, s16 x, s16 y);
extern void SetCursorXY(s16 x, s16 y);
extern void SetCursorFrame(s8 frame);
extern void DeleteCursor(void);

extern const unsigned char cw1_Sprite[4096] _GFX_ALIGN; // 16bit Sprite
extern const unsigned char cw2_Sprite[4096] _GFX_ALIGN; // 16bit Sprite
extern const unsigned char cw3_Sprite[4096] _GFX_ALIGN; // 16bit Sprite
extern const unsigned char cw4_Sprite[4096] _GFX_ALIGN; // 16bit Sprite

extern const unsigned short sprite1_GFX[4096] __attribute__ ((aligned (4)));
extern const unsigned short sprite2_GFX[4096] __attribute__ ((aligned (4)));
extern const unsigned short sprite3_GFX[4096] __attribute__ ((aligned (4)));
extern const unsigned short sprite4_GFX[4096] __attribute__ ((aligned (4)));

#endif
