#include "TexturedTriangle.h"

TexturedTriangle::TexturedTriangle () {
	size = 6;
}

TexturedTriangle::TexturedTriangle (Triangle &tri) : Triangle (tri) {
	size += 3;
}

TexturedTriangle::TexturedTriangle (TexturedTriangle &tri) {
	size = 6;
	p[0] = tri.getP (0);
	p[1] = tri.getP (1);
	p[2] = tri.getP (2);
	t[0] = tri.t[0];
	t[1] = tri.t[1];
	t[2] = tri.t[2];
}

TexturedTriangle::TexturedTriangle (Vec2D vec1, Vec2D vec2, Vec2D vec3) {
	t[0] = vec1;
	t[1] = vec2;
	t[2] = vec3;
}

TexturedTriangle::TexturedTriangle (Vec3D vec1, Vec3D vec2, Vec3D vec3) : Triangle (vec1, vec2, vec3) {
}

TexturedTriangle::TexturedTriangle (Vec3D vec1, Vec3D vec2, Vec3D vec3, Vec2D vec4, Vec2D vec5, Vec2D vec6) : Triangle (vec1, vec2, vec3) {
	TexturedTriangle (vec4, vec5, vec6);
}

void TexturedTriangle::setT (int index, Vec2D vec) {
	t[index] = vec;
}

Vec2D TexturedTriangle::getT (int index) {
	return t[index];
}