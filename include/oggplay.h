#ifndef __OGG_PLAY_H__
#define __OGG_PLAY_H__

#define ENABLEOGG

#include <maxmod9.h>  
#include "../source/Tremolo/ivorbiscodec.h"
#include "../source/Tremolo/ivorbisfile.h"
	
#ifdef ENABLEOGG
	#define OGG_BUFFER_GROOTTE (1024) 
	FILE * oggfile;
	extern OggVorbis_File vf;	/* vorbis structure*/
	extern mm_ds_system sys;
	extern mm_stream mystream;

	extern s8 channels;
	s8 OggStatus;
	s16	Ogg_Track, Ogg_MaxTracks;
	bool Ogg_AutoPlay;
	char Ogg_CurrentSong[256];

	extern void Play_OGG(char *file);
	extern void Stop_OGG(void);
	extern void InitOGG(void);
	extern void Pause_OGG(void);
	extern void Ogg_Pause(bool pause);
#endif

#endif
