#include "olcPixelGameEngine.h"
#include "Engine3D.h"

#include <fstream>
#include <strstream>
#include <algorithm>

Engine3D::Engine3D () {
	sAppName = "3D Renderer/Engine";
}

bool Engine3D::OnUserCreate () {
	// Handcrafted Cube
	//meshCube.tris = {

	//		// SOUTH
	//		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
	//		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	//		// EAST                                                      
	//		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
	//		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

	//		// NORTH                                                     
	//		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
	//		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

	//		// WEST                                                      
	//		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
	//		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

	//		// TOP                                                       
	//		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
	//		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

	//		// BOTTOM                                                    
	//		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
	//		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	//	};

	meshCube = Mesh ("axis.obj");

	// Projection Matrix
	projMat.project (90.0f, (float) ScreenHeight () / (float) ScreenWidth (), 0.1f, 1000.0f);

	return true;
}
olc::Pixel GetColour (float lum) {

	int nValue = (int) (std::max (lum, 0.20f) * 255.0f);
	return olc::Pixel (nValue, nValue, nValue);
}

bool Engine3D::OnUserUpdate (float fElapsedTime) {
	if (GetKey (olc::UP).bHeld) {
		vCamera.setY (vCamera.getY () + (80.0f * fElapsedTime));
	}

	if (GetKey (olc::DOWN).bHeld) {
		vCamera.setY (vCamera.getY () - (80.0f * fElapsedTime));
	}

	if (GetKey (olc::RIGHT).bHeld) {
		vCamera.setX (vCamera.getX () + (80.0f * fElapsedTime));
	}

	if (GetKey (olc::LEFT).bHeld) {
		vCamera.setX (vCamera.getX () - (80.0f * fElapsedTime));
	}

	FillRect (0, 0, ScreenWidth (), ScreenHeight (), olc::BLACK);

	Mat4x4 matRotZ, matRotX;
	// fTheta += 1.0f * fElapsedTime;

	// Rotation Z
	matRotZ = matRotZ.rotateZ (fTheta * 0.5f);

	// Rotation X
	matRotX = matRotX.rotateX (fTheta);

	Mat4x4 transMat;
	transMat = transMat.translate (0.0f, 0.0f, 16.0f);

	Mat4x4 worldMat;
	worldMat.identity ();
	worldMat = matRotZ * matRotX;
	worldMat = worldMat * transMat;

	Vec3D vUp = {0, 1, 0};
	Vec3D vTarget = {0, 0, 1};
	Mat4x4 cameraRotMat = cameraRotMat.rotateY (fYaw);
	vLookDirection = vTarget.project(cameraRotMat);
	vTarget = vCamera + vLookDirection;
	Mat4x4 cameraMatrix = pointAt (vCamera, vTarget, vUp);

	// Make view matrix from camera
	Mat4x4 viewMatrix = cameraMatrix.inverse ();

	// Store triangles
	std::vector<Triangle> trianglesToRasterize;

	// Draw Triangles

	for (auto tri : meshCube.getTris ()) {
		Triangle projectedTriangle, transformedTriangle, viewedTriangle;

		transformedTriangle.setP (0, tri.getP (0).project (worldMat));
		transformedTriangle.setP (1, tri.getP (1).project (worldMat));
		transformedTriangle.setP (2, tri.getP (2).project (worldMat));

		// Use Cross-Product to get surface normal
		Vec3D normal, line1, line2;
		
		// Get lines of either side of triangle
		line1 = transformedTriangle.getP (1) - transformedTriangle.getP (0);
		line2 = transformedTriangle.getP (2) - transformedTriangle.getP (0);

		// Take cross product of lines to get normal to triangle surface
		normal = line1.crossProduct (line2);

		// Normalize the normal
		normal.normalize ();

		// Get ray from triangle to camera
		Vec3D vCameraRay = transformedTriangle.getP (0) - vCamera;
		
		if (normal.dotProduct(vCameraRay) < 0.0f) {

			// Illumination
			Vec3D light_direction = {0.0f, 1.0f, -1.0f};
			light_direction.normalize ();

			// Alignment of light to triangle surface
			float dpLD = std::max (0.1f, light_direction.dotProduct (normal));

			transformedTriangle.setColor(GetColour (dpLD));

			// Convert World Space --> View Space
			viewedTriangle.setP (0, (transformedTriangle.getP (0).project (viewMatrix)));
			viewedTriangle.setP (1, (transformedTriangle.getP (1).project (viewMatrix)));
			viewedTriangle.setP (2, (transformedTriangle.getP (2).project (viewMatrix)));
			viewedTriangle.setColor (transformedTriangle.getColor ());

			// Project triangles from 3D --> 2D
			projectedTriangle.setP (0, viewedTriangle.getP (0).project (projMat));
			projectedTriangle.setP (1, viewedTriangle.getP (1).project (projMat));
			projectedTriangle.setP (2, viewedTriangle.getP (2).project (projMat));
			projectedTriangle.setColor (viewedTriangle.getColor ());

			// Normalize points
			projectedTriangle.setP (0, projectedTriangle.getP (0) / projectedTriangle.getP (0).getW ());
			projectedTriangle.setP (1, projectedTriangle.getP (1) / projectedTriangle.getP (1).getW ());
			projectedTriangle.setP (2, projectedTriangle.getP (2) / projectedTriangle.getP (2).getW ());

			// X/Y are inverted so put them back
			projectedTriangle.getP (0).setX (projectedTriangle.getP (0).getX () * -1.0f);
			projectedTriangle.getP (1).setX (projectedTriangle.getP (1).getX () * -1.0f);
			projectedTriangle.getP (2).setX (projectedTriangle.getP (2).getX () * -1.0f);
			projectedTriangle.getP (0).setY (projectedTriangle.getP (0).getY () * -1.0f);
			projectedTriangle.getP (1).setY (projectedTriangle.getP (1).getY () * -1.0f);
			projectedTriangle.getP (2).setY (projectedTriangle.getP (2).getY () * -1.0f);

			// Offset vertices into normalized screen space
			Vec3D vOffsetView = {1, 1, 0};
			projectedTriangle.setP (0, projectedTriangle.getP (0) + vOffsetView);
			projectedTriangle.setP (1, projectedTriangle.getP (1) + vOffsetView);
			projectedTriangle.setP (2, projectedTriangle.getP (2) + vOffsetView);
			projectedTriangle.getP (0).setX (projectedTriangle.getP (0).getX () * 0.5f * (float) ScreenWidth ());
			projectedTriangle.getP (0).setY (projectedTriangle.getP (0).getY () * 0.5f * (float) ScreenHeight ());
			projectedTriangle.getP (1).setX (projectedTriangle.getP (1).getX () * 0.5f * (float) ScreenWidth ());
			projectedTriangle.getP (1).setY (projectedTriangle.getP (1).getY () * 0.5f * (float) ScreenHeight ());
			projectedTriangle.getP (2).setX (projectedTriangle.getP (2).getX () * 0.5f * (float) ScreenWidth ());
			projectedTriangle.getP (2).setY (projectedTriangle.getP (2).getY () * 0.5f * (float) ScreenHeight ());

			trianglesToRasterize.push_back (projectedTriangle);
		}
	}

	// Sort triangles from back to front
	sort (trianglesToRasterize.begin (), trianglesToRasterize.end (), [](Triangle &t1, Triangle &t2) {
		float z1 = (t1.getP (0).getZ () + t1.getP (1).getZ () + t1.getP (2).getZ ()) / 3.0f;
		float z2 = (t2.getP (0).getZ () + t2.getP (1).getZ () + t2.getP (2).getZ ()) / 3.0f;
		return z1 > z2;
		});

	for (auto &projectedTriangle : trianglesToRasterize) {
		// Drawing the triangles using a 2D Matrix , to create a 3D illusion of an object on the screen
		FillTriangle (projectedTriangle.getP (0).getX (), projectedTriangle.getP (0).getY (),
			projectedTriangle.getP (1).getX (), projectedTriangle.getP (1).getY (),
			projectedTriangle.getP (2).getX (), projectedTriangle.getP (2).getY (),
			projectedTriangle.getColor ());

		// Wire Frame for Debugging
		//DrawTriangle (projectedTriangle.getP (0).getX (), projectedTriangle.getP (0).getY (),
		//	projectedTriangle.getP (1).getX (), projectedTriangle.getP (1).getY (),
		//	projectedTriangle.getP (2).getX (), projectedTriangle.getP (2).getY (),
		//	olc::WHITE);
	}

	return true;
}

