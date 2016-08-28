#include <PA_BgStruct.h>

extern const char modernlarge1bit_Tiles[];
extern const char modernlarge1bit_Map[];
extern const char modernlarge1bit_Sizes[];

const PA_BgStruct modernlarge1bit = {
	PA_Font4bit,
	512, 128,

	modernlarge1bit_Tiles,
	modernlarge1bit_Map,
	{modernlarge1bit_Sizes},

	6304,
	{13}
};
