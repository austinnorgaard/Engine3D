#pragma once

#pragma warning (disable: 4996)

#include <algorithm>

class Vec2D {
public: 
	Vec2D ();
	Vec2D (float newU, float newV);

	void setU (float newU);
	void setV (float newV);

	float getU ();
	float getV ();

private:
	float u, v;
};