#include <PA_BgStruct.h>

extern const char maininterface_Tiles[];
extern const char maininterface_Map[];
extern const char maininterface_Pal[];

const PA_BgStruct maininterface = {
	PA_BgNormal,
	256, 192,

	maininterface_Tiles,
	maininterface_Map,
	{maininterface_Pal},

	5440,
	{1536}
};
