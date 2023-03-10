#pragma once

#pragma warning (disable: 4996)

#include <strstream>

class Vec2D {
public: 
	Vec2D ();
	Vec2D (float newU, float newV, float newW);

	friend std::istream &operator>> (std::istream &input, Vec2D &vec);

	void setU (float newU);
	void setV (float newV);
	void setW (float newW);

	float getU () const;
	float getV () const;
	float getW () const;

private:
	float u, v, w;
};