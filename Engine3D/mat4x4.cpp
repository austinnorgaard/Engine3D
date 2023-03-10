#include "mat4x4.h"

Mat4x4::Mat4x4 () {
	size = 4;
	for (int i = 0; i < 4; i++) {
		for (int k = 0; k < 4; k++) {
			mat[i][k] = 0;
		}
	}
}

Mat4x4 Mat4x4::identity () {
	Mat4x4 m;
	m.mat[0][0] = 1.0f;
	m.mat[1][1] = 1.0f;
	m.mat[2][2] = 1.0f;
	m.mat[3][3] = 1.0f;
	return m;
}

Mat4x4 Mat4x4::rotateX (float fAngleRad) {
	Mat4x4 m;
	m.mat[0][0] = 1.0f;
	m.mat[1][1] = cosf (fAngleRad);
	m.mat[1][2] = sinf (fAngleRad);
	m.mat[2][1] = -1.0f * sinf (fAngleRad);
	m.mat[2][2] = cosf (fAngleRad);
	m.mat[3][3] = 1.0f;
	return m;
}

Mat4x4 Mat4x4::rotateY (float fAngleRad) {
	Mat4x4 m;
	m.mat[0][0] = cosf (fAngleRad);
	m.mat[0][2] = sinf (fAngleRad);
	m.mat[2][0] = -1.0f * sinf (fAngleRad);
	m.mat[1][1] = 1.0f;
	m.mat[2][2] = cosf (fAngleRad);
	m.mat[3][3] = 1.0f;
	return m;
}

Mat4x4 Mat4x4::rotateZ (float fAngleRad) {
	Mat4x4 m;
	m.mat[0][0] = cosf (fAngleRad);
	m.mat[0][1] = sinf (fAngleRad);
	m.mat[1][0] = -1.0f * sinf (fAngleRad);
	m.mat[1][1] = cosf (fAngleRad);
	m.mat[2][2] = 1.0f;
	m.mat[3][3] = 1.0f;
	return m;
}

Mat4x4 Mat4x4::translate (float x, float y, float z) {
	Mat4x4 m;
	m.mat[0][0] = 1.0f;
	m.mat[1][1] = 1.0f;
	m.mat[2][2] = 1.0f;
	m.mat[3][3] = 1.0f;
	m.mat[3][0] = x;
	m.mat[3][1] = y;
	m.mat[3][2] = z;
	return m;
}

Mat4x4 Mat4x4::project (float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
	float fFovRad = 1.0f / tanf (fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat[0][0] = fAspectRatio * fFovRad;
	mat[1][1] = fFovRad;
	mat[2][2] = fFar / (fFar - fNear);
	mat[3][2] = (( - 1.0f * fFar) * fNear) / (fFar - fNear);
	mat[2][3] = 1.0f;
	mat[3][3] = 0.0f;
	return *this;
}

Mat4x4 operator* (Mat4x4 &mat1, Mat4x4 &mat2) {
	Mat4x4 cMatrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			cMatrix.setMat (r, c, mat1.getMat (r, 0) * mat2.getMat (0, c) + mat1.getMat (r, 1) *
				mat2.getMat (1, c) + mat1.getMat (r, 2) * mat2.getMat (2, c) + mat1.getMat (r, 3) * mat2.getMat (3, c));
	return cMatrix;
}

float Mat4x4::getMat (int index1, int index2) const {
	return mat[index1][index2];
}

void Mat4x4::setMat (int index1, int index2, float newValue) {
	mat[index1][index2] = newValue;
}

Mat4x4 Mat4x4::inverse (Mat4x4 &matrix2) {
	Mat4x4 matrix;
	matrix.mat[0][0] = matrix2.mat[0][0]; 
	matrix.mat[0][1] = matrix2.mat[1][0];
	matrix.mat[0][2] = matrix2.mat[2][0]; 
	matrix.mat[0][3] = 0.0f;
	matrix.mat[1][0] = matrix2.mat[0][1]; 
	matrix.mat[1][1] = matrix2.mat[1][1]; 
	matrix.mat[1][2] = matrix2.mat[2][1]; 
	matrix.mat[1][3] = 0.0f;
	matrix.mat[2][0] = matrix2.mat[0][2]; 
	matrix.mat[2][1] = matrix2.mat[1][2];
	matrix.mat[2][2] = matrix2.mat[2][2]; 
	matrix.mat[2][3] = 0.0f;
	matrix.mat[3][0] = -1.0f * (matrix2.mat[3][0] * matrix.mat[0][0] + matrix2.mat[3][1] * matrix.mat[1][0] + matrix2.mat[3][2] * matrix.mat[2][0]);
	matrix.mat[3][1] = -1.0f * (matrix2.mat[3][0] * matrix.mat[0][1] + matrix2.mat[3][1] * matrix.mat[1][1] + matrix2.mat[3][2] * matrix.mat[2][1]);
	matrix.mat[3][2] = -1.0f * (matrix2.mat[3][0] * matrix.mat[0][2] + matrix2.mat[3][1] * matrix.mat[1][2] + matrix2.mat[3][2] * matrix.mat[2][2]);
	matrix.mat[3][3] = 1.0f;
	return matrix;
}

int Mat4x4::getSize () const {
	return size;
}