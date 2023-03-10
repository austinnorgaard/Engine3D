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

	meshCube = Mesh ("mountains.obj");

	// Projection Matrix
	projMat = projMat.project (90.0f, (float) ScreenHeight () / (float) ScreenWidth (), 0.1f, 1000.0f);

	return true;
}
olc::Pixel GetColour (float lum) {

	int nValue = (int) (std::max (lum, 0.20f) * 255.0f);
	return olc::Pixel (nValue, nValue, nValue);
}

bool Engine3D::OnUserUpdate (float fElapsedTime) {
	// Travel Upwards
	if (GetKey (olc::UP).bHeld) {
		vCamera.setY (vCamera.getY () + (8.0f * fElapsedTime));
	}

	// Travel Downwards
	if (GetKey (olc::DOWN).bHeld) {
		vCamera.setY (vCamera.getY () - (8.0f * fElapsedTime));
	}

	// Look Left
	if (GetKey (olc::RIGHT).bHeld) {
		vCamera.setX (vCamera.getX () + (8.0f * fElapsedTime));
	}
	
	// Look Right
	if (GetKey (olc::LEFT).bHeld) {
		vCamera.setX (vCamera.getX () - (8.0f * fElapsedTime));
	}

	Vec3D vForward (vLookDirection * (8.0f * fElapsedTime));

	// Standard FPS Control scheme, but turn instead of strafe
	if (GetKey (olc::W).bHeld)
		vCamera = (vCamera + vForward);

	if (GetKey (olc::S).bHeld)
		vCamera = (vCamera - vForward);

	if (GetKey (olc::A).bHeld)
		fYaw -= 2.0f * fElapsedTime;

	if (GetKey (olc::D).bHeld)
		fYaw += 2.0f * fElapsedTime;

	Mat4x4 matRotZ, matRotX;
	// fTheta += 1.0f * fElapsedTime;

	// Rotation Z
	matRotZ = Mat4x4::rotateZ (fTheta * 0.5f);

	// Rotation X
	matRotX = Mat4x4::rotateX (fTheta);

	Mat4x4 transMat;
	transMat = Mat4x4::translate (0.0f, 0.0f, 5.0f);

	Mat4x4 worldMat;
	worldMat = Mat4x4::identity ();
	worldMat = matRotZ * matRotX;
	worldMat = worldMat * transMat;

	Vec3D vUp (0, 1, 0);
	Vec3D vTarget (0, 0, 1);
	Mat4x4 rotationCamera (Mat4x4::rotateY(fYaw));
	vLookDirection = vTarget * rotationCamera;
	vTarget = vCamera + vLookDirection;
	Mat4x4 cameraMatrix = pointAt (vCamera, vTarget, vUp);

	// Make view matrix from camera
	Mat4x4 viewMatrix = Mat4x4::inverse (cameraMatrix);

	// Store triangles
	std::vector<Triangle> trianglesToRasterize;

	// Draw Triangles

	for (auto &tri : meshCube.getTris ()) {
		Triangle projectedTriangle, transformedTriangle, viewedTriangle;

		for (int i = 0; i < transformedTriangle.getSize (); i++) {
			transformedTriangle.setP (i, (tri.getP (i) * worldMat));
		}

		// Use Cross-Product to get surface normal
		Vec3D normal, line1, line2;
		
		// Get lines of either side of triangle
		line1 = transformedTriangle.getP (1) - transformedTriangle.getP (0);
		line2 = transformedTriangle.getP (2) - transformedTriangle.getP (0);

		// Take cross product of lines to get normal to triangle surface
		normal = Vec3D::crossProduct (line1, line2);

		// Normalize the normal
		normal = Vec3D::normalize (normal);

		// Get ray from triangle to camera
		Vec3D vCameraRay = transformedTriangle.getP (0) - vCamera;
		
		if (Vec3D::dotProduct(normal, vCameraRay) < 0.0f) {

			// Illumination
			Vec3D light_direction (0.0f, 1.0f, -1.0f);
			light_direction = Vec3D::normalize (light_direction);

			// Alignment of light to triangle surface
			float dpLD = std::max (0.1f, Vec3D::dotProduct (light_direction, normal));

			transformedTriangle.setColor(GetColour (dpLD));

			// Convert World Space --> View Space
			for (int i = 0; i < viewedTriangle.getSize (); i++) {
				viewedTriangle.setP (i, (transformedTriangle.getP (i) *viewMatrix));
			}
			viewedTriangle.setColor (transformedTriangle.getColor ());

			// Clip Viewed Triangle against near plane, this could form two additional
			// additional triangles. 
			int nClippedTriangles = 0;
			Triangle clipped[2];
			nClippedTriangles = clipAgainstPlane ({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, viewedTriangle, clipped[0], clipped[1]);
			Vec3D vOffsetView = {1, 1, 0};

			for (int n = 0; n < nClippedTriangles; n++) {
				// Project triangles from 3D --> 2D
				for (int i = 0; i < projectedTriangle.getSize (); i++) {
					projectedTriangle.setP (i, (clipped[n].getP (i) * projMat));
					projectedTriangle.setP (i, projectedTriangle.getP (i) / projectedTriangle.getP (i).getW ());
					projectedTriangle.setP (i, Vec3D (projectedTriangle.getP (i).getX () * -1.0f, projectedTriangle.getP (i).getY () * -1.0f, projectedTriangle.getP (i).getZ ()));
					projectedTriangle.setP (i, projectedTriangle.getP (i) + vOffsetView);
					projectedTriangle.setP (i, Vec3D (projectedTriangle.getP (i).getX () *(0.5f * (float) ScreenWidth ()), projectedTriangle.getP (i).getY () *(0.5f * (float) ScreenHeight ()), projectedTriangle.getP (i).getZ ()));
				}
				projectedTriangle.setColor (clipped[n].getColor ());

				trianglesToRasterize.push_back (projectedTriangle);
			}
		}
	}

	// Sort triangles from back to front
	std::sort (trianglesToRasterize.begin (), trianglesToRasterize.end (), [](Triangle &t1, Triangle &t2) {
		float z1 = (t1.getP (0).getZ () + t1.getP (1).getZ () + t1.getP (2).getZ ()) / 3.0f;
		float z2 = (t2.getP (0).getZ () + t2.getP (1).getZ () + t2.getP (2).getZ ()) / 3.0f;
		return z1 > z2;
		});

	FillRect (0, 0, ScreenWidth (), ScreenHeight (), olc::BLACK);

	for (auto &tri : trianglesToRasterize) {
		// Clip triangles against all four screen edges, this could yield
		// a bunch of triangles, so create a queue that we traverse to 
		//  ensure we only test new triangles generated against planes
		Triangle clipped[2];
		std::list<Triangle> listTriangles;

		// Add initial triangle
		listTriangles.push_back (tri);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++) {
			int nTrisToAdd = 0;
			while (nNewTriangles > 0) {
				// Take triangle from front of queue
				Triangle test = listTriangles.front ();
				listTriangles.pop_front ();
				nNewTriangles--;

				// Clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p) {
				case 0:	nTrisToAdd = clipAgainstPlane ({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, test, clipped[0], clipped[1]);
					break;
				case 1:	nTrisToAdd = clipAgainstPlane ({0.0f, (float) ScreenHeight () - 1, 0.0f}, {0.0f, -1.0f, 0.0f}, test, clipped[0], clipped[1]);
					break;
				case 2:	nTrisToAdd = clipAgainstPlane ({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
					break;
				case 3:	nTrisToAdd = clipAgainstPlane ({(float) ScreenWidth () - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]); 
					break;
				}

				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w < nTrisToAdd; w++) {
					listTriangles.push_back (clipped[w]);
				}
			}
			nNewTriangles = listTriangles.size ();
		}


		// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		for (auto &t : listTriangles) {
		FillTriangle (t.getP (0).getX (), t.getP (0).getY (), t.getP (1).getX (), t.getP (1).getY (), t.getP (2).getX (), t.getP (2).getY (), t.getColor ());
		// Wire Frame for Debugging
			/*DrawTriangle (t.getP (0).getX (), t.getP (0).getY (),
			t.getP (1).getX (), t.getP (1).getY (),
			t.getP (2).getX (), t.getP (2).getY (),
			olc::WHITE);*/
		}
		
	}

	return true;
}

