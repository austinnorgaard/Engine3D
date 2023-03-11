#include "Mesh.h"

Mesh::Mesh () {

}

Mesh::Mesh (std::string sFileName) {
	loadFromObjectFile (sFileName);
}

bool Mesh::loadFromObjectFile (std::string sFileName, bool bIsTexture) {
	std::ifstream f (sFileName);
	if (!f.is_open ()) {
		exit (EXIT_FAILURE);
	}
	

	while (!f.eof ()) {
		char line[128];
		f.getline (line, 128);

		std::strstream s;
		s << line;

		char unUsed;

		if (line[0] == 'v') {
			if (line[1] == 't') {
				Vec2D v;
				s >> unUsed >> unUsed >> v;
				textures.push_back (v);
			}
			else {
				Vec3D v;
				s >> unUsed >> v;
				vertices.push_back (v);
			}
		}

		if (!bIsTexture) {
			if (line[0] == 'f') {
				int f[3];
				s >> unUsed >> f[0] >> f[1] >> f[2];
				TexturedTriangle t (vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1]);
				tris.push_back (t);
			}
		}

		else {
			if (line[0] == 'f') {
				s >> unUsed;

				std::string tokens[6];
				int nTokenCount = -1;


				while (!s.eof ()) {
					char c = s.get ();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else
						tokens[nTokenCount].append (1, c);
				}

				tokens[nTokenCount].pop_back ();


				tris.push_back ({vertices[stoi (tokens[0]) - 1], vertices[stoi (tokens[2]) - 1], vertices[stoi (tokens[4]) - 1],
					textures[stoi (tokens[1]) - 1], textures[stoi (tokens[3]) - 1], textures[stoi (tokens[5]) - 1]});

			}

		}
	}

	return true;
}

std::vector<TexturedTriangle> Mesh::getTris () const {
	return tris;
}