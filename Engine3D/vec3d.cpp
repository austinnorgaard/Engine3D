#include "vec3d.h"

Vec3D::Vec3D () {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

Vec3D::Vec3D (float newX, float newY, float newZ) {
	x = newX;
	y = newY;
	z = newZ;
	w = 1.0f;
}

Vec3D Vec3D::operator+ (Vec3D vec) {
	return {this->x + vec.x, this->y + vec.y, this->z + vec.z};
}

Vec3D Vec3D::operator- (Vec3D vec) {
	return {this->x - vec.x, this->y - vec.y, this->z - vec.z};
}

Vec3D Vec3D::operator* (float k) {
	return {this->x * k, this->y * k, this->z * k};
}

Vec3D Vec3D::operator/ (float k) {
	return {this->x / k, this->y / k, this->z / k};
}

std::istream &operator>> (std::istream &input, Vec3D &vec) {
	input >> vec.x >> vec.y >> vec.z;
	return input;
}

float Vec3D::dotProduct (Vec3D &vec) {
	return this->x * vec.x + this->y * vec.y + this->z * vec.z;
}

float Vec3D::len () {
	return sqrtf (this->dotProduct (*this));
}

Vec3D Vec3D::normalize () {
	float l = this->len ();
	return {this->x / l, this->y / l, this->z / l};
}

Vec3D Vec3D::crossProduct (Vec3D &vec) {
	Vec3D v;
	v.x = this->y * vec.z - this->z * vec.y;
	v.y = this->z * vec.x - this->x * vec.z;
	v.z = this->x * vec.y - this->y * vec.x;
	return v;
}

Vec3D Vec3D::project (Mat4x4 &mat) {
	Vec3D v;
	v.x = this->x * mat.getMat (0, 0) + this->y * mat.getMat (1, 0) + this->z * mat.getMat (2, 0) + this->w * mat.getMat (3, 0);
	v.y = this->x * mat.getMat (0, 1) + this->y * mat.getMat (1, 1) + this->z * mat.getMat (2, 1) + this->w * mat.getMat (3, 1);
	v.z = this->x * mat.getMat (0, 2) + this->y * mat.getMat (1, 2) + this->z * mat.getMat (2, 2) + this->w * mat.getMat (3, 2);
	v.w = this->x * mat.getMat (0, 3) + this->y * mat.getMat (1, 3) + this->z * mat.getMat (2, 3) + this->w * mat.getMat (3, 3);
	return v;
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
