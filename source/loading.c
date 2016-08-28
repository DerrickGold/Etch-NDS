#include <PA9.h>
#include "bgdata.h"
#include "stdlib.h"
//======================================================================================================================================================
//My Modified font loading- restricts to certain sizes
//======================================================================================================================================================
s8 BAG_FontFatLoad(char *filename,s8 slot,bool Preview){
	if(Font_FatLoad(filename,slot)){	
		if(Preview){
			DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			dmaFillHalfWords(PA_RGB(31,31,31),PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			Font_SetScreenBuf(1,PA_DrawBg[1]);
			Font_OutputText(1,0,0,256,192,"AA Preview",1,FT_FIXED(42),0,PA_RGB(0,0,0));
			Font_OutputText(1,0,50,256,192,"Preview",0,FT_FIXED(30),0,PA_RGB(0,0,0));
			Font_OutputText(1,0,100,256,192,"AA Small",1,FT_FIXED(20),0,PA_RGB(0,0,0));
			Font_OutputText(1,0,130,256,192,"Small",0,FT_FIXED(20),0,PA_RGB(0,0,0));
		}
		return 1;
	}
	return 0;
}
//======================================================================================================================================================
//Image Loading
//======================================================================================================================================================
void CalcAvailableLayers(int width, int height){
	int i=0;
	int NumOfFreeLayers=1, FreeBuffers=0;
	int MemUsed =width*height*2; //width * height * 2 bytes per pixel
	int alphamem = width*height;
	if(DSBmp.Bits <16) alphamem=0;
	//calculate free layers
	for(i=2;i<Settings.MaxLayers+2;i++){
		if(MemUsed*i<=Settings.MaxMemory)NumOfFreeLayers++;
	}
	
	while( (MemUsed*NumOfFreeLayers) + (alphamem*(NumOfFreeLayers-1)) >Settings.MaxMemory)NumOfFreeLayers--;
	DSBmp.NumOfLayers=NumOfFreeLayers-1;
	if(NumOfFreeLayers==0)NumOfFreeLayers=1;// set to one for further calculations
	//calculate free buffers for undo
	#ifndef UNDO_Buf_Size
		int UndoMem = SCREEN_WIDTH*SCREEN_HEIGHT*2; //undo for one screen
	#else 
		int UndoMem = UNDO_Buf_Size;
	#endif
	if(Settings.SDCache==0){
		MemUsed=(MemUsed*NumOfFreeLayers) +(alphamem*(NumOfFreeLayers-1)) ; 
		for(i=2;i<Settings.MaxBuffers+2;i++){
			if((MemUsed+(UndoMem*i))<Settings.MaxMemory+1)FreeBuffers++;
		}	
		DSBmp.NumOfUndos=FreeBuffers;
	}
	else DSBmp.NumOfUndos=Settings.MaxBuffers;
}
void ResetImageSettings(void){
	Draw.Bits=DSBmp.Bits;
	memset(&DSBmp, 0,sizeof(DSBmp));
	DSBmp.Bits=Draw.Bits;
	Box.Tool[TB_Zoom].SliderY=0;
	Box.Tool[TB_Zoom].SliderValue=256;
	Draw.Zoom=256;
	PA_SetBgRot(0, 3,0,0,0,0,0,Draw.Zoom);
}
bool InitFreeBuffers(int width, int height){
	int i=0;	
	bool checks=0;
	//free the filebrowser list
	free(files);
	files = NULL;
	//main drawing layer
	//clear buffers
	if(!Settings.ExternalRam){
		for(i=0;i<10;i++){
			free(bglayer[i]);
			bglayer[i]=NULL;
			free(alphalayer[i]);
			alphalayer[i]=NULL;
			free(UndoBuf[i]);
			UndoBuf[i]=NULL;
		}
		free(Cache.UndoBuf);
		Cache.UndoBuf=NULL;
	}
	free(LinesToWrite);
	LinesToWrite=NULL;
	ResetImageSettings();
	CalcAvailableLayers(width,height);
	if(!Settings.ExternalRam){
		bglayer[0] =(u16*)calloc(width*height, sizeof(u16));
		LayerOrder[0]=0;
		if(bglayer[0]!=NULL){
			//other layers
			if(DSBmp.NumOfLayers>0){
				for(i=0;i<DSBmp.NumOfLayers;i++){
					bglayer[i+1]=(u16*)calloc(width*height, sizeof(u16));
					if(bglayer[i+1]==NULL)BAG_Error_Print(Error_LayerAlloc);
					LayerOrder[i+1]=i+1;
					if(DSBmp.Bits>8){
						alphalayer[i]=(u8*)calloc(width*height, sizeof(u8));
						if(alphalayer[i]==NULL)BAG_Error_Print(Error_LayerAlloc);
					}
				}
			}
			if(DSBmp.NumOfUndos>0 && Settings.SDCache==0){
				for(i=0;i<=DSBmp.NumOfUndos;i++){
					#ifndef UNDO_Buf_Size
						UndoBuf[i]=(u16*)calloc(SCREEN_WIDTH*SCREEN_HEIGHT ,sizeof(u16));
					#else 
						UndoBuf[i]=(u16*)calloc(UNDO_Buf_Size>>1 ,sizeof(u16));
					#endif
					if(UndoBuf[i]==NULL)BAG_Error_Print(Error_UndoAlloc);
				}
			}
			if(Settings.SDCache==1){
				Cache.UndoBuf = (u16*)calloc(UNDO_Buf_Size>>1 ,sizeof(u16));
				//Cache.SlotPos = (fpos_t*)calloc(Settings.MaxBuffers ,sizeof(fpos_t));
			}
			LinesToWrite = (bool*)calloc(height, sizeof(bool));
			if(LinesToWrite==NULL)BAG_Error_Print(Error_LinesToWrite);
			checks=1;
		}
		else{
			BAG_Error_Print(Error_ImageAlloc);
		}
	}
	else{
		if(DSBmp.ExtRam!=NULL){
			ram_lock();
			DSBmp.ExtRam=NULL;
		}
		DSBmp.ExtRam = ram_unlock ();
		
		int i=0;
		for(i=0;i<10;i++){
			#ifdef USE_BGET
				if(bglayer[i])brel(bglayer[i]);
				if(UndoBuf[i])brel(UndoBuf[i]);
			#endif
			bglayer[i]=NULL;
			UndoBuf[i]=NULL;
		}
		#ifdef USE_BGET
			bglayer[0] =(u16*) bget(width*height*2);
		#else 
			u32 LayerSize= width*height;
			u32 AlphaSize = LayerSize>>1;
			bglayer[0] =(u16*)DSBmp.ExtRam;
			DSBmp.ExtRam+=LayerSize;
		#endif
		LayerOrder[0]=0;
		
		if(bglayer[0] !=NULL){
			if(DSBmp.NumOfLayers>0){
				for(i=0;i<DSBmp.NumOfLayers;i++){
					#ifdef USE_BGET
						bglayer[i+1]=bget(width*height*2);
						if(DSBmp.Bits>8)alphalayer[i]=bget(width*height);
					#else
						bglayer[i+1]=(u16*)DSBmp.ExtRam;
						DSBmp.ExtRam+=LayerSize;
						if(DSBmp.Bits>8){
							alphalayer[i]=(u8*)DSBmp.ExtRam;
							DSBmp.ExtRam+=AlphaSize;
						}
					#endif
					if(bglayer[i+1]==NULL)BAG_Error_Print(Error_LayerAlloc);
					LayerOrder[i+1]=i+1;
				}
			}
			if(DSBmp.NumOfUndos>0 && Settings.SDCache==0){
				for(i=0;i<=DSBmp.NumOfUndos;i++){
					#ifndef UNDO_Buf_Size
						#ifdef USE_BGET
							UndoBuf[i]=(u16*) bget(SCREEN_WIDTH*SCREEN_HEIGHT*2);
						#else 
							UndoBuf[i]=(u16*)DSBmp.ExtRam;
							DSBmp.ExtRam+=(SCREEN_WIDTH*SCREEN_HEIGHT);
						#endif
					#else 
						#ifdef USE_BGET
							UndoBuf[i]=(u16*) bget(UNDO_Buf_Size);
						#else
							UndoBuf[i]=(u16*)DSBmp.ExtRam;
							DSBmp.ExtRam+=(UNDO_Buf_Size>>1);
						#endif
					#endif
					if(UndoBuf[i]==NULL)BAG_Error_Print(Error_UndoAlloc);
				}
			}
			if(Settings.SDCache==1){
				//Cache.UndoBuf = (u16*)bget(UNDO_Buf_Size);
				//Cache.SlotPos = (fpos_t*)bget(Settings.MaxBuffers*sizeof(fpos_t));
			}
			LinesToWrite = (bool*)calloc(height, sizeof(bool));
			if(LinesToWrite==NULL)BAG_Error_Print(Error_LinesToWrite);
			checks=1;
		}
		else{
			BAG_Error_Print(Error_ImageAlloc);
		}
	}
	if(checks){
		//make new canvas limits
		if(width<SCREEN_WIDTH){
			DSBmp.CanvasX=128-(width>>1);
			DSBmp.TempCanvasX = DSBmp.CanvasX;
			while(DSBmp.TempCanvasX%2 !=0 )DSBmp.TempCanvasX--;
			DSBmp.CanvasX=DSBmp.TempCanvasX;
		}
		else {
			DSBmp.CanvasX=0;
			DSBmp.TempCanvasX=0;
			DSBmp.HWScrollX=0;
		}
		if(height<SCREEN_HEIGHT){
			DSBmp.CanvasY=96-(height>>1);
			DSBmp.TempCanvasY = DSBmp.CanvasY;
			while(DSBmp.TempCanvasY%2 !=0 )DSBmp.TempCanvasY--;
			DSBmp.CanvasY=DSBmp.TempCanvasY;
		}
		else {
			DSBmp.CanvasY=0;
			DSBmp.TempCanvasY=0;
			DSBmp.HWScrollY=0;
		}	
		return 1;
	}
	return 0;
}

void Scan8BitPalForWhite(u16  *Pal){
	s16 i=0;
	u16 White = 65535, Diff = White;
	u8 TempPal=0, EmptyFound=0, BlackFound=0;
	//find the closest color to whiite
	for(i=0;i<256;i++){
		u16 Temp = White-Pal[i] ;
		if(Temp< Diff){
			Diff=Temp;
			TempPal=i;
		}
	}
	
	if(Diff!=0){//no exact white colors
		//now check if there is any empty spaces
		for(i=0;i<256;i++){
			if(Pal[i] == 32768){
				BlackFound++;
				if(BlackFound>1){
					TempPal=i;
					EmptyFound=1;
					break;
				}
			}
		}
		
		if(EmptyFound){
			//set empty palette to white
			palette[TempPal]=PA_RGB(31,31,31);
		}
	}
	//set palette
	DSBmp.EraserPal = TempPal;
}

bool New_Canvas(int width, int height){
	if(InitFreeBuffers(width,height)){
		DSBmp.Width=width;DSBmp.Height=height;
		if(width>SCREEN_WIDTH)DSBmp.CenterX=DSBmp.ScrollX+(Draw.Zoom>>1); 
		else DSBmp.CenterX=DSBmp.HWScrollX+(Draw.Zoom>>1);
		if(height>SCREEN_HEIGHT)DSBmp.CenterY=DSBmp.ScrollY+(Draw.Zoom>>1);
		else DSBmp.CenterY=DSBmp.HWScrollY+(Draw.Zoom>>1);

		
		int y=0,i=0;
		//set bg layer 0 to all white
		if(DSBmp.Bits==8){
			//set all the palettes to random colors
			palette[0]=PA_RGB(31,31,31);
			palette[1]=PA_RGB(0,0,0);
			DSBmp.EraserPal=0;
			DSBmp.NColors=2;
			for(i=2;i<256;i++)palette[i]=0;
			//convert draw color into color with a palette
			Draw.Color[0]=1;
			Draw.Color[1]=0;
			//clear all layers
			for(y=0;y<height;y++){
				memset ((u16*)&bglayer[0][y*width],0,width<<1);
				if(DSBmp.NumOfLayers>=1){
					for(i=0;i<DSBmp.NumOfLayers;i++){
						memset ((u16*)&bglayer[i+1][y*width],Trans8bit+1,width<<1);
					}
				}
				if(Settings.ExternalRam)LoadingBar(0,28,106,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),(y),height);
			}
		}
		if(DSBmp.Bits>8){
			for(y=0;y<height;y++){
				if(DSBmp.NumOfLayers>=1){
					for(i=0;i<DSBmp.NumOfLayers;i++){
						memset ((u16*)&bglayer[i+1][y*width],Trans24bit,width<<1);
					}
				}
				memset ((u16*)&bglayer[0][y*width],PA_RGB(31,31,31),width<<1);
				if(Settings.ExternalRam)LoadingBar(0,28,106,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),(y),height);
			}
		}
		CalculateScale(width,height,Settings.MiniviewWidth,Settings.MiniviewHeight);
		return 1;
	}
	return 0;
}
s8 LoadEPF(char *filename){
	CreateSaveScreen(filename,0);
	bgfile = fopen (filename, "rb"); // change before compiling to your bmp

	if(bgfile){
		struct EPFheader EPF;
		//read the header to see if it is a epf file
		fread(&EPF.Header,4,1, bgfile);
		if(strcmp(EPF.Header,"EPF"))return 0;//if the file isn't a EPF file
		
		//read the whole header if file is legit
		fread(&EPF.Bits,1,1, bgfile);
		fread(&EPF.Layers,1,1, bgfile);
		fread(&EPF.Width,2,1, bgfile);
		fread(&EPF.Height,2,1, bgfile);
		fread(&EPF.NColors,2,1, bgfile);
		
		if((EPF.Width*EPF.Height*(EPF.Layers+1))>Settings.MaxBmpSize)return -1;//image is too large
		else{
			//set the bitmode for the program
			DSBmp.Bits=EPF.Bits;
			int y=0,i=0,x=0;
			u16* bytes = calloc(EPF.Width, sizeof(u16));
			
			
			if(New_Canvas(EPF.Width,EPF.Height)){	
				//read palette if there is one
				DSBmp.NColors = EPF.NColors;
				if(DSBmp.Bits==8)fread(&palette,1,256<<1, bgfile);
				
				for(y=0;y<EPF.Height;y++){
					for(i=0;i<EPF.Layers+1;i++){
						fread(bytes,1,EPF.Width*2, bgfile);
						for(x=0;x<EPF.Width;x++)bglayer[LayerOrder[i]][x+ (y* EPF.Width)]=bytes[x];
					}
					LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),y,EPF.Height);
				}
				fclose(bgfile);
				free(bytes);
				bytes=NULL;
				RefreshDrawing(0,0,0);
				DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
				DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
				DSBmp.InitialSave=1;
				return 1;
			}
		}
	}
	return 0;
}
	
