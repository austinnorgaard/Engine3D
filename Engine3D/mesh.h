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

	std::vector<Triangle> getTris ();

private:
	std::vector<Triangle> tris;
};