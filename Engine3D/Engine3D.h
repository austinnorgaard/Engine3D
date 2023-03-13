#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Mat4x4.h"
#include "Vec3D.h"
#include "Vec2D.h"

#include <fstream>
#include <strstream>
#include <algorithm>

class Engine3D : public olc::PixelGameEngine {

public:
	Engine3D ();

	bool OnUserCreate () override;
	bool OnUserUpdate (float fElapsedTime) override;

	Mat4x4 pointAt (Vec3D &pos, Vec3D &target, Vec3D &up);
	int clipAgainstPlane (Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);
	void DrawTexturedTriangle (int x1, int y1, float u1, float v1, float w1, int x2, int y2, float u2, float v2, float w2, int x3, int y3, float u3, float v3, float w3, olc::Sprite *tex);

private:
	Mesh meshCube;
	Mat4x4 projMat;
	float fTheta;
	Vec3D vCamera;
	Vec3D vLookDirection;
	float fYaw;
	olc::Sprite *sprTex1;
	float *pDepthBuffer;
};