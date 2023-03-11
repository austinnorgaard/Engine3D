#include "Vec2D.h"

Vec2D::Vec2D () {
	u = 0;
	v = 0;
}

Vec2D::Vec2D (float newU, float newV) {
	setU (newU);
	setV (newV);
}

void Vec2D::setU (float newU) {
	u = newU;
}

void Vec2D::setV (float newV) {
	v = newV;
}

float Vec2D::getU () {
	return u;
}

float Vec2D::getV () {
	return v;
}