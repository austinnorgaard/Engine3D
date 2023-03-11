#pragma once

#pragma warning (disable: 4996)

#include "Mat4x4.h"
#include <iostream>

class Vec3D {
public:
	Vec3D ();
	Vec3D (float newX, float newY, float newZ);

	Vec3D operator+ (const Vec3D &vec);
	Vec3D operator- (const Vec3D &vec);
	Vec3D operator* (float k);
	extern friend Vec3D operator* (const Vec3D &vec, const Mat4x4 &mat);
	Vec3D operator/ (float k);
	friend std::istream &operator>> (std::istream &input, Vec3D &vec);

	static float dotProduct (Vec3D &vec1, Vec3D &vec2);
	static float len (Vec3D &vec);

	static Vec3D normalize (Vec3D &vec);
	static Vec3D crossProduct (Vec3D &vec1, Vec3D &vec2);
	static Vec3D intersectPlane (Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd, float &t);

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