#pragma once

#pragma warning (disable: 4996)

#include "Triangle.h"
#include "TexturedTriangle.h"
#include <strstream>
#include <fstream>

class Mesh {
public:
	Mesh ();
	Mesh (std::string sFileName);

	bool loadFromObjectFile (std::string sFileName);

	std::vector<TexturedTriangle> getTris () const;

private:
	std::vector<TexturedTriangle> tris;
	std::vector<Vec3D> vertices;
};