s8 LoadBmp(char *filename){
	s32 i = 0;
	bgfile = fopen (filename, "rb"); // change before compiling to your bmp
	int fileWidth,fileHeight;
	u8* bytes=NULL;
	int r=0,g=0,b=0, x=0, y=0, imageOffset=0,ClrsUsed=0;
	short bitcount=0,tempcount=DSBmp.Bits;
	#ifdef ENABLEOGG
		s8 tempogg=OggStatus;
		//if(OggStatus==3)Pause_OGG();
		Ogg_Pause(1);
	#endif
	if(bgfile){
		CreateSaveScreen(filename,0);
		//read the image position in the file
		fseek(bgfile, 10, SEEK_SET);
		fread(&imageOffset, sizeof(int),1, bgfile);
		//read the width
		fseek(bgfile, 18, SEEK_SET);
		fread(&fileWidth, sizeof(int),1, bgfile);
		//read the height
		fseek(bgfile, 22, SEEK_SET);
		fread(&fileHeight, sizeof(int), 1, bgfile);
		//bit count
		fseek(bgfile, 28, SEEK_SET);
		fread(&bitcount, sizeof(short), 1, bgfile);
		//num of colors used - 8bit
		fseek(bgfile, 46, SEEK_SET);
		fread(&ClrsUsed, sizeof(int), 1, bgfile);
		
		
		if((fileWidth*fileHeight)>Settings.MaxBmpSize)return -1;//image is too large
		else{
			if(Settings.ExternalRam){
				PA_16bitText(0,100,80, 255,192,"Preparing Ram", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
				LoadStylusCursor(0,3,1,TB_Bucket,(256>>1)-(16>>1),(192>>1)-(16>>1));
			}
			DSBmp.Bits=bitcount;
			if(New_Canvas(fileWidth, fileHeight)){
				if(Settings.ExternalRam){
					DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
					dmaFillHalfWords(0,PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
					DeleteCursor();
					PA_16bitText(0,108,70, 255,192,"Loading Image", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
				}
				fseek(bgfile, imageOffset, SEEK_SET);
				int bytesPerLine = (((fileWidth * bitcount) +31)/32)*4;
				bytes = calloc((bytesPerLine), sizeof(u8));
				
				if(bitcount>16){
					for (y = fileHeight-1; y > -1; y--, i = 0){
						fread(bytes,1,bytesPerLine,bgfile);
						for (x = 0; x < fileWidth; x++){
							b = (bytes[i] >> 3)&31;	i++;
							g = (bytes[i] >> 3)&31;	i++;
							r = (bytes[i] >> 3)&31;	i++;	
							bglayer[0][(x) + ((y) * fileWidth)] = PA_RGB(r,g,b);
						}
						while(i&3) i++;
						LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),(fileHeight-y),fileHeight);
					}
					if(Draw.Color[0]<=256 || Draw.Color[1]<=256){
						Draw.Color[0]=PA_RGB(0,0,0);
						Draw.Color[1]=PA_RGB(31,31,31);
					}
				}
				else if (bitcount == 16) {
					for (y = fileHeight-1; y >-1 ; y--){
						fread(bytes,1,bytesPerLine,bgfile);
						for (x = 0; x < fileWidth; x++) {
							u16 lobyte = bytes[(x<<1)];
							u16 hibyte = bytes[((x<<1) + 1)];
							u16 color = (hibyte << 8) +lobyte;
							r = (color>>10)&31;
							g = (color>>5) &31;
							b = (color & 31);							
							bglayer[0][x + (y  * fileWidth)] =PA_RGB(r,g,b);
						}
						LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),(fileHeight-y),fileHeight);
					}
					if(Draw.Color[0]<=256 || Draw.Color[1]<=256){
						Draw.Color[0]=PA_RGB(0,0,0);
						Draw.Color[1]=PA_RGB(31,31,31);
					}					
				}
				else if (bitcount == 8) {		
					i = 0;
					temp_pal= calloc(256*4, sizeof(char));
					fseek(bgfile, 54, SEEK_SET);
					fread(temp_pal,1,256*4,bgfile);
					//load palette
					DSBmp.NColors = ClrsUsed & 0xff;
					if(DSBmp.NColors==0 ) DSBmp.NColors = 256;
					for (i = 0; i < 256; i++) {
						if(i<DSBmp.NColors)palette[i]= PA_RGB(temp_pal[(i*4)+2]>>3,temp_pal[(i*4)+1]>>3,temp_pal[i*4]>>3);
						else palette[i]=0;
					}
					Scan8BitPalForWhite(palette);
					free(temp_pal);
					i=0;
					fseek(bgfile, imageOffset, SEEK_SET);
					for (y = fileHeight-1; y > -1; y--, i = 0){
						fread(bytes,1,bytesPerLine,bgfile);
						for (x = 0; x < fileWidth; x++) {
							bglayer[0][x + (y  * fileWidth)] =  bytes[i];
							i++;
						}
						LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),(fileHeight-y),fileHeight);
					}
					//convert draw color into color with a palette
					if(Draw.Color[0] >256 || Draw.Color[1]>256){
						Draw.Color[0]=1;
						Draw.Color[1]=2;
					}
				}
				DSBmp.Bits=bitcount;
				free(bytes);
				bytes=NULL;
				fclose(bgfile);
				DSBmp.InitialSave=1;
				#ifdef ENABLEOGG
				//Pause_OGG();
				if(tempogg==2)Ogg_Pause(0);
				//Ogg_Pause(1);
				#endif
				return 1;
			}
			DSBmp.Bits=tempcount;
			fclose(bgfile);
			return 0;
		}
	}
	return 0;
}
s8 LoadBmpPreview(char *filename){
	FILE *bgfile = fopen (filename, "rb"); // change before compiling to your bmp
	int y=0,x=0;
	s32 r,g,b;
	u8* bytes=NULL;
	int fileWidth=0,fileHeight=0,imageOffset=0;
	short bitcount=0;
	if(bgfile){
		DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		dmaFillHalfWords(0,PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		//read the image position in the file
		fseek(bgfile, 10, SEEK_SET);
		fread(&imageOffset, sizeof(int),1, bgfile);
		//read the width
		fseek(bgfile, 18, SEEK_SET);
		fread(&fileWidth, sizeof(int),1, bgfile);
		//read the height
		fseek(bgfile, 22, SEEK_SET);
		fread(&fileHeight, sizeof(int), 1, bgfile);
		//bits
		fseek(bgfile, 28, SEEK_SET);
		fread(&bitcount, sizeof(short), 1, bgfile);
		
		if((fileWidth*fileHeight)>Settings.MaxBmpSize)return -1;//image is too large
		else{	
			int bytesPerLine = (((fileWidth * bitcount) +31)/32)*4;
			bytes = calloc((bytesPerLine), sizeof(u8));
			fseek(bgfile, imageOffset, SEEK_SET);
			memset ((u16*)&bg_array[1],0,SCREEN_WIDTH*SCREEN_HEIGHT*2);
			CalculateScale(fileWidth,fileHeight,Settings.PreviewWd,Settings.PreviewHt);
			int NewFileHeight=fileHeight/DSBmp.YScale;
			
			if(bitcount==24){
				for (y = NewFileHeight; y >=0; y--){
					if(fseek (bgfile,(DSBmp.YScale-1)*bytesPerLine,SEEK_CUR)==0){
						fread(bytes,1,bytesPerLine,bgfile);
						for (x = 0; x <fileWidth; x+=(DSBmp.XScale)){			
							b = (bytes[3*x] >> 3)&31;	
							g = (bytes[3*x+1] >> 3)&31;	
							r = (bytes[3*x+2] >> 3)&31;		
							if(x>fileWidth)x=fileWidth;
							PA_DrawBg[1][DSBmp.MiniViewX+(x/DSBmp.XScale)+ ((y+DSBmp.MiniViewY) * SCREEN_WIDTH)] = PA_RGB(r,g,b);
						}
					}
				}
			}
			else if (bitcount == 16) {
				for (y = NewFileHeight; y >=0; y--){
					if(fseek (bgfile,(DSBmp.YScale-1)*bytesPerLine,SEEK_CUR)==0){
						fread(bytes,1,bytesPerLine,bgfile);
						for (x = 0; x <fileWidth; x+=DSBmp.XScale){
							u16 lobyte = bytes[(x*2)];
							u16 hibyte = bytes[((x*2) + 1)];
							u16 color = (hibyte << 8) +lobyte;
							r = (color>>10)&31;
							g = (color>>5) &31;
							b = (color & 31);	
							if(x>fileWidth)x=fileWidth;
							PA_DrawBg[1][DSBmp.MiniViewX+(x/DSBmp.XScale)+ ((y+DSBmp.MiniViewY) * SCREEN_WIDTH)] = PA_RGB(r,g,b);
						}
					}
				}
			}
			else if (bitcount == 8) {
				int i=0;
				temp_pal= calloc(256*4, sizeof(char));
				fseek(bgfile, 54, SEEK_SET);
				fread(temp_pal,1,256*4,bgfile);
				u16 PAL[256];
				
				for (i = 0; i < 256; i++) {
					PAL[i]= PA_RGB(temp_pal[(i*4)+2]>>3,temp_pal[(i*4)+1]>>3,temp_pal[i*4]>>3);
				}
				free(temp_pal);
				fseek(bgfile, imageOffset, SEEK_SET);
				i=0;
				for (y = NewFileHeight-1; y >= 0; y--,i=0) {
					if(fseek (bgfile,((int)(DSBmp.YScale-1))*bytesPerLine,SEEK_CUR)==0){
						fread(bytes,1,bytesPerLine,bgfile);
						for (x = 0; x < fileWidth; x+=DSBmp.XScale) {
							if(x>fileWidth)x=fileWidth;
							PA_DrawBg[1][DSBmp.MiniViewX+(x/DSBmp.XScale)+ ((y+DSBmp.MiniViewY) * SCREEN_WIDTH)] =  PAL[bytes[i]];
							i+=DSBmp.XScale;
						}
					}
				}
			}
			free(bytes);
			bytes=NULL;
			fclose(bgfile);
		}
		return 1;
	}
	return 0;
}
s8 LoadEpfPreview(char *filename){
	FILE *bgfile = fopen (filename, "rb"); // change before compiling to your bmp
	//memset ((u16*)&ToolBuf,0,SCREEN_WIDTH*SCREEN_HEIGHT*2);
	u16* bytes=NULL;
	if(bgfile){
		DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		dmaFillHalfWords(0,PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		
		struct EPFheader EPF;
		//read the header to see if it is a epf file
		fread(&EPF.Header,4, 1, bgfile);
		if(strcmp(EPF.Header,"EPF"))return 0;//if the file isn't a EPF file

		//read the whole header if file is legit
		fread(&EPF.Bits,1,1, bgfile);
		fread(&EPF.Layers,1,1, bgfile);
		fread(&EPF.Width,2,1, bgfile);
		fread(&EPF.Height,2,1, bgfile);
		u16 PAL[256];
		if(EPF.Bits==8)fread(&PAL,1,256<<1, bgfile);
			
		if((EPF.Width*EPF.Height*(EPF.Layers+1))>Settings.MaxBmpSize)return -1;//image is too large
		else{	
			memset ((u16*)&bg_array[1],0,SCREEN_WIDTH*SCREEN_HEIGHT*2);
			CalculateScale(EPF.Width,EPF.Height,Settings.PreviewWd,Settings.PreviewHt);
			int NewFileHeight=EPF.Height/DSBmp.YScale;
			
			bytes = calloc(EPF.Width, sizeof(u16));
			
			int x=0, y=0,i=0;
			for(y=0;y<NewFileHeight;y++){
				if(fseek (bgfile,(DSBmp.YScale-1)*(EPF.Width*2*(EPF.Layers+1)),SEEK_CUR)==0){
					for(i=0;i<EPF.Layers+1;i++){
						fread(bytes,EPF.Width*2,1, bgfile);
						for (x = 0; x <EPF.Width; x+=DSBmp.XScale){
							if(EPF.Bits>8){
								if(bytes[x]>=TransLim)PA_DrawBg[1][DSBmp.MiniViewX+(x/DSBmp.XScale)+ ((y+DSBmp.MiniViewY) * SCREEN_WIDTH)] = bytes[x];
							}
							else{
								if(bytes[x]<Trans8bit)PA_DrawBg[1][DSBmp.MiniViewX+(x/DSBmp.XScale)+ ((y+DSBmp.MiniViewY) * SCREEN_WIDTH)] = PAL[bytes[x]];
							}
						}							
					}
				}
			}
			free(bytes);
			bytes=NULL;
			fclose(bgfile);
			return 1;
		}
	}
	return 0;
}

