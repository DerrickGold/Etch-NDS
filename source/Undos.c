#include <PA9.h>
#include "bgdata.h"
//============================================================================================================================================================
//Normal Undo 
//============================================================================================================================================================
void SaveBucketUndo(int slot){
	DSBmp.UndoX[slot]=DSBmp.ScrollX-((UNDO_Width>>1)-(SCREEN_WIDTH>>1));
	DSBmp.UndoY[slot]=DSBmp.ScrollY-((UNDO_Height>>1)-(SCREEN_HEIGHT>>1));
		
	s32 x=((Stylus.X*Draw.Zoom)>>8)+DSBmp.HWScrollX+DSBmp.ScrollX-DSBmp.CanvasX;
	s32 y=((Stylus.Y*Draw.Zoom)>>8)+DSBmp.HWScrollY+DSBmp.ScrollY-DSBmp.CanvasY;	
	UndoInfo[DSBmp.UndoEnd].StylusX = x;
	UndoInfo[DSBmp.UndoEnd].StylusY = y;
	UndoInfo[DSBmp.UndoEnd].OrigCol = bglayer[LayerOrder[DSBmp.LayerSelected]][CANVAS_INDEX(x,y,DSBmp.Width)];
}
//captures screen before undo
void SaveUndo(int slot){
	UndoInfo[slot].ToolUsed = Box.ToolSelected;
	DSBmp.UndoLayer[slot]=DSBmp.LayerSelected;
	if(UndoInfo[slot].ToolUsed!= TB_Bucket){
		int y=0,x=0;
		#ifndef UNDO_Buf_Size
			DSBmp.UndoX[slot]=DSBmp.ScrollX;
			DSBmp.UndoY[slot]=DSBmp.ScrollY;
			if(Box.ToolSelected!=TB_Bucket){
				for(y=0;y<192;y++)memcpy ((u16*)&UndoBuf[slot][(y *SCREEN_WIDTH)],&bglayer[LayerOrder[DSBmp.LayerSelected]][DSBmp.ScrollX + ((DSBmp.ScrollY+y)* DSBmp.Width)],SCREEN_WIDTH*2);
			}
		#else
			DSBmp.UndoX[slot]=DSBmp.ScrollX-((UNDO_Width>>1)-(SCREEN_WIDTH>>1));
			DSBmp.UndoY[slot]=DSBmp.ScrollY-((UNDO_Height>>1)-(SCREEN_HEIGHT>>1));
			
			s16 TempWidth=UNDO_Width,Height=UNDO_Height;
			s16 OffsetX=0;
			if(DSBmp.Height<SCREEN_HEIGHT)Height=DSBmp.Height;
			if(DSBmp.Width<SCREEN_WIDTH){
				if(DSBmp.CanvasX<0)TempWidth=DSBmp.Width+DSBmp.UndoX[slot];
				else TempWidth=DSBmp.Width;
			}
			if(DSBmp.UndoX[slot]+UNDO_Width > DSBmp.Width){
				TempWidth=UNDO_Width-(DSBmp.UndoX[slot]+UNDO_Width-DSBmp.Width);
			}
			else if(DSBmp.UndoX[slot]<0){
				OffsetX=abs(DSBmp.UndoX[slot]);
				TempWidth=UNDO_Width+DSBmp.UndoX[slot];
			}

			for(y=0;y<=UNDO_Height;y++){
				if(DSBmp.UndoY[slot]+y>=0 && DSBmp.UndoY[slot]+y<DSBmp.Height){
					if(Settings.ExternalRam){
						for(x=0;x<TempWidth;x++){
							UndoBuf[slot][x+(y *UNDO_Width)] = bglayer[LayerOrder[DSBmp.LayerSelected]][x+OffsetX+DSBmp.UndoX[slot] + ((DSBmp.UndoY[slot]+y)* DSBmp.Width)];
						}
					}
					else
						memcpy(&UndoBuf[slot][(y *UNDO_Width)],&bglayer[LayerOrder[DSBmp.LayerSelected]][OffsetX+DSBmp.UndoX[slot] + ((DSBmp.UndoY[slot]+y)* DSBmp.Width)],TempWidth<<1);
				}
			}	
		#endif
	}
	else SaveBucketUndo(slot);
}
void UndoBucket(int slot){
	LoadStylusCursor(0,3,1,UndoInfo[slot].ToolUsed,(256>>1)-(16>>1),(192>>1)-(16>>1));
	u8 TempLayer = DSBmp.LayerSelected;
	DSBmp.LayerSelected = DSBmp.UndoLayer[slot];
	FFCancel = CreateTextButton(0, 4, 4, (256>>1)-(32>>1), (192>>1)+16,"Cancel",3, 4,PA_RGB(0,0,0));
	CheckFill(UndoInfo[slot].StylusX,UndoInfo[slot].StylusY,UndoInfo[slot].OrigCol);
	PA_DeleteSprite(FFCancel.Screen,FFCancel.Sprite);		
	DSBmp.LayerSelected = TempLayer;
	DeleteCursor();
	RefreshDrawing(DSBmp.ScrollX,DSBmp.ScrollY,0);
	DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);	
}

