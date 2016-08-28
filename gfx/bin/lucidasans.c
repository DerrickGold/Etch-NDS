#include <PA_BgStruct.h>

extern const char lucidasans_Tiles[];
extern const char lucidasans_Map[];
extern const char lucidasans_Sizes[];

const PA_BgStruct lucidasans = {
	PA_Font8bit,
	512, 128,

	lucidasans_Tiles,
	lucidasans_Map,
	{lucidasans_Sizes},

	6400,
	{9}
};
