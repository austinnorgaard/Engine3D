#pragma once

#pragma warning (disable: 4996)

#include "mat4x4.h"
#include <iostream>

class Vec3D {
public:
	Vec3D ();
	Vec3D (float newX, float newY, float newZ);

	Vec3D operator+ (const Vec3D &vec) const;
	Vec3D operator- (const Vec3D &vec) const;
	Vec3D operator* (float k) const;
	extern friend Vec3D operator* (const Vec3D &vec, const Mat4x4 &mat);
	Vec3D operator/ (float k) const;
	friend std::istream &operator>> (std::istream &input, Vec3D &vec);

	static float dotProduct (const Vec3D &vec1, const Vec3D &vec2);
	static float len (const Vec3D &vec);

	static Vec3D normalize (const Vec3D &vec);
	static Vec3D crossProduct (const Vec3D &vec1, const Vec3D &vec2);
	static Vec3D intersectPlane (Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd);

	void setX (float newX);
	void setY (float newY);
	void setZ (float newZ);
	void setW (float newW);

	float getX () const;
	float getY () const;
	float getZ () const;
	float getW () const;

private:
	float x, y, z, w;
	
};