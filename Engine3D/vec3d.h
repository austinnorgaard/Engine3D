#pragma once

#pragma warning (disable: 4996)

#include "mat4x4.h"
#include <iostream>

class Vec3D {
public:
	Vec3D ();
	Vec3D (float newX, float newY, float newZ);

	Vec3D operator+ (Vec3D vec);
	Vec3D operator- (Vec3D vec);
	Vec3D operator* (float k);
	Vec3D operator/ (float k);
	friend std::istream &operator>> (std::istream &input, Vec3D &vec);

	float dotProduct (Vec3D &vec);
	float len ();

	Vec3D normalize ();
	Vec3D crossProduct (Vec3D &vec);
	Vec3D project (Mat4x4 &mat);

	void setX (float newX);
	void setY (float newY);
	void setZ (float newZ);
	void setW (float newW);

	float getX ();
	float getY ();
	float getZ ();
	float getW ();

private:
	float x, y, z, w;
};