Mat4x4 Engine3D::pointAt (Vec3D &pos, Vec3D &target, Vec3D &up) {
	// Calculate new forward direction
	Vec3D newForward = target - pos;
	newForward.normalize ();

	// Calculate new up direction
	Vec3D a = newForward * (up.dotProduct (newForward));
	Vec3D newUp = up - a;
	newUp.normalize ();

	// Calculate new right direction
	Vec3D newRight = newUp.crossProduct (newForward);

	// Construct Dimensioning and Translation Matrix
	Mat4x4 matrix;

	matrix.setMat (0, 0, newRight.getX ());		matrix.setMat (0, 1, newRight.getY ());		matrix.setMat (0, 2, newRight.getZ ());		matrix.setMat (0, 3, 0.0f);
	matrix.setMat (1, 0, newUp.getX ());		matrix.setMat (1, 1, newUp.getY ());		matrix.setMat (1, 2, newUp.getZ ());		matrix.setMat (1, 3, 0.0f);
	matrix.setMat (2, 0, newForward.getX ());	matrix.setMat (2, 1, newForward.getY ());	matrix.setMat (2, 2, newForward.getZ ());	matrix.setMat (2, 3, 0.0f);
	matrix.setMat (3, 0, pos.getX ());			matrix.setMat (3, 1, pos.getY ());			matrix.setMat (3, 2, pos.getZ ());			matrix.setMat (3, 3, 1.0f);

	return matrix;
}