#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"
#include "Triangle.h"
#include "TexturedTriangle.h"
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

	Mat4x4 pointAt (Vec3D &pos, Vec3D &target, Vec3D &up) const;
	int clipAgainstPlane (Vec3D plane_p, Vec3D plane_n, TexturedTriangle &in_tri, TexturedTriangle &out_tri1, Triangle &out_tri2);

private:
	Mesh meshCube;
	Mat4x4 projMat;
	float fTheta;
	Vec3D vCamera;
	Vec3D vLookDirection;
	float fYaw;
};