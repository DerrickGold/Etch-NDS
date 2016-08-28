#include <PA_BgStruct.h>

extern const char _256colormenu_Tiles[];
extern const char _256colormenu_Map[];
extern const char _256colormenu_Pal[];

const PA_BgStruct _256colormenu = {
	PA_BgNormal,
	256, 192,

	_256colormenu_Tiles,
	_256colormenu_Map,
	{_256colormenu_Pal},

	2752,
	{1536}
};