void UndoSlot(int slot){
	if(UndoInfo[slot].ToolUsed!= TB_Bucket){
		int y=0,x=0;
		#ifndef UNDO_Buf_Size
			for(y=0;y<192;y++)memcpy ((u16*)&bglayer[LayerOrder[DSBmp.UndoLayer[slot]]][DSBmp.UndoX[slot] + ((DSBmp.UndoY[slot]+y)* DSBmp.Width)],&UndoBuf[slot][(y *SCREEN_WIDTH)],SCREEN_WIDTH*2);
			RefreshDrawing(DSBmp.UndoX[slot],DSBmp.UndoY[slot],0);
		#else
			s16 TempWidth=UNDO_Width,Height=UNDO_Height;
			s16 OffsetX=0;
			if(DSBmp.Height<SCREEN_HEIGHT)Height=DSBmp.Height;
			if(DSBmp.Width<SCREEN_WIDTH){
				if(DSBmp.CanvasX<0)TempWidth=DSBmp.Width+DSBmp.UndoX[slot];
				else TempWidth=DSBmp.Width;
			}
			if(DSBmp.UndoX[slot]+UNDO_Width > DSBmp.Width){
				TempWidth=UNDO_Width-(DSBmp.UndoX[slot]+UNDO_Width-DSBmp.Width);
			}
			else if(DSBmp.UndoX[slot]<0){
				OffsetX=-DSBmp.UndoX[slot];
				TempWidth=UNDO_Width+DSBmp.UndoX[slot];
			}	
		
			for(y=0;y<=UNDO_Height;y++){
				if(DSBmp.UndoY[slot]+y>=0 && DSBmp.UndoY[slot]+y<DSBmp.Height){
					if(Settings.ExternalRam){
						for(x=0;x<TempWidth;x++){
							bglayer[LayerOrder[DSBmp.LayerSelected]][x+OffsetX+DSBmp.UndoX[slot] + ((DSBmp.UndoY[slot]+y)* DSBmp.Width)] = UndoBuf[slot][x+(y *UNDO_Width)];
						}
					}
					else 
						memcpy(&bglayer[LayerOrder[DSBmp.UndoLayer[slot]]][OffsetX + DSBmp.UndoX[slot] + ((DSBmp.UndoY[slot]+y)* DSBmp.Width)],&UndoBuf[slot][(y *UNDO_Width)],TempWidth<<1);
				}
			}
			RefreshDrawing(DSBmp.UndoX[slot]+((UNDO_Width>>1)-(SCREEN_WIDTH>>1)),DSBmp.UndoY[slot]+((UNDO_Height>>1)-(SCREEN_HEIGHT>>1)),0);
		#endif
	}
	else UndoBucket(slot);

	DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	ReloadMiniView(Settings.MiniviewX,Settings.MiniviewY,Settings.MiniviewWidth,Settings.MiniviewHeight);	
	ViewSquare(DSBmp.MiniViewX,DSBmp.MiniViewY,DSBmp.XScale,DSBmp.YScale,0);
}
//============================================================================================================================================================
//Experimental unfinished FAT undo caching
//============================================================================================================================================================
void Cache_SaveUndo(int slot){
	int y=0;
	/*if(Cache.SlotPos[slot]==0)Cache.UNDO = fopen("fat:/UndoTemp.etch", "a");//open file to append to 
	else {
		Cache.UNDO = fopen("fat:/UndoTemp.etch", "r+");
		fsetpos (Cache.UNDO,&Cache.SlotPos[slot]);
	}*/
	Cache.UNDO = fopen("fat:/UndoTemp.etch", "r+");
	if(!Cache.UNDO){
		Cache.UNDO = fopen("fat:/UndoTemp.etch", "wb");
	}
	if(Cache.SlotPos[slot])fsetpos (Cache.UNDO,&Cache.SlotPos[slot]);
	if(Cache.UNDO){
		fseek (Cache.UNDO , slot*(UNDO_Buf_Size+6) , SEEK_SET );
		fgetpos (Cache.UNDO,&Cache.SlotPos[slot]);
		//setup layer for undo
		Cache.UndoLayer=DSBmp.LayerSelected;
		fwrite (&Cache.UndoLayer,1, sizeof(Cache.UndoLayer),Cache.UNDO);
		//Setup x position for undo
		Cache.UndoX=DSBmp.ScrollX-((UNDO_Width>>1)-(SCREEN_WIDTH>>1));
		if(Cache.UndoX<0)Cache.UndoX=0;
		else if(Cache.UndoX>(DSBmp.Width-UNDO_Width))Cache.UndoX=DSBmp.Width-UNDO_Width;
		fwrite (&Cache.UndoX,1, sizeof(Cache.UndoX),Cache.UNDO);
		
		//setup y position for undo
		Cache.UndoY=DSBmp.ScrollY-((UNDO_Height>>1)-(SCREEN_HEIGHT>>1));
		if(Cache.UndoY<0)Cache.UndoY=0;
		else if(Cache.UndoY>(DSBmp.Height-UNDO_Height))Cache.UndoY=DSBmp.Height-UNDO_Height;
		fwrite (&Cache.UndoY,1, sizeof(Cache.UndoY),Cache.UNDO);
		//write the undo info
		for(y=0;y<UNDO_Height;y++){
			memmove ((u16*)&Cache.UndoBuf[(y *SCREEN_WIDTH<<1)],&bglayer[LayerOrder[DSBmp.LayerSelected]][DSBmp.UndoX[slot] + ((DSBmp.UndoY[slot]+y)* DSBmp.Width)],UNDO_Width<<1);
		}
		fwrite (&Cache.UndoBuf,1,UNDO_Buf_Size,Cache.UNDO);
		rewind(Cache.UNDO);
		fclose(Cache.UNDO);
	}
}

