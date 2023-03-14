#include "olcPixelGameEngine.h"
#include "Engine3D.h"

#include <fstream>
#include <strstream>
#include <algorithm>

Engine3D::Engine3D () {
	sAppName = "3D Renderer/Engine";
	pDepthBuffer = nullptr;
	sprTex = nullptr;
	decalTex = nullptr;
	numSprites = 4;
	whichSpritesIndex = 0;
}

bool Engine3D::OnUserCreate () {
	// Index to determine which sprites to load

	pDepthBuffer = new float[ScreenWidth () * ScreenHeight ()];

	meshCube = Mesh ("lamp.obj", true);

	sprTex = new olc::Sprite ("lampnormal.png");

	vSprites.push_back (sprTex);

	sprTex = new olc::Sprite ("lampmetal.png");

	vSprites.push_back (sprTex);

	sprTex = new olc::Sprite ("lamprough.png");

	vSprites.push_back (sprTex);

	sprTex = new olc::Sprite ("lamp.png");

	vSprites.push_back (sprTex);


	for (int i = whichSpritesIndex; i < numSprites; i++) {
		decalTex = new olc::Decal (vSprites.at (i));
		vDecals.push_back (decalTex);
	}

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
		vCamera.setY (vCamera.getY () + (2.0f * fElapsedTime));
	}

	// Travel Downwards
	if (GetKey (olc::DOWN).bHeld) {
		vCamera.setY (vCamera.getY () - (2.0f * fElapsedTime));
	}

	// Look Left
	if (GetKey (olc::RIGHT).bHeld) {
		vCamera.setX (vCamera.getX () + (2.0f * fElapsedTime));
	}
	
	// Look Right
	if (GetKey (olc::LEFT).bHeld) {
		vCamera.setX (vCamera.getX () - (2.0f * fElapsedTime));
	}

	Vec3D vForward (vLookDirection * (2.0f * fElapsedTime));

	// Standard FPS Control scheme, but turn instead of strafe
	if (GetKey (olc::W).bHeld)
		vCamera = (vCamera + vForward);

	if (GetKey (olc::S).bHeld)
		vCamera = (vCamera - vForward);

	if (GetKey (olc::A).bHeld)
		fYaw -= 0.5f * fElapsedTime;

	if (GetKey (olc::D).bHeld)
		fYaw += 0.5f * fElapsedTime;

	Mat4x4 matRotZ, matRotX;
	// fTheta += 1.0f * fElapsedTime;

	// Rotation Z
	matRotZ = Mat4x4::rotateZ (fTheta * 0.5f);

	// Rotation X
	matRotX = Mat4x4::rotateX (fTheta);

	Mat4x4 transMat;
	transMat = Mat4x4::translate (0.0f, 0.0f, 2.0f);

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
	std::vector<std::vector<Triangle *>> trianglesToRasterize;

	// Draw Triangles

	for (auto &tri : meshCube.getTris ()) {
		std::vector<Triangle *> layerTris;
		Triangle projectedTriangle, transformedTriangle, viewedTriangle;

		for (int i = whichSpritesIndex; i < vDecals.size (); i++) {
			layerTris.push_back (new Triangle (transformedTriangle));
			for (int j = 0; j < transformedTriangle.getSize (); j++) {
				layerTris.at (i)->setP (j, (tri.getP (j) * Mat4x4::scale (float(5 + (0.1 * i)), worldMat)));
				layerTris.at (i)->setT (j, tri.getT (j));
			}
		}

		/*for (int i = 0; i < transformedTriangle.getSize (); i++) {
			transformedTriangle.setP (i, (tri.getP (i) * worldMat));
			transformedTriangle.setT (i, tri.getT (i));
		}*/

		// Use Cross-Product to get surface normal
		Vec3D line1, line2, normal;

		for (int cycle = 0; cycle < layerTris.size (); cycle++) {
			// Get lines of either side of triangle
			line1 = layerTris.at (cycle)->getP (1) - layerTris.at (cycle)->getP (0);
			line2 = layerTris.at (cycle)->getP (2) - layerTris.at (cycle)->getP (0);

			// Take cross product of lines to get normal to triangle surface
			normal = Vec3D::crossProduct (line1, line2);

			// Normalize the normal
			normal = Vec3D::normalize (normal);

			// Get ray from triangle to camera
			Vec3D vCameraRay = layerTris.at (cycle)->getP (0) - vCamera;

			if (Vec3D::dotProduct (normal, vCameraRay) < 0.0f) {

				// Illumination
				//Vec3D light_direction (0.0f, 1.0f, -1.0f);
				//light_direction = Vec3D::normalize (light_direction);

				// Alignment of light to triangle surface
				// float dpLD = std::max (0.1f, Vec3D::dotProduct (light_direction, normal));

				// transformedTriangle.setColor(GetColour (dpLD));

				// Convert World Space --> View Space
				for (int j = 0; j < viewedTriangle.getSize (); j++) {
					layerTris.at (cycle)->setP (j, (layerTris.at (cycle)->getP (j) * viewMatrix));
					layerTris.at (cycle)->setT (j, layerTris.at (cycle)->getT (j));
				}

				/*for (int i = 0; i < viewedTriangle.getSize (); i++) {
					viewedTriangle.setP (i, (transformedTriangle.getP (i) * viewMatrix));
					viewedTriangle.setT (i, transformedTriangle.getT (i));
				}
				viewedTriangle.setColor (transformedTriangle.getColor ());*/

				// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles. 
				int nClippedTriangles = 0;
				Triangle clipped[2];
				std::vector <Triangle> layerClipped[2];
				nClippedTriangles += clipAgainstPlane ({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, *layerTris.at (cycle), clipped[0], clipped[1]);
				layerClipped[0].push_back (clipped[0]);
				layerClipped[1].push_back (clipped[1]);
				// nClippedTriangles += clipAgainstPlane ({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, viewedTriangle, clipped[0], clipped[1]);
				Vec3D vOffsetView = {1, 1, 0};

				for (int n = 0; n < layerClipped->size (); n++) {
					// Project triangles from 3D --> 2D
					for (int i = 0; i < projectedTriangle.getSize (); i++) {
						layerTris.at (cycle)->setP (i, layerClipped[0].at (n).getP (i) * projMat);
						layerTris.at (cycle)->setT (i, (layerClipped[0].at (n).getT (i)));
						layerTris.at (cycle)->setT (i, (Vec2D (layerTris.at (cycle)->getT (i).getU () / layerTris.at (cycle)->getP (i).getW (), layerTris.at (cycle)->getT (i).getV () / layerTris.at (cycle)->getP (i).getW (), 1.0f / layerTris.at (cycle)->getP (i).getW ())));
						layerTris.at (cycle)->setP (i, layerTris.at (cycle)->getP (i) / layerTris.at (cycle)->getP (i).getW ());
						layerTris.at (cycle)->setP (i, Vec3D (layerTris.at (cycle)->getP (i).getX () * -1.0f, layerTris.at (cycle)->getP (i).getY () * -1.0f, layerTris.at (cycle)->getP (i).getZ ()));
						layerTris.at (cycle)->setP (i, layerTris.at (cycle)->getP (i) + vOffsetView);
						layerTris.at (cycle)->setP (i, Vec3D (layerTris.at (cycle)->getP (i).getX () * (0.5f * (float) ScreenWidth ()), layerTris.at (cycle)->getP (i).getY () * (0.5f * (float) ScreenHeight ()), layerTris.at (cycle)->getP (i).getZ ()));
					}
					/*for (int i = 0; i < projectedTriangle.getSize (); i++) {
						projectedTriangle.setP (i, (clipped[n].getP (i) * projMat));
						projectedTriangle.setT (i, (clipped[n].getT (i)));
						projectedTriangle.setT (i, (Vec2D (projectedTriangle.getT (i).getU () / projectedTriangle.getP (i).getW (), projectedTriangle.getT (i).getV () / projectedTriangle.getP (i).getW (), 1.0f / projectedTriangle.getP (i).getW ())));
						projectedTriangle.setP (i, projectedTriangle.getP (i) / projectedTriangle.getP (i).getW ());
						projectedTriangle.setP (i, Vec3D (projectedTriangle.getP (i).getX () * -1.0f, projectedTriangle.getP (i).getY () * -1.0f, projectedTriangle.getP (i).getZ ()));
						projectedTriangle.setP (i, projectedTriangle.getP (i) + vOffsetView);
						projectedTriangle.setP (i, Vec3D (projectedTriangle.getP (i).getX () *(0.5f * (float) ScreenWidth ()), projectedTriangle.getP (i).getY () *(0.5f * (float) ScreenHeight ()), projectedTriangle.getP (i).getZ ()));
					}
					projectedTriangle.setColor (clipped[n].getColor ());*/
					
					// trianglesToRasterize.push_back (projectedTriangle);
				}
				trianglesToRasterize.push_back (layerTris);
			}
		}
	}

	// Sort triangles from back to front
	/*std::sort (trianglesToRasterize.begin (), trianglesToRasterize.end (), [](Triangle &t1, Triangle &t2) {
		float z1 = (t1.getP (0).getZ () + t1.getP (1).getZ () + t1.getP (2).getZ ()) / 3.0f;
		float z2 = (t2.getP (0).getZ () + t2.getP (1).getZ () + t2.getP (2).getZ ()) / 3.0f;
		return z1 > z2;
		});*/

	FillRect (0, 0, ScreenWidth (), ScreenHeight (), olc::WHITE);

	for (int i = 0; i < ScreenWidth () * ScreenHeight (); i++) {
		pDepthBuffer[i] = 0.0f;
	}

	for (int cycles = 0; cycles < vDecals.size (); cycles++) {
		for (auto &tri : trianglesToRasterize) {
			// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to 
			//  ensure we only test new triangles generated against planes
			Triangle clipped[2];
			std::list<Triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back (*tri.at (cycles));
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
				// FillTriangle (t.getP (0).getX (), t.getP (0).getY (), t.getP (1).getX (), t.getP (1).getY (), t.getP (2).getX (), t.getP (2).getY (), t.getColor ());
				DrawTexturedTriangle (t.getP (0).getX (), t.getP (0).getY (), t.getT (0).getU (), t.getT (0).getV (), t.getT (0).getW (),
					t.getP (1).getX (), t.getP (1).getY (), t.getT (1).getU (), t.getT (1).getV (), t.getT (1).getW (),
					t.getP (2).getX (), t.getP (2).getY (), t.getT (2).getU (), t.getT (2).getV (), t.getT (2).getW (), vDecals.at (cycles));

				// Wire Frame for Debugging
				// DrawTriangle (t.getP (0).getX (), t.getP (0).getY (), t.getP (1).getX (), t.getP (1).getY (), t.getP (2).getX (), t.getP (2).getY (), olc::BLACK);
			}

		}
	}

	return true;
}