Mat4x4 Engine3D::pointAt (Vec3D &pos, Vec3D &target, Vec3D &up) const {
	// Calculate new forward direction
	Vec3D newForward = target - pos;
	newForward = Vec3D::normalize (newForward);

	// Calculate new up direction
	Vec3D a = newForward * (Vec3D::dotProduct (up, newForward));
	Vec3D newUp = up - a;
	newUp = Vec3D::normalize (newUp);

	// Calculate new right direction
	Vec3D newRight = Vec3D::crossProduct (newUp, newForward);

	// Add Vec3D objects to vector to clean up code
	std::vector<Vec3D *> pVecs;
	pVecs.push_back (&newRight);
	pVecs.push_back (&newUp);
	pVecs.push_back (&newForward);
	pVecs.push_back (&pos);

	// Construct Dimensioning and Translation Matrix
	Mat4x4 matrix;

	int index = 0;

	for (auto vec : pVecs) {
		matrix.setMat (index, 0, vec->getX ());
		matrix.setMat (index, 1, vec->getY ());
		matrix.setMat (index, 2, vec->getZ ());
		matrix.setMat (index, 3, 0.0f);
		index++;
	}

	matrix.setMat (3, 3, 1.0f);

	return matrix;
}

int Engine3D::clipAgainstPlane (Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2) {
	// Make sure plane normal is indeed normal
	plane_n = Vec3D::normalize (plane_n);

	// Return case instead of multiple returns
	int returnCase = 0;

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](Vec3D &p) {
		Vec3D n = Vec3D::normalize (p);
		return (plane_n.getX () * p.getX () + plane_n.getY () * p.getY () + plane_n.getZ () * p.getZ () - Vec3D::dotProduct (plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	Vec3D inside_points[3];  
	int nInsidePointCount = 0;
	Vec3D outside_points[3]; 
	int nOutsidePointCount = 0;

	// Get signed distance of each point in triangle to plane
	Vec3D p = Vec3D();
	p = in_tri.getP (0);
	float d0 = dist (p);
	p = in_tri.getP (1);
	float d1 = dist (p);
	p = in_tri.getP (2);
	float d2 = dist (p);

	if (d0 >= 0) {
		inside_points[nInsidePointCount++] = in_tri.getP (0); 
	}
	else { 
		outside_points[nOutsidePointCount++] = in_tri.getP (0); 
	};
	if (d1 >= 0) { 
		inside_points[nInsidePointCount++] = in_tri.getP (1); 
	}
	else { 
		outside_points[nOutsidePointCount++] = in_tri.getP (1); 
	}
	if (d2 >= 0) { 
		inside_points[nInsidePointCount++] = in_tri.getP (2); 
	}
	else { 
		outside_points[nOutsidePointCount++] = in_tri.getP (2); 
	}

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0) {
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		returnCase = 0; // No returned triangles are valid
	}

	else if (nInsidePointCount == 3) {
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		returnCase = 1; // Just the one returned original triangle is valid
	}

	else if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
		out_tri1.setColor (in_tri.getColor ());

		// The inside point is valid, so keep that...
		out_tri1.setP (0, inside_points[0]);

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		out_tri1.setP (1, Vec3D::intersectPlane (plane_p, plane_n, inside_points[0], outside_points[0]));
		out_tri1.setP (2, Vec3D::intersectPlane (plane_p, plane_n, inside_points[0], outside_points[1]));

		returnCase = 1; // Return the newly formed single triangle
	}

	else if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles
		out_tri1.setColor (in_tri.getColor ());

		out_tri2.setColor (in_tri.getColor ());

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.setP (0, inside_points[0]);
		out_tri1.setP (1, inside_points[1]);
		out_tri1.setP (2, Vec3D::intersectPlane (plane_p, plane_n, inside_points[0], outside_points[0]));

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.setP (0, inside_points[1]);
		out_tri2.setP (1, out_tri1.getP (2));
		out_tri2.setP (2, Vec3D::intersectPlane (plane_p, plane_n, inside_points[1], outside_points[0]));

		returnCase = 2; // Return two newly formed triangles which form a quad
	}

	return returnCase;
}