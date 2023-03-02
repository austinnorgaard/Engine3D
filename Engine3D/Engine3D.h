#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"

#include <fstream>
#include <strstream>
#include <algorithm>

class Engine3D : public olc::PixelGameEngine {

public:
	Engine3D ();

	bool OnUserCreate () override;
	bool OnUserUpdate (float fElapsedTime);

	struct mat4x4 {
		float mat[4][4] = {0};

		mat4x4 identity ();
		mat4x4 rotateX (float fAngleRad);
		mat4x4 rotateY (float fAngleRad);
		mat4x4 rotateZ (float fAngleRad);
		mat4x4 translate (float x, float y, float z);
		mat4x4 project (float fFovDegrees, float fAspectRatio, float fNear, float fFar);
		mat4x4 operator* (mat4x4 &mat);
	};

	struct vec3d {
		float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

		vec3d operator+ (vec3d &vec);
		vec3d operator- (vec3d &vec);
		vec3d operator* (float k);
		vec3d operator/ (float k);
		float dotProduct (vec3d &vec);
		float len ();
		vec3d normalize ();
		vec3d crossProduct (vec3d &vec);
		vec3d project (mat4x4 &mat);
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

private:
	mesh meshCube;
	mat4x4 projMat;
	float fTheta;
	Engine3D::vec3d vCamera;
};