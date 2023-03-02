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

	meshCube.loadFromObjectFile ("teapot.obj");


	// Projection Matrix
	projMat = projMat.project (90.0f, (float) ScreenHeight () / (float) ScreenWidth (), 0.1f, 1000.0f);

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
	matRotZ = matRotZ.rotateZ (fTheta * 0.5f);

	// Rotation X
	matRotX = matRotX.rotateX (fTheta);

	mat4x4 transMat;
	transMat = transMat.translate (0.0f, 0.0f, 8.0f);

	mat4x4 worldMat;
	worldMat = worldMat.identity ();
	worldMat = matRotZ * matRotX;
	worldMat = worldMat * transMat;

	std::vector<triangle> trianglesToRasterize;

	// Draw Triangles

	for (auto tri : meshCube.tris) {
		triangle projectedTriangle, transformedTriangle;

		transformedTriangle.p[0] = tri.p[0].project (worldMat);
		transformedTriangle.p[1] = tri.p[1].project (worldMat);
		transformedTriangle.p[2] = tri.p[2].project (worldMat);

		// Use Cross-Product to get surface normal
		vec3d normal, line1, line2;
		
		// Get lines of either side of triangle
		line1 = transformedTriangle.p[1] - transformedTriangle.p[0];
		line2 = transformedTriangle.p[2] - transformedTriangle.p[0];

		// Take cross product of lines to get normal to triangle surface
		normal = line1.crossProduct (line2);

		// Normalize the normal
		normal = normal.normalize ();

		// Get ray from triangle to camera
		vec3d vCameraRay = transformedTriangle.p[0] - vCamera;
		
		if (normal.dotProduct(vCameraRay) < 0.0f) {

			// Illumination
			vec3d light_direction = {0.0f, 0.0f, -1.0f};
			light_direction = light_direction.normalize ();

			// Alignment of light to triangle surface
			float dpLD = std::max (0.1f, light_direction.dotProduct (normal));

			transformedTriangle.col = GetColour (dpLD);

			// Project triangles from 3D --> 2D
			projectedTriangle.p[0] = transformedTriangle.p[0].project(projMat);
			projectedTriangle.p[1] = transformedTriangle.p[1].project (projMat);
			projectedTriangle.p[2] = transformedTriangle.p[2].project (projMat);
			projectedTriangle.col = transformedTriangle.col;

			// Normalize points
			projectedTriangle.p[0] = projectedTriangle.p[0] / projectedTriangle.p[0].w;
			projectedTriangle.p[1] = projectedTriangle.p[1] / projectedTriangle.p[1].w;
			projectedTriangle.p[2] = projectedTriangle.p[2] / projectedTriangle.p[2].w;

			// Offset vertices into normalized screen space
			vec3d vOffsetView = {1, 1, 0};
			projectedTriangle.p[0] = projectedTriangle.p[0] + vOffsetView;
			projectedTriangle.p[1] = projectedTriangle.p[1] + vOffsetView;
			projectedTriangle.p[2] = projectedTriangle.p[2] + vOffsetView;
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

Engine3D::vec3d Engine3D::vec3d::operator+ (vec3d &vec) {
	return {this->x + vec.x, this->y + vec.y, this->z + vec.z};
}

Engine3D::vec3d Engine3D::vec3d::operator- (vec3d &vec) {
	return {this->x - vec.x, this->y - vec.y, this->z - vec.z};
}

Engine3D::vec3d Engine3D::vec3d::operator* (float k) {
	return {this->x * k, this->y * k, this->z * k};
}

Engine3D::vec3d Engine3D::vec3d::operator/ (float k) {
	return {this->x / k, this->y / k, this->z / k};
}

float Engine3D::vec3d::dotProduct (vec3d &vec) {
	return this->x * vec.x + this->y * vec.y + this->z * vec.z;
}

float Engine3D::vec3d::len () {
	return sqrtf (this->dotProduct (*this));
}

Engine3D::vec3d Engine3D::vec3d::normalize () {
	float l = this->len ();
	return {this->x / l, this->y / l, this->z / l};
}

Engine3D::vec3d Engine3D::vec3d::crossProduct (vec3d &vec) {
	vec3d v;
	v.x = this->y * vec.z - this->z * vec.y;
	v.y = this->z * vec.x - this->x * vec.z;
	v.z = this->x * vec.y - this->y * vec.x;
	return v;
}

Engine3D::vec3d Engine3D::vec3d::project (mat4x4 &mat) {
	vec3d v;
	v.x = this->x * mat.mat[0][0] + this->y * mat.mat[1][0] + this->z * mat.mat[2][0] + this->w * mat.mat[3][0];
	v.y = this->x * mat.mat[0][1] + this->y * mat.mat[1][1] + this->z * mat.mat[2][1] + this->w * mat.mat[3][1];
	v.z = this->x * mat.mat[0][2] + this->y * mat.mat[1][2] + this->z * mat.mat[2][2] + this->w * mat.mat[3][2];
	v.w = this->x * mat.mat[0][3] + this->y * mat.mat[1][3] + this->z * mat.mat[2][3] + this->w * mat.mat[3][3];
	return v;
}

Engine3D::mat4x4 Engine3D::mat4x4::identity () {
	mat4x4 mat;
	mat.mat[0][0] = 1.0f;
	mat.mat[1][1] = 1.0f;
	mat.mat[2][2] = 1.0f;
	mat.mat[3][3] = 1.0f;
	return mat;
}

Engine3D::mat4x4 Engine3D::mat4x4::rotateX (float fAngleRad) {
	mat4x4 mat;
	mat.mat[0][0] = 1.0f;
	mat.mat[1][1] = cosf (fAngleRad);
	mat.mat[1][2] = sinf (fAngleRad);
	mat.mat[2][1] = -sinf (fAngleRad);
	mat.mat[2][2] = cosf (fAngleRad);
	mat.mat[3][3] = 1.0f;
	return mat;
}

Engine3D::mat4x4 Engine3D::mat4x4::rotateY (float fAngleRad) {
	mat4x4 mat;
	mat.mat[0][0] = cosf (fAngleRad);
	mat.mat[0][2] = sinf (fAngleRad);
	mat.mat[2][0] = -sinf (fAngleRad);
	mat.mat[1][1] = 1.0f;
	mat.mat[2][2] = cosf (fAngleRad);
	mat.mat[3][3] = 1.0f;
	return mat;
}

Engine3D::mat4x4 Engine3D::mat4x4::rotateZ (float fAngleRad) {
	mat4x4 mat;
	mat.mat[0][0] = cosf (fAngleRad);
	mat.mat[0][1] = sinf (fAngleRad);
	mat.mat[1][0] = -sinf (fAngleRad);
	mat.mat[1][1] = cosf (fAngleRad);
	mat.mat[2][2] = 1.0f;
	mat.mat[3][3] = 1.0f;
	return mat;
}

Engine3D::mat4x4 Engine3D::mat4x4::translate (float x, float y, float z) {
	mat4x4 mat;
	mat.mat[0][0] = 1.0f;
	mat.mat[1][1] = 1.0f;
	mat.mat[2][2] = 1.0f;
	mat.mat[3][3] = 1.0f;
	mat.mat[3][0] = x;
	mat.mat[3][1] = y;
	mat.mat[3][2] = z;
	return mat;
}

Engine3D::mat4x4 Engine3D::mat4x4::project (float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
	float fFovRad = 1.0f / tanf (fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat4x4 mat;
	mat.mat[0][0] = fAspectRatio * fFovRad;
	mat.mat[1][1] = fFovRad;
	mat.mat[2][2] = fFar / (fFar - fNear);
	mat.mat[3][2] = (-fFar * fNear) / (fFar - fNear);
	mat.mat[2][3] = 1.0f;
	mat.mat[3][3] = 0.0f;
	return mat;
}

Engine3D::mat4x4 Engine3D::mat4x4::operator* (mat4x4 &mat) {
	mat4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.mat[r][c] = this->mat[r][0] * mat.mat[0][c] + this->mat[r][1] * 
			mat.mat[1][c] + this->mat[r][2] * mat.mat[2][c] + this->mat[r][3] * mat.mat[3][c];
	return matrix;
}