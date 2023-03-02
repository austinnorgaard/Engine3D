#pragma once

#pragma warning (disable: 4996)

#include <algorithm>

class Mat4x4 {
public:
	Mat4x4 ();

	Mat4x4 identity ();
	Mat4x4 rotateX (float fAngleRad);
	Mat4x4 rotateY (float fAngleRad);
	Mat4x4 rotateZ (float fAngleRad);
	Mat4x4 translate (float x, float y, float z);
	Mat4x4 project (float fFovDegrees, float fAspectRatio, float fNear, float fFar);
	Mat4x4 inverse ();

	Mat4x4 operator* (Mat4x4 &mat);

	float getMat (int index1, int index2);
	void setMat (int index1, int index2, float newValue);

private:
	float mat[4][4];
};