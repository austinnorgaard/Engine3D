#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"
#include "vec3d.h"

class Triangle {
public:
	Triangle ();
	Triangle (Vec3D &vec1, Vec3D &vec2, Vec3D &vec3);

	void setColor (olc::Pixel col);
	olc::Pixel getColor ();

	Vec3D getP (int index);
	void setP (int index, Vec3D vec);

private:
	olc::Pixel col;
	Vec3D p[3];
};