#include "triangle.h"

Triangle::Triangle () {
}

Triangle::Triangle (Vec3D vec1, Vec3D vec2, Vec3D vec3) {
	p[0] = vec1;
	p[1] = vec2;
	p[2] = vec3;
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