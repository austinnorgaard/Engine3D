#pragma once

#include "Mesh.h"
#include "Vec3D.h"
#include "olcPixelGameEngine.h"

class Object {
public:
	Object (int width, int height, std::string fileName, std::vector<std::string> textureFileNames, bool bIsTexture = true);

	void TranslateTris (Mat4x4 &world, Triangle &tri, Triangle &tri2);
	void ScaleTris (Mat4x4 &mat, int scaleFactor, Triangle &tri, Triangle &tri2);
	void ClipTris (Mat4x4 &projection, int layer);

	void CollectTris (Mat4x4 &world, Mat4x4 &view, Mat4x4 &projection, Vec3D &camera);
	void CheckViewClip (Triangle tri);

	int clipAgainstPlane (Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);

	std::vector<Triangle> getTrisToRasterize ();
	std::list<Triangle> getLTris ();
	olc::Decal *getDecal (int layer);
	int getLayers ();

private:
	Mesh objFile;
	std::vector<olc::Sprite *> vSprites;
	std::vector<olc::Decal *> vDecals;
	std::vector<Triangle *> layerTris;
	std::vector<Triangle> vTris;
	std::list<Triangle> listTriangles;

	int screenWidth, screenHeight;
};