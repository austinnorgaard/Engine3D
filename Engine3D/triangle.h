#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"
#include "Vec3D.h"
#include "Vec2D.h"

class Triangle {
public:
	Triangle ();
	Triangle (const Triangle &tri);
	Triangle (Vec3D vec1, Vec3D vec2, Vec3D vec3);
	Triangle (Vec2D vec1, Vec2D vec2, Vec2D vec3);
	Triangle (Vec3D vec1, Vec3D vec2, Vec3D vec3, Vec2D vec4, Vec2D vec5, Vec2D vec6);

	void setColor (olc::Pixel newCol);
	olc::Pixel getColor () const;

	Vec3D getP (int index) const;
	void setP (int index, Vec3D vec);

	int getSize () const;

	void setT (int index, Vec2D vec);
	Vec2D getT (int index) const;

private:
	Vec2D t[3];
	olc::Pixel col;
	Vec3D p[3];

	// Number of Points
	int size;

};