#include <PA_BgStruct.h>

extern const char modernlarge_Tiles[];
extern const char modernlarge_Map[];
extern const char modernlarge_Sizes[];

const PA_BgStruct modernlarge = {
	PA_Font8bit,
	512, 128,

	modernlarge_Tiles,
	modernlarge_Map,
	{modernlarge_Sizes},

	12608,
	{13}
};
