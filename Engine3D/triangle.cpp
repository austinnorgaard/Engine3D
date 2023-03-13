#include "Triangle.h"

Triangle::Triangle () {
	size = 3;
}

Triangle::Triangle (Vec2D vec1, Vec2D vec2, Vec2D vec3) {
	size = 3;
	t[0] = vec1;
	t[1] = vec2;
	t[2] = vec3;
}

Triangle::Triangle (Vec3D vec1, Vec3D vec2, Vec3D vec3, Vec2D vec4, Vec2D vec5, Vec2D vec6) {
	size = 3;
	p[0] = vec1;
	p[1] = vec2;
	p[2] = vec3;
	t[0] = vec4;
	t[1] = vec5;
	t[2] = vec6;
}

Triangle::Triangle (const Triangle &tri) {
	p[0] = tri.p[0];
	p[1] = tri.p[1];
	p[2] = tri.p[2];
	t[0] = tri.t[0];
	t[1] = tri.t[1];
	t[2] = tri.t[2];
	size = tri.size;
	col = tri.col;
}

Triangle::Triangle (Vec3D vec1, Vec3D vec2, Vec3D vec3) {
	p[0] = vec1;
	p[1] = vec2;
	p[2] = vec3;
	size = 3;
}

void Triangle::setColor (olc::Pixel newCol) {
	col = newCol;
}

olc::Pixel Triangle::getColor () const {
	return col;
}

Vec3D Triangle::getP (int index) const {
	return p[index];
}

void Triangle::setP (int index, Vec3D vec) {
	p[index] = vec;
}

int Triangle::getSize () const {
	return size;
}

void Triangle::setT (int index, Vec2D vec) {
	t[index] = vec;
}

Vec2D Triangle::getT (int index) const {
	return t[index];
}