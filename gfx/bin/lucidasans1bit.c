#include <PA_BgStruct.h>

extern const char lucidasans1bit_Tiles[];
extern const char lucidasans1bit_Map[];
extern const char lucidasans1bit_Sizes[];

const PA_BgStruct lucidasans1bit = {
	PA_Font4bit,
	512, 128,

	lucidasans1bit_Tiles,
	lucidasans1bit_Map,
	{lucidasans1bit_Sizes},

	3200,
	{9}
};
