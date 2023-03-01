#include "olcPixelGameEngine.h"
#include "Engine3D.h"

#include <fstream>
#include <strstream>
#include <algorithm>

Engine3D::Engine3D () {
	sAppName = "3D Demo";
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

	meshCube.loadFromObjectFile ("VideoShip.obj");


	// Projection Matrix
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float) ScreenHeight () / (float) ScreenWidth ();
	float fFovRad = 1.0f / tanf (fFov * 0.5f / 180.0f * 3.14159f);

	projMat.mat[0][0] = fAspectRatio * fFovRad;
	projMat.mat[1][1] = fFovRad;
	projMat.mat[2][2] = fFar / (fFar - fNear);
	projMat.mat[3][2] = (-fFar * fNear) / (fFar - fNear);
	projMat.mat[2][3] = 1.0f;
	projMat.mat[3][3] = 0.0f;

	return true;
}
olc::Pixel GetColour (float lum) {

	int nValue = (int) (std::max (lum, 0.20f) * 255.0f);
	return olc::Pixel (nValue, nValue, nValue);
}

bool Engine3D::OnUserUpdate (float fElapsedTime) {
	FillRect (0, 0, ScreenWidth (), ScreenHeight (), olc::BLACK);

	mat4x4 matRotZ, matRotX;
	fTheta += 1.0f * fElapsedTime;

	// Rotation Z
	matRotZ.mat[0][0] = cosf (fTheta);
	matRotZ.mat[0][1] = sinf (fTheta);
	matRotZ.mat[1][0] = -sinf (fTheta);
	matRotZ.mat[1][1] = cosf (fTheta);
	matRotZ.mat[2][2] = 1;
	matRotZ.mat[3][3] = 1;

	// Rotation X
	matRotX.mat[0][0] = 1;
	matRotX.mat[1][1] = cosf (fTheta);
	matRotX.mat[1][2] = sinf (fTheta);
	matRotX.mat[2][1] = -sinf (fTheta);
	matRotX.mat[2][2] = cosf (fTheta);
	matRotX.mat[3][3] = 1;

	std::vector<triangle> trianglesToRasterize;

	// Draw Triangles

	for (auto tri : meshCube.tris) {
		triangle projectedTriangle, translatedTriangle, rotatedZTriangle, rotatedZXTriangle;

		// Project Z Matrix Rotation
		multiplyMatrixProjections (tri.p[0], rotatedZTriangle.p[0], matRotZ);
		multiplyMatrixProjections (tri.p[1], rotatedZTriangle.p[1], matRotZ);
		multiplyMatrixProjections (tri.p[2], rotatedZTriangle.p[2], matRotZ);
		
		// Project X Matrix Rotation
		multiplyMatrixProjections (rotatedZTriangle.p[0], rotatedZXTriangle.p[0], matRotX);
		multiplyMatrixProjections (rotatedZTriangle.p[1], rotatedZXTriangle.p[1], matRotX);
		multiplyMatrixProjections (rotatedZTriangle.p[2], rotatedZXTriangle.p[2], matRotX);

		// Offset Triangle into the screen
		translatedTriangle = rotatedZXTriangle;
		translatedTriangle.p[0].z = rotatedZXTriangle.p[0].z + 8.0f;
		translatedTriangle.p[1].z = rotatedZXTriangle.p[1].z + 8.0f;
		translatedTriangle.p[2].z = rotatedZXTriangle.p[2].z + 8.0f;

		// Use Cross-Product to get surface normal
		vec3d normal, line1, line2;
		line1.x = translatedTriangle.p[1].x - translatedTriangle.p[0].x;
		line1.y = translatedTriangle.p[1].y - translatedTriangle.p[0].y;
		line1.z = translatedTriangle.p[1].z - translatedTriangle.p[0].z;

		line2.x = translatedTriangle.p[2].x - translatedTriangle.p[0].x;
		line2.y = translatedTriangle.p[2].y - translatedTriangle.p[0].y;
		line2.z = translatedTriangle.p[2].z - translatedTriangle.p[0].z;

		normal.x = line1.y * line2.z - line1.z * line2.y;
		normal.y = line1.z * line2.x - line1.x * line2.z;
		normal.z = line1.x * line2.y - line1.y * line2.x;

		// It's normally normal to normalise the normal
		float lnL = sqrt (normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		if (lnL != 0.0f) {
			normal.x /= lnL; 
			normal.y /= lnL; 
			normal.z /= lnL;
		}

		float dpN = ((normal.x * (translatedTriangle.p[0].x - vCamera.x)) +
					(normal.y * (translatedTriangle.p[0].y - vCamera.y)) +
					(normal.z * (translatedTriangle.p[0].z - vCamera.z)));
		
		if (dpN < 0.0f) {

			// Illumination
			vec3d light_direction = {0.0f, 0.0f, -1.0f};
			float ldL = (light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
			if (ldL != 0.0f) {
				light_direction.x /= ldL;
				light_direction.y /= ldL;
				light_direction.z /= ldL;
			}

			float dpLD = ((normal.x * light_direction.x) +
						  (normal.y * light_direction.y) +
						  (normal.z * light_direction.z));

			translatedTriangle.col = GetColour (dpLD);

			// Project triangles from 3D --> 2D
			multiplyMatrixProjections (translatedTriangle.p[0], projectedTriangle.p[0], projMat);
			multiplyMatrixProjections (translatedTriangle.p[1], projectedTriangle.p[1], projMat);
			multiplyMatrixProjections (translatedTriangle.p[2], projectedTriangle.p[2], projMat);
			projectedTriangle.col = translatedTriangle.col;


			// Scale into view
			projectedTriangle.p[0].x += 1.0f; 
			projectedTriangle.p[0].y += 1.0f;
			projectedTriangle.p[1].x += 1.0f; 
			projectedTriangle.p[1].y += 1.0f;
			projectedTriangle.p[2].x += 1.0f; 
			projectedTriangle.p[2].y += 1.0f;
			projectedTriangle.p[0].x *= 0.5f * (float) ScreenWidth ();
			projectedTriangle.p[0].y *= 0.5f * (float) ScreenHeight ();
			projectedTriangle.p[1].x *= 0.5f * (float) ScreenWidth ();
			projectedTriangle.p[1].y *= 0.5f * (float) ScreenHeight ();
			projectedTriangle.p[2].x *= 0.5f * (float) ScreenWidth ();
			projectedTriangle.p[2].y *= 0.5f * (float) ScreenHeight ();

			trianglesToRasterize.push_back (projectedTriangle);
		}
	}

	// Sort triangles from back to front
	sort (trianglesToRasterize.begin (), trianglesToRasterize.end (), [](triangle &t1, triangle &t2) {
		float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
		float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
		return z1 > z2;
		});

	for (auto &projectedTriangle : trianglesToRasterize) {
		// Drawing the triangles using a 2D Matrix , to create a 3D illusion of an object on the screen
		FillTriangle (projectedTriangle.p[0].x, projectedTriangle.p[0].y,
			projectedTriangle.p[1].x, projectedTriangle.p[1].y,
			projectedTriangle.p[2].x, projectedTriangle.p[2].y,
			projectedTriangle.col);

		// Wire Frame for Debugging
		/*DrawTriangle (projectedTriangle.p[0].x, projectedTriangle.p[0].y,
			projectedTriangle.p[1].x, projectedTriangle.p[1].y,
			projectedTriangle.p[2].x, projectedTriangle.p[2].y,
			olc::BLACK);*/
	}

	return true;
}

void Engine3D::multiplyMatrixProjections (vec3d &input, vec3d &output, mat4x4 &m) {
	output.x = input.x * m.mat[0][0] + input.y * m.mat[1][0] + input.z * m.mat[2][0] + m.mat[3][0];
	output.y = input.x * m.mat[0][1] + input.y * m.mat[1][1] + input.z * m.mat[2][1] + m.mat[3][1];
	output.z = input.x * m.mat[0][2] + input.y * m.mat[1][2] + input.z * m.mat[2][2] + m.mat[3][2];
	float w = input.x * m.mat[0][3] + input.y * m.mat[1][3] + input.z * m.mat[2][3] + m.mat[3][3];

	if (w != 0.0f) {
		output.x /= w;
		output.y /= w;
		output.z /= w;
	}
}