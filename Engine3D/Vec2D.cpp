#include "Vec2D.h"

Vec2D::Vec2D () {
	u = 0;
	v = 0;
	w = 1;
}

Vec2D::Vec2D (float newU, float newV, float newW) {
	setU (newU);
	setV (newV);
	setW (newW);
}

std::istream &operator>> (std::istream &input, Vec2D &vec) {
	input >> vec.u >> vec.v;
	return input;
}

void Vec2D::setU (float newU) {
	u = newU;
}

void Vec2D::setV (float newV) {
	v = newV;
}

void Vec2D::setW (float newW) {
	w = newW;
}

float Vec2D::getU () {
	return u;
}

float Vec2D::getV () {
	return v;
}

float Vec2D::getW () {
	return w;
}