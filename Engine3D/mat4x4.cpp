#include "mat4x4.h"

Mat4x4::Mat4x4 () {
	for (int i = 0; i < 4; i++) {
		for (int k = 0; k < 4; k++) {
			mat[i][k] = 0;
		}
	}
}

Mat4x4 Mat4x4::identity () {
	Mat4x4 mat;
	mat.mat[0][0] = 1.0f;
	mat.mat[1][1] = 1.0f;
	mat.mat[2][2] = 1.0f;
	mat.mat[3][3] = 1.0f;
	return mat;
}

Mat4x4 Mat4x4::rotateX (float fAngleRad) {
	Mat4x4 mat;
	mat.mat[0][0] = 1.0f;
	mat.mat[1][1] = cosf (fAngleRad);
	mat.mat[1][2] = sinf (fAngleRad);
	mat.mat[2][1] = -sinf (fAngleRad);
	mat.mat[2][2] = cosf (fAngleRad);
	mat.mat[3][3] = 1.0f;
	return mat;
}

Mat4x4 Mat4x4::rotateY (float fAngleRad) {
	Mat4x4 mat;
	mat.mat[0][0] = cosf (fAngleRad);
	mat.mat[0][2] = sinf (fAngleRad);
	mat.mat[2][0] = -sinf (fAngleRad);
	mat.mat[1][1] = 1.0f;
	mat.mat[2][2] = cosf (fAngleRad);
	mat.mat[3][3] = 1.0f;
	return mat;
}

Mat4x4 Mat4x4::rotateZ (float fAngleRad) {
	Mat4x4 mat;
	mat.mat[0][0] = cosf (fAngleRad);
	mat.mat[0][1] = sinf (fAngleRad);
	mat.mat[1][0] = -sinf (fAngleRad);
	mat.mat[1][1] = cosf (fAngleRad);
	mat.mat[2][2] = 1.0f;
	mat.mat[3][3] = 1.0f;
	return mat;
}

Mat4x4 Mat4x4::translate (float x, float y, float z) {
	Mat4x4 mat;
	mat.mat[0][0] = 1.0f;
	mat.mat[1][1] = 1.0f;
	mat.mat[2][2] = 1.0f;
	mat.mat[3][3] = 1.0f;
	mat.mat[3][0] = x;
	mat.mat[3][1] = y;
	mat.mat[3][2] = z;
	return mat;
}

Mat4x4 Mat4x4::project (float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
	float fFovRad = 1.0f / tanf (fFovDegrees * 0.5f / 180.0f * 3.14159f);
	Mat4x4 mat;
	mat.mat[0][0] = fAspectRatio * fFovRad;
	mat.mat[1][1] = fFovRad;
	mat.mat[2][2] = fFar / (fFar - fNear);
	mat.mat[3][2] = (-fFar * fNear) / (fFar - fNear);
	mat.mat[2][3] = 1.0f;
	mat.mat[3][3] = 0.0f;
	return mat;
}

Mat4x4 Mat4x4::operator* (Mat4x4 &mat) {
	Mat4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.mat[r][c] = this->mat[r][0] * mat.mat[0][c] + this->mat[r][1] *
			mat.mat[1][c] + this->mat[r][2] * mat.mat[2][c] + this->mat[r][3] * mat.mat[3][c];
	return matrix;
}

float Mat4x4::getMat (int index1, int index2) {
	return mat[index1][index2];
}

void Mat4x4::setMat (int index1, int index2, float newValue) {
	mat[index1][index2] = newValue;
}

Mat4x4 Mat4x4::inverse () {
	Mat4x4 matrix;
	matrix.mat[0][0] = this->mat[0][0]; matrix.mat[0][1] = this->mat[1][0]; matrix.mat[0][2] = this->mat[2][0]; matrix.mat[0][3] = 0.0f;
	matrix.mat[1][0] = this->mat[0][1]; matrix.mat[1][1] = this->mat[1][1]; matrix.mat[1][2] = this->mat[2][1]; matrix.mat[1][3] = 0.0f;
	matrix.mat[2][0] = this->mat[0][2]; matrix.mat[2][1] = this->mat[1][2]; matrix.mat[2][2] = this->mat[2][2]; matrix.mat[2][3] = 0.0f;
	matrix.mat[3][0] = -(this->mat[3][0] * matrix.mat[0][0] + this->mat[3][1] * matrix.mat[1][0] + this->mat[3][2] * matrix.mat[2][0]);
	matrix.mat[3][1] = -(this->mat[3][0] * matrix.mat[0][1] + this->mat[3][1] * matrix.mat[1][1] + this->mat[3][2] * matrix.mat[2][1]);
	matrix.mat[3][2] = -(this->mat[3][0] * matrix.mat[0][2] + this->mat[3][1] * matrix.mat[1][2] + this->mat[3][2] * matrix.mat[2][2]);
	matrix.mat[3][3] = 1.0f;
	return matrix;
}