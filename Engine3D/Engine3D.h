#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"

#include <fstream>
#include <strstream>
#include <algorithm>

struct vec3d {
	float x, y, z;
};

struct triangle {
	vec3d p[3];

	olc::Pixel col;
};

struct mesh {
	std::vector<triangle> tris;

	bool loadFromObjectFile (std::string sFileName) {
		std::ifstream f (sFileName);
		if (!f.is_open ()) {
			exit (EXIT_FAILURE);
		}

		// Local cache of vertices
		std::vector<vec3d> vertices;

		while (!f.eof ()) {
			char line[128];
			f.getline (line, 128);

			std::strstream s;
			s << line;

			char unUsed;

			if (line[0] == 'v') {
				vec3d v;
				s >> unUsed >> v.x >> v.y >> v.z;
				vertices.push_back (v);
			}

			if (line[0] == 'f') {
				int f[3];
				s >> unUsed >> f[0] >> f[1] >> f[2];
				tris.push_back ({vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1]});
			}
		}

		return true;
	}
};

struct mat4x4 {
	float mat[4][4] = {0};
};

class Engine3D : public olc::PixelGameEngine {

public:
	Engine3D ();

	bool OnUserCreate () override;
	bool OnUserUpdate (float fElapsedTime);

private:
	mesh meshCube;
	mat4x4 projMat;
	float fTheta;
	vec3d vCamera;

	void multiplyMatrixProjections (vec3d &input, vec3d &output, mat4x4 &m);
};