void Cache_UndoSlot(int slot){
	int y=0;
	Cache.UNDO = fopen("fat:/UndoTemp.etch", "r");//open file to append to 

	if(Cache.UNDO){
		fsetpos (Cache.UNDO,&Cache.SlotPos[slot]);
		//fseek (Cache.UNDO , slot*(UNDO_Buf_Size+6) , SEEK_SET );
		//read layer for undo
		fread (&Cache.UndoLayer,1, sizeof(Cache.UndoLayer),Cache.UNDO);		
		//read x position for undo
		fread (&Cache.UndoX,1, sizeof(Cache.UndoX),Cache.UNDO);
		//read y position for undo
		fread (&Cache.UndoY,1, sizeof(Cache.UndoY),Cache.UNDO);
		//read undo info 
		fread (&Cache.UndoBuf,1,UNDO_Buf_Size,Cache.UNDO);
		rewind(Cache.UNDO);
		fclose(Cache.UNDO);
		
		for(y=0;y<UNDO_Height;y++)memcpy ((u16*)&bglayer[LayerOrder[Cache.UndoLayer]][Cache.UndoX + ((Cache.UndoY+y)* DSBmp.Width)],&Cache.UndoBuf[(y *SCREEN_WIDTH<<1)],UNDO_Width<<1);
		RefreshDrawing(Cache.UndoX+((UNDO_Width>>1)-(SCREEN_WIDTH>>1)),Cache.UndoY+((UNDO_Height>>1)-(SCREEN_HEIGHT>>1)),0);
		DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
		ReloadMiniView(Settings.MiniviewX,Settings.MiniviewY,Settings.MiniviewWidth,Settings.MiniviewHeight);	
		ViewSquare(DSBmp.MiniViewX,DSBmp.MiniViewY,DSBmp.XScale,DSBmp.YScale,0);
	}
}

//============================================================================================================================================================
//The ring buffer setup
//============================================================================================================================================================
void CheckSaveUndo(void){
	if(DSBmp.NumOfUndos>0){
		++DSBmp.UndoEnd;
		if(DSBmp.UndoEnd>DSBmp.NumOfUndos)DSBmp.UndoEnd=0;
		if(DSBmp.UndoEnd==DSBmp.UndoStart){
			DSBmp.UndoStart++;
		}
		if(DSBmp.UndoStart>DSBmp.NumOfUndos)DSBmp.UndoStart=0;
		if(Settings.SDCache==0)SaveUndo(DSBmp.UndoEnd);
		else Cache_SaveUndo(DSBmp.UndoEnd);
	}
}
void CheckUndo(void){
	if(DSBmp.NumOfUndos>0){
		if(DSBmp.UndoEnd!=DSBmp.UndoStart){
			if(Settings.SDCache==0)UndoSlot(DSBmp.UndoEnd);
			else Cache_UndoSlot(DSBmp.UndoEnd);
			--DSBmp.UndoEnd;
			if(DSBmp.UndoEnd<0)DSBmp.UndoEnd=(DSBmp.NumOfUndos);
		}
	}
}
