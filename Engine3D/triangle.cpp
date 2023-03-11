#include "Triangle.h"

Triangle::Triangle () {
	size = 3;
}

Triangle::Triangle (Triangle &tri) {
	p[0] = tri.p[0];
	p[1] = tri.p[1];
	p[2] = tri.p[2];
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