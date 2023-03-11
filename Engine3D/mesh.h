#pragma once

#pragma warning (disable: 4996)

#include "Triangle.h"
#include <strstream>
#include <fstream>

class Mesh {
public:
	Mesh ();
	Mesh (std::string sFileName);

	bool loadFromObjectFile (std::string sFileName, bool bIsTexture = false);

	std::vector<Triangle> getTris ();

private:
	std::vector<Triangle> tris;
	std::vector<Vec3D> vertices;
	std::vector<Vec2D> textures;
};