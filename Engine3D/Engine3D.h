#pragma once

#pragma warning (disable: 4996)

#include "olcPixelGameEngine.h"
#include "triangle.h"
#include "mesh.h"
#include "mat4x4.h"
#include "vec3d.h"

#include <fstream>
#include <strstream>
#include <algorithm>

class Engine3D : public olc::PixelGameEngine {

public:
	Engine3D ();

	bool OnUserCreate () override;
	bool OnUserUpdate (float fElapsedTime);

	Mat4x4 pointAt (Vec3D &pos, Vec3D &target, Vec3D &up);

private:
	Mesh meshCube;
	Mat4x4 projMat;
	float fTheta;
	Vec3D vCamera;
	Vec3D vLookDirection;
};