void Engine3D::DrawTexturedTriangle (int x1, int y1, float u1, float v1, float w1,
									 int x2, int y2, float u2, float v2, float w2,
									 int x3, int y3, float u3, float v3, float w3,
									 olc::Decal *texture) {
	if (y2 < y1) {
		std::swap (y1, y2);
		std::swap (x1, x2);
		std::swap (u1, u2);
		std::swap (v1, v2);
		std::swap (w1, w2);
	}

	if (y3 < y1) {
		std::swap (y1, y3);
		std::swap (x1, x3);
		std::swap (u1, u3);
		std::swap (v1, v3);
		std::swap (w1, w3);
	}

	if (y3 < y2) {
		std::swap (y2, y3);
		std::swap (x2, x3);
		std::swap (u2, u3);
		std::swap (v2, v3);
		std::swap (w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float) abs (dy1);
	if (dy2) dbx_step = dx2 / (float) abs (dy2);

	if (dy1) du1_step = du1 / (float) abs (dy1);
	if (dy1) dv1_step = dv1 / (float) abs (dy1);
	if (dy1) dw1_step = dw1 / (float) abs (dy1);

	if (dy2) du2_step = du2 / (float) abs (dy2);
	if (dy2) dv2_step = dv2 / (float) abs (dy2);
	if (dy2) dw2_step = dw2 / (float) abs (dy2);

	if (dy1) {
		for (int i = y1; i <= y2; i++) {
			int ax = x1 + (float) (i - y1) * dax_step;
			int bx = x1 + (float) (i - y1) * dbx_step;

			float tex_su = u1 + (float) (i - y1) * du1_step;
			float tex_sv = v1 + (float) (i - y1) * dv1_step;
			float tex_sw = w1 + (float) (i - y1) * dw1_step;

			float tex_eu = u1 + (float) (i - y1) * du2_step;
			float tex_ev = v1 + (float) (i - y1) * dv2_step;
			float tex_ew = w1 + (float) (i - y1) * dw2_step;

			if (ax > bx) {
				std::swap (ax, bx);
				std::swap (tex_su, tex_eu);
				std::swap (tex_sv, tex_ev);
				std::swap (tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float) (bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++) {
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;
				if (tex_w > pDepthBuffer[i * ScreenWidth () + j]) {
					Draw (j, i, texture->sprite->Sample (tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * ScreenWidth () + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float) abs (dy1);
	if (dy2) dbx_step = dx2 / (float) abs (dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float) abs (dy1);
	if (dy1) dv1_step = dv1 / (float) abs (dy1);
	if (dy1) dw1_step = dw1 / (float) abs (dy1);

	if (dy1) {
		for (int i = y2; i <= y3; i++) {
			int ax = x2 + (float) (i - y2) * dax_step;
			int bx = x1 + (float) (i - y1) * dbx_step;

			float tex_su = u2 + (float) (i - y2) * du1_step;
			float tex_sv = v2 + (float) (i - y2) * dv1_step;
			float tex_sw = w2 + (float) (i - y2) * dw1_step;

			float tex_eu = u1 + (float) (i - y1) * du2_step;
			float tex_ev = v1 + (float) (i - y1) * dv2_step;
			float tex_ew = w1 + (float) (i - y1) * dw2_step;

			if (ax > bx) {
				std::swap (ax, bx);
				std::swap (tex_su, tex_eu);
				std::swap (tex_sv, tex_ev);
				std::swap (tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float) (bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++) {
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * ScreenWidth () + j]) {
					Draw (j, i, texture->sprite->Sample (tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * ScreenWidth () + j] = tex_w;
				}
				t += tstep;
			}
		}
	}
}


Mat4x4 Engine3D::pointAt (Vec3D &pos, Vec3D &target, Vec3D &up) {
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

	float u, v, w;
	Vec2D tmpVec;

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
	Vec2D inside_tex[3];
	int nInsideTexturePointCount = 0;
	Vec2D outside_tex[3];
	int nOutsideTexturePointCount = 0;

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
		inside_tex[nInsideTexturePointCount++] = in_tri.getT (0);
	}
	else { 
		outside_points[nOutsidePointCount++] = in_tri.getP (0);
		outside_tex[nOutsideTexturePointCount++] = in_tri.getT (0);
	};
	if (d1 >= 0) { 
		inside_points[nInsidePointCount++] = in_tri.getP (1);
		inside_tex[nInsideTexturePointCount++] = in_tri.getT (1);
	}
	else { 
		outside_points[nOutsidePointCount++] = in_tri.getP (1); 
		outside_tex[nOutsideTexturePointCount++] = in_tri.getT (1);
	}
	if (d2 >= 0) { 
		inside_points[nInsidePointCount++] = in_tri.getP (2);
		inside_tex[nInsideTexturePointCount++] = in_tri.getT (2);
	}
	else { 
		outside_points[nOutsidePointCount++] = in_tri.getP (2); 
		outside_tex[nOutsideTexturePointCount++] = in_tri.getT (2);
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
		out_tri1.setT (0, inside_tex[0]);

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane

		float t;
		out_tri1.setP (1, Vec3D::intersectPlane (plane_p, plane_n, inside_points[0], outside_points[0], t));

		u = t * (outside_tex[0].getU () - inside_tex[0].getU ()) + inside_tex[0].getU ();
		v = t * (outside_tex[0].getV () - inside_tex[0].getV ()) + inside_tex[0].getV ();
		w = t * (outside_tex[0].getW () - inside_tex[0].getW ()) + inside_tex[0].getW ();

		tmpVec.setU (u);
		tmpVec.setV (v);
		tmpVec.setW (w);

		out_tri1.setT (1, tmpVec);

		out_tri1.setP (2, Vec3D::intersectPlane (plane_p, plane_n, inside_points[0], outside_points[1], t));

		u = t * (outside_tex[1].getU () - inside_tex[0].getU ()) + inside_tex[0].getU ();
		v = t * (outside_tex[1].getV () - inside_tex[0].getV ()) + inside_tex[0].getV ();
		w = t * (outside_tex[1].getW () - inside_tex[0].getW ()) + inside_tex[0].getW ();

		tmpVec.setU (u);
		tmpVec.setV (v);
		tmpVec.setW (w);

		out_tri1.setT (2, tmpVec);

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
		out_tri1.setT (0, inside_tex[0]);
		out_tri1.setT (1, inside_tex[1]);

		float t;
		out_tri1.setP (2, Vec3D::intersectPlane (plane_p, plane_n, inside_points[0], outside_points[0], t));

		u = t * (outside_tex[0].getU () - inside_tex[0].getU ()) + inside_tex[0].getU ();
		v = t * (outside_tex[0].getV () - inside_tex[0].getV ()) + inside_tex[0].getV ();
		w = t * (outside_tex[0].getW () - inside_tex[0].getW ()) + inside_tex[0].getW ();

		tmpVec.setU (u);
		tmpVec.setV (v);
		tmpVec.setW (w);

		out_tri1.setT (2, tmpVec);

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.setP (0, inside_points[1]);
		out_tri2.setT (0, inside_tex[1]);
		out_tri2.setP (1, out_tri1.getP (2));
		out_tri2.setT (1, out_tri1.getT (2));
		out_tri2.setP (2, Vec3D::intersectPlane (plane_p, plane_n, inside_points[1], outside_points[0], t));
		u = t * (outside_tex[0].getU () - inside_tex[1].getU ()) + inside_tex[1].getU ();
		v = t * (outside_tex[0].getV () - inside_tex[1].getV ()) + inside_tex[1].getV ();
		w = t * (outside_tex[0].getW () - inside_tex[1].getW ()) + inside_tex[1].getW ();

		tmpVec.setU (u);
		tmpVec.setV (v);
		tmpVec.setW (w);

		out_tri2.setT (2, tmpVec);
		
		returnCase = 2; // Return two newly formed triangles which form a quad
	}

	return returnCase;
}