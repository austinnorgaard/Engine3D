#include "mesh.h"

Mesh::Mesh () {

}

Mesh::Mesh (std::string sFileName) {
	loadFromObjectFile (sFileName);
}

bool Mesh::loadFromObjectFile (std::string sFileName) {
	std::ifstream f (sFileName);
	if (!f.is_open ()) {
		exit (EXIT_FAILURE);
	}

	// Local cache of vertices
	std::vector<Vec3D> vertices;

	while (!f.eof ()) {
		char line[128];
		f.getline (line, 128);

		std::strstream s;
		s << line;

		char unUsed;

		if (line[0] == 'v') {
			Vec3D v;
			s >> unUsed >> v;
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

std::vector<Triangle> Mesh::getTris () {
	return tris;
}