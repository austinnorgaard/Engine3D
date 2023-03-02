#include "vec3d.h"

Vec3D::Vec3D () {
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

Vec3D::Vec3D (float newX, float newY, float newZ) {
	x = newX;
	y = newY;
	z = newZ;
	w = 1.0f;
}

Vec3D Vec3D::operator+ (Vec3D vec) {
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

Vec3D Vec3D::operator- (Vec3D vec) {
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

Vec3D Vec3D::operator* (float k) {
	x *= k;
	y *= k;
	z *= k;
	return *this;
}

Vec3D Vec3D::operator/ (float k) {
	x /= k;
	y /= k;
	z /= k;
	return *this;
}

std::istream &operator>> (std::istream &input, Vec3D &vec) {
	input >> vec.x >> vec.y >> vec.z;
	return input;
}

float Vec3D::dotProduct (Vec3D vec) {
	return (this->x * vec.x + this->y * vec.y + this->z * vec.z);
}

float Vec3D::len () {
	return sqrtf (dotProduct (*this));
}

Vec3D Vec3D::normalize () {
	float l = len ();
	x /= l;
	y /= l;
	z /= l;
	return *this;
}

Vec3D Vec3D::crossProduct (Vec3D &vec) {
	x = y * vec.z - z * vec.y;
	y = z * vec.x - x * vec.z;
	z = x * vec.y - y * vec.x;
	return *this;
}

Vec3D Vec3D::project (Mat4x4 &mat) {
	x = x * mat.getMat (0, 0) + y * mat.getMat (1, 0) + z * mat.getMat (2, 0) + w * mat.getMat (3, 0);
	y = x * mat.getMat (0, 1) + y * mat.getMat (1, 1) + z * mat.getMat (2, 1) + w * mat.getMat (3, 1);
	z = x * mat.getMat (0, 2) + y * mat.getMat (1, 2) + z * mat.getMat (2, 2) + w * mat.getMat (3, 2);
	w = x * mat.getMat (0, 3) + y * mat.getMat (1, 3) + z * mat.getMat (2, 3) + w * mat.getMat (3, 3);
	return *this;
}

void Vec3D::setX (float newX) {
	x = newX;
}

void Vec3D::setY (float newY) {
	y = newY;
}

void Vec3D::setZ (float newZ) {
	z = newZ;
}

void Vec3D::setW (float newW) {
	w = newW;
}

float Vec3D::getX () {
	return x;
}

float Vec3D::getY () {
	return y;
}

float Vec3D::getZ () {
	return z;
}

float Vec3D::getW () {
	return w;
}
