#include "olcPixelGameEngine.h"
#include "Engine3D.h"

#include <fstream>
#include <strstream>
#include <algorithm>

Engine3D::Engine3D (std::vector<std::string> objectFileNames, std::vector<std::string> textureFileNames, bool bIsTexture) {
	sAppName = "3D Renderer/Engine";
	pDepthBuffer = nullptr;
	mObjectFileNames = objectFileNames;
	mTextureFileNames = textureFileNames;
	mBIsTexture = bIsTexture;
}

bool Engine3D::OnUserCreate () {
	for (auto &file : mObjectFileNames) {
		world.push_back (new Object (ScreenWidth (), ScreenHeight (), file, mTextureFileNames, mBIsTexture));
	}
	// Index to determine which sprites to load

	pDepthBuffer = new float[ScreenWidth () * ScreenHeight ()];

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

	// Draw Triangles
	int iterator = 1;
	for (auto &object : world) {
		Mat4x4 *matWorld = new Mat4x4 (Mat4x4::scale (iterator, worldMat));
		matWorld = new Mat4x4(Mat4x4::translate (iterator-1, iterator-1, 5));
		object->CollectTris (*matWorld, viewMatrix, projMat, vCamera);
		iterator++;
	}

	// Sort triangles from back to front
	/*std::sort (trianglesToRasterize.begin (), trianglesToRasterize.end (), [](Triangle &t1, Triangle &t2) {
		float z1 = (t1.getP (0).getZ () + t1.getP (1).getZ () + t1.getP (2).getZ ()) / 3.0f;
		float z2 = (t2.getP (0).getZ () + t2.getP (1).getZ () + t2.getP (2).getZ ()) / 3.0f;
		return z1 > z2;
		});*/

	FillRect (0, 0, ScreenWidth (), ScreenHeight (), olc::BLACK);

	for (int i = 0; i < ScreenWidth () * ScreenHeight (); i++) {
		pDepthBuffer[i] = 0.0f;
	}

	for (auto &object : world) {
		for (auto &tri : object->getTrisToRasterize ()) {
			object->CheckViewClip (tri);

			// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles

			// FillTriangle (t.getP (0).getX (), t.getP (0).getY (), t.getP (1).getX (), t.getP (1).getY (), t.getP (2).getX (), t.getP (2).getY (), t.getColor ());
			DrawTexturedTriangle (object);
			// Wire Frame for Debugging
			// DrawTriangle (t.getP (0).getX (), t.getP (0).getY (), t.getP (1).getX (), t.getP (1).getY (), t.getP (2).getX (), t.getP (2).getY (), olc::BLACK);
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

void Engine3D::DrawTexturedTriangle (Object *object) {
	for (auto &t : object->getLTris ()) {
		DrawTexturedTriangle (t.getP (0).getX (), t.getP (0).getY (), t.getT (0).getU (), t.getT (0).getV (), t.getT (0).getW (),
			t.getP (1).getX (), t.getP (1).getY (), t.getT (1).getU (), t.getT (1).getV (), t.getT (1).getW (),
			t.getP (2).getX (), t.getP (2).getY (), t.getT (2).getU (), t.getT (2).getV (), t.getT (2).getW (), object->getDecal (0));
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

float *Engine3D::getPDepth () {
	return pDepthBuffer;
}