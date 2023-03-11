#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"
#include "Vec3D.h"
#include "Vec2D.h"

class Triangle {
public:
	Triangle ();
	Triangle (Triangle &tri);
	Triangle (Vec3D vec1, Vec3D vec2, Vec3D vec3);

	void setColor (olc::Pixel newCol);
	olc::Pixel getColor () const;

	Vec3D getP (int index) const;
	void setP (int index, Vec3D vec);

	int getSize () const;

protected:
	olc::Pixel col;
	Vec3D p[3];

	// Number of Points
	int size;

private:

};