#ifndef TOOLBARDATA_H
#define TOOLBARDATA_H



//number of Tools
#define MainTools 10
#define PALNUM 15

#define SET_FLAG(Var, Flag) (Var|=Flag)
#define RESET_FLAG(Var, Flag) (Var &= ~Flag)
#define CLEAR_FLAGS(Var,Val) (Var=Val)
#define GET_FLAG(Var, Flag) ((Var & Flag)!=0)

typedef enum{
	TB_Move = 1,
	TB_Pencil,
	TB_EyeDropper,
	TB_Bucket,
	TB_Shapes,
	TB_Marquis,
	TB_Eraser,
	TB_Zoom,
	TB_Text,
	TB_Grid
}ToolBar_Tools;

typedef enum{
	TB_State_Screen = (1<<0),
	TB_State_Created = (1<<1),
	TB_State_Move = (1<<2),
	
	TB_State_EpandMain = (1<<3),
	TB_State_MainExpanded = (1<<4),
	TB_State_CollapseMain = (1<<5),
	TB_State_MainCollapsed = (1<<6),
	
	TB_State_ExpandSub = (1<<7),
	TB_State_SubExpanded = (1<<8),
	TB_State_CollapseSub = (1<<9),
	TB_State_SubCollapsed = (1<<10),
	
	TB_State_ExpandSlide =(1<<11),
	TB_State_SlideExpanded = (1<<12),
	TB_State_CollapseSlide =(1<<13),
	TB_State_SlideCollapsed = (1<<14),
	TB_State_ForceSlideCollapse = (1<<15),
	TB_State_UpdateJump = (1<<16),
	TB_State_Hide = (1<<17),
	TB_State_StylusPress = (1<<18)
}ToolBar_State;
	

typedef struct{
	u8 StartSprite;
	bool Created;
	u32 Flags;
	u8 ToolSelected,ToolMode, OldTool, OldMode,DestTool,DestMode;
	s16 X,Y;
	s16 MediaX,MediaY;
	u16 Color;
	u8 Sprites,BarSprite;
	struct{
		u8 SubIcons[10];
		s16 SliderValue;
		s8 SliderY;
		s16 MaxVal;
	}Tool[MainTools+1];
	u8 SwapPal;
	s16 Length;
	s8 ExpandTimer;
}ToolBoxInfo;
ToolBoxInfo Box;

extern void DeleteToolBar(void);
#define ResetTB(void) DeleteToolBar();
extern void InitToolBar(s8 screen, s16 x, s16 y, u16 color,u8 StartSprite);
#define InitTB(screen, color,x, y,start) InitToolBar(screen, x,y, color,start);
extern void CreateToolBar(void);
#define LoadTB(x,y) CreateToolBar();
extern void UpdateToolBar(void);
#define UpdateTB(void) UpdateToolBar();
extern void JumpToTool(s8 Tool,s8 subtool);

extern void FastDelTB(void);
#endif
