#ifndef DATA_H
#define DATA_H

struct Vec3
{
	union
	{
		struct { float x, y, z; };
		struct { float r, g, b; };
	};
};

#endif