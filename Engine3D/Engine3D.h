#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Mat4x4.h"
#include "Vec3D.h"
#include "Vec2D.h"
#include "Object.h"

#include <fstream>
#include <strstream>
#include <algorithm>

class Engine3D : public olc::PixelGameEngine {

public:
	Engine3D (std::vector<std::string> fileNames, std::vector<std::string> textureFileNames, bool bIsTexture = true);

	bool OnUserCreate () override;
	bool OnUserUpdate (float fElapsedTime) override;

	Mat4x4 pointAt (Vec3D &pos, Vec3D &target, Vec3D &up);
	void DrawTexturedTriangle (int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		olc::Decal *texture);

	void DrawTexturedTriangle (Object *object);

	float *getPDepth ();

private:
	std::vector<Object *> world;
	Mesh *meshCube;
	Mat4x4 projMat;
	float fTheta;
	Vec3D vCamera;
	Vec3D vLookDirection;
	float fYaw;
	float *pDepthBuffer;
	std::vector<std::string> mObjectFileNames;
	std::vector<std::string> mTextureFileNames;
	bool mBIsTexture;
};