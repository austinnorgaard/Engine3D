#pragma once

#pragma warning (disable: 4996)

#include <algorithm>

class Mat4x4 {
public:
	Mat4x4 ();

	static Mat4x4 identity ();
	static Mat4x4 rotateX (float fAngleRad);
	static Mat4x4 rotateY (float fAngleRad);
	static Mat4x4 rotateZ (float fAngleRad);
	static Mat4x4 translate (float x, float y, float z);
	static Mat4x4 scale (float k, Mat4x4 &matrix);
	Mat4x4 project (float fFovDegrees, float fAspectRatio, float fNear, float fFar);
	static Mat4x4 inverse (Mat4x4 &matrix2);
	extern friend Mat4x4 operator* (Mat4x4 &mat1, Mat4x4 &mat2);

	float getMat (int index1, int index2) const;
	void setMat (int index1, int index2, float newValue);

	int getSize ();

private:
	float mat[4][4];
	int size;
};