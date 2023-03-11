#pragma once

#pragma warning (disable: 4996)

#include "Triangle.h"
#include <algorithm>

class TexturedTriangle : public Triangle {
public:
	TexturedTriangle ();
	TexturedTriangle (Triangle &tri);
	TexturedTriangle (TexturedTriangle &tri);
	TexturedTriangle (Vec2D vec1, Vec2D vec2, Vec2D vec3);
	TexturedTriangle (Vec3D vec1, Vec3D vec2, Vec3D vec3);

	void setT (int index, Vec2D vec);
	Vec2D getT (int index);

private:
	Vec2D t[3];
};