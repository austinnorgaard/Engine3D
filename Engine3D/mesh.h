#pragma once

#pragma warning (disable: 4996)

#include "triangle.h"
#include <strstream>
#include <fstream>

class Mesh {
public:
	Mesh ();
	Mesh (std::string sFileName);

	bool loadFromObjectFile (std::string sFileName);

	std::vector<Triangle> getTris () const;

private:
	std::vector<Triangle> tris;
	std::vector<Vec3D> vertices;
};