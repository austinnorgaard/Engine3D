#include "Object.h"

Object::Object (int width, int height, std::string objectFileName, std::vector<std::string> textureFileNames, bool bIsTexture) {
	screenHeight = height;
	screenWidth = width;
	objFile.loadFromObjectFile (objectFileName, bIsTexture);
	for (auto &file : textureFileNames) {
		vSprites.push_back (new olc::Sprite (file));
	}
	for (auto &sprite : vSprites) {
		vDecals.push_back (new olc::Decal (sprite));
	}
}

void Object::TranslateTris (Mat4x4 &mat, Triangle &tri, Triangle &tri2) {
	for (int j = 0; j < tri.getSize (); j++) {
		tri.setP (j, (tri2.getP (j) * mat));
		tri.setT (j, tri2.getT (j));
	}	
}

void Object::ClipTris (Mat4x4 &projection, int layer) {
	// Clip Viewed Triangle against near plane, this could form two additional
				// additional triangles. 
	int nClippedTriangles = 0;
	Triangle clipped[2];
	nClippedTriangles += clipAgainstPlane ({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, *layerTris.at (layer), clipped[0], clipped[1]);
	Vec3D vOffsetView = {1, 1, 0};

	for (int n = 0; n < nClippedTriangles; n++) {
		// Project triangles from 3D --> 2D
		for (int i = 0; i < clipped[n].getSize (); i++) {
			layerTris.at (layer)->setP (i, clipped[n].getP (i) * (projection));
			layerTris.at (layer)->setT (i, (clipped[n].getT (i)));
			layerTris.at (layer)->setT (i, (Vec2D (layerTris.at (layer)->getT (i).getU () / layerTris.at (layer)->getP (i).getW (), layerTris.at (layer)->getT (i).getV () / layerTris.at (layer)->getP (i).getW (), 1.0f / layerTris.at (layer)->getP (i).getW ())));
			layerTris.at (layer)->setP (i, layerTris.at (layer)->getP (i) / layerTris.at (layer)->getP (i).getW ());
			layerTris.at (layer)->setP (i, Vec3D (layerTris.at (layer)->getP (i).getX () * -1.0f, layerTris.at (layer)->getP (i).getY () * -1.0f, layerTris.at (layer)->getP (i).getZ ()));
			layerTris.at (layer)->setP (i, layerTris.at (layer)->getP (i) + vOffsetView);
			layerTris.at (layer)->setP (i, Vec3D (layerTris.at (layer)->getP (i).getX () * (0.5f * (float) screenWidth), layerTris.at (layer)->getP (i).getY () * (0.5f * (float) screenHeight), layerTris.at (layer)->getP (i).getZ ()));
		}
	}
}

void Object::ScaleTris (Mat4x4 &mat, int scaleFactor, Triangle &tri, Triangle &tri2) {
	for (int j = 0; j < tri.getSize (); j++) {
		tri.setP (j, (tri2.getP (j) * Mat4x4::scale (scaleFactor, mat)));
		tri.setT (j, tri2.getT (j));
	}
}

void Object::CollectTris (Mat4x4 &world, Mat4x4 &view, Mat4x4 &projection, Vec3D &camera) {
	int iterator = 0;
	std::vector<Triangle> trisToRas;
	vTris = trisToRas;
	std::vector<Triangle *>layerOfTris;
	layerTris = layerOfTris;
	for (auto &mainTri : objFile.getTris ()) {
		Triangle triProjected, triTransformed, triViewed;

		TranslateTris (world, triTransformed, mainTri);
		layerTris.push_back (new Triangle(triTransformed));

		// Use Cross-Product to get surface normal
		Vec3D line1, line2, normal;

		// Get lines of either side of triangle
		line1 = layerTris.at (iterator)->getP (1) - layerTris.at (iterator)->getP (0);
		line2 = layerTris.at (iterator)->getP (2) - layerTris.at (iterator)->getP (0);

		// Take cross product of lines to get normal to triangle surface
		normal = Vec3D::crossProduct (line1, line2);

		// Normalize the normal
		normal = Vec3D::normalize (normal);

		// Get ray from triangle to camera
		Vec3D vCameraRay = layerTris.at (iterator)->getP (0) - camera;

		if (Vec3D::dotProduct (normal, vCameraRay) < 0.0f) {
			// Convert World Space --> View Space
			TranslateTris (view, *layerTris.at (iterator), *layerTris.at (iterator));

			ClipTris (projection, iterator);

			vTris.push_back (*layerTris.at (iterator));
		}
		iterator++;
	}
}

void Object::CheckViewClip (Triangle tri) {
	// Clip triangles against all four screen edges, this could yield
		// a bunch of triangles, so create a queue that we traverse to 
		//  ensure we only test new triangles generated against planes
	Triangle clipped[2];
	std::list<Triangle> listTriangle;
	listTriangles = listTriangle;

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
			case 1:	nTrisToAdd = clipAgainstPlane ({0.0f, (float) screenHeight - 1, 0.0f}, {0.0f, -1.0f, 0.0f}, test, clipped[0], clipped[1]);
				break;
			case 2:	nTrisToAdd = clipAgainstPlane ({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
				break;
			case 3:	nTrisToAdd = clipAgainstPlane ({(float) screenWidth - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
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
}

int Object::clipAgainstPlane (Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2) {
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
	Vec3D p = Vec3D ();
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

std::vector<Triangle> Object::getTrisToRasterize () {
	return vTris;
}

olc::Decal *Object::getDecal (int layer) {
	return vDecals.at (layer);
}

std::list<Triangle> Object::getLTris () {
	return listTriangles;
}

int Object::getLayers () {
	return vDecals.size ();
}