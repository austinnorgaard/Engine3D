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

Vec3D Vec3D::operator+ (const Vec3D &vec) const {
	return {x + vec.x, y + vec.y, z + vec.z};
}

Vec3D Vec3D::operator- (const Vec3D &vec) const {
	return {x - vec.x, y - vec.y, z - vec.z};
}

Vec3D Vec3D::operator* (float k) const {
	return {x * k, y * k, z * k};
}

Vec3D operator* (const Vec3D &vec, const Mat4x4 &mat) {
	Vec3D v;
	v.x = vec.x * mat.getMat(0, 0) + vec.y * mat.getMat(1, 0) + vec.z * mat.getMat(2, 0) + vec.w * mat.getMat(3, 0);
	v.y = vec.x * mat.getMat(0, 1) + vec.y * mat.getMat(1, 1) + vec.z * mat.getMat(2, 1) + vec.w * mat.getMat(3, 1);
	v.z = vec.x * mat.getMat(0, 2) + vec.y * mat.getMat(1, 2) + vec.z * mat.getMat(2, 2) + vec.w * mat.getMat(3, 2);
	v.w = vec.x * mat.getMat(0, 3) + vec.y * mat.getMat(1, 3) + vec.z * mat.getMat(2, 3) + vec.w * mat.getMat(3, 3);
	return v;
}


Vec3D Vec3D::operator/ (float k) const {
	return {x / k, y / k, z / k};
}

std::istream &operator>> (std::istream &input, Vec3D &vec) {
	input >> vec.x >> vec.y >> vec.z;
	return input;
}

float Vec3D::dotProduct (const Vec3D &vec1, const Vec3D &vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

float Vec3D::len (const Vec3D &vec) {
	return sqrtf (dotProduct (vec, vec));
}

Vec3D Vec3D::normalize (const Vec3D &vec) {
	float l = len(vec);
	return {vec.x / l, vec.y / l, vec.z / l};
}

Vec3D Vec3D::crossProduct (const Vec3D &vec1, const Vec3D &vec2) {
	Vec3D vector;
	vector.x = vec1.y * vec2.z - vec1.z * vec2.y;
	vector.y = vec1.z * vec2.x - vec1.x * vec2.z;
	vector.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return vector;
}

Vec3D Vec3D::intersectPlane (Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd) {
	plane_n = normalize(plane_n);
	float plane_d = -1.0f * (dotProduct (plane_n, plane_p));
	float ad = dotProduct (lineStart, plane_n);
	float bd = dotProduct (lineEnd, plane_n);
	float t = (-1.0f * plane_d - ad) / (bd - ad);
	Vec3D lineStartToEnd = (lineEnd - lineStart);
	Vec3D lineToIntersect = (lineStartToEnd * t);
	return (lineStart + lineToIntersect);
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

float Vec3D::getX () const {
	return x;
}

float Vec3D::getY () const {
	return y;
}

float Vec3D::getZ () const {
	return z;
}

float Vec3D::getW () const {
	return w;
}
