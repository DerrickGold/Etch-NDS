#include <PA9.h>
#include "oggplay.h"

#ifdef ENABLEOGG
s8 channels=2;
OggVorbis_File vf;	/* vorbis structure*/
mm_ds_system sys;
mm_stream mystream;

void Open_OGG_File(FILE * oggfile, OggVorbis_File * vf){
	if(ov_open(oggfile, vf, NULL, 0) < 0) {
	}
	else{
    	char **ptr=ov_comment(vf,-1)->user_comments;
		
		//song playback info
    	vorbis_info *vi=ov_info(vf,-1);
		channels=vi->channels;
		switch(channels){
			case 1:mystream.format = MM_STREAM_16BIT_MONO;break;
			case 2:mystream.format = MM_STREAM_16BIT_STEREO;break;
		}
		mystream.sampling_rate= vi->rate;
		
    	while(*ptr){
			++ptr;
    	}
	}
}

void Close_OGG_File(OggVorbis_File * vf){
	if(OggStatus>0){
		mmStreamClose(); // close the stream, we're ready
		ov_clear(vf);	// clean up the info struct
		//free(vf);
		//vf=NULL;
		fclose(oggfile);
		OggStatus=1;
	}
}
/* update function*/
mm_word on_stream_request( mm_word length, mm_addr dest, mm_stream_formats format ) 
{
	int rlength = length;
	int current_section;
	
	s16 *target = (s16*)dest;
	
	while( rlength && OggStatus==2) { // while we have to fill something
		long ret = ov_read( &vf, (void*)target, (rlength<<1)*channels, &current_section ); // length * 4, 2x for stereo en 2x due samples(not bytes!
		// a sample is 2 bytes
		if( ret == 0 ) {/*EOF*/
			length = 0;
			OggStatus=1;
			//Close_OGG_File(&vf);
			break;		
		} 
		else {
			if( ret < 0 ) {/* error*/
			} 
			else {
				if(channels==2){
					target += ret >>1; 	// because of stereo sound!
					rlength -= ret / 4;	/*already mentioned why*/
				}
				else {
					target += ret>>1;
					rlength -= ret>>1;	/*already mentioned why*/
				}
			}
		}
	}	
	return length;
}


void Play_OGG(char *file){
	if(OggStatus>0){
		Close_OGG_File(&vf); // close the previous file
		oggfile = fopen(file, "rb");
		Open_OGG_File(oggfile, &vf);
		mmStreamOpen(&mystream );
		OggStatus=2;
	}
}
void Pause_OGG(void){
	switch(OggStatus){
		default:break;
		case 2:
			OggStatus=3;
			mmStreamClose(); // close the stream, we're ready
		break;
		case 3:
			OggStatus=2;
			mmStreamOpen(&mystream );
		break;
	}
}
void Ogg_Pause(bool pause){
	if(pause){
		OggStatus=3;
		mmStreamClose(); // close the stream, we're ready	
	}
	else{
		OggStatus=2;
		mmStreamOpen(&mystream );	
	}
}
void Stop_OGG(void){
	if(OggStatus==2)Close_OGG_File(&vf); // close the previous file
}

void InitOGG(void){
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit( &sys );

	mystream.sampling_rate		= 32768;
	mystream.buffer_length		= 8192;		
	mystream.callback 		= on_stream_request;		
	mystream.format			= MM_STREAM_16BIT_STEREO;	
	mystream.timer			= MM_TIMER0;				
	mystream.manual			= false;
	OggStatus=1;
	Ogg_AutoPlay=1;
}
#endif
