#include "math/MyMatrix.h"
#define _USE_MATH_DEFINES
#include "ImGuiManager.h"
#include <math.h>

#pragma region Matrix4x4

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] = m1.m[row][column] + m2.m[row][column];
		}
	}
	return result;
}

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] = m1.m[row][column] - m2.m[row][column];
		}
	}
	return result;
}

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result{};
	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = -(std::sin(radian));
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result{};
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = 0;
	result.m[0][2] = -(std::sin(radian));
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = std::sin(radian);
	result.m[2][1] = 0;
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result{};
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = -(std::sin(radian));
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

Matrix4x4 MakeRotateMatrix(const Vector3& radian) {
	Matrix4x4 rotateX{};
	Matrix4x4 rotateY{};
	Matrix4x4 rotateZ{};
	rotateX = MakeRotateXMatrix(radian.x);
	rotateY = MakeRotateYMatrix(radian.y);
	rotateZ = MakeRotateZMatrix(radian.z);

	Matrix4x4 result{};
	result = Multiply(rotateX, Multiply(rotateY, rotateZ));

	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;
	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result{};
	result.m[0][0] = scale.x;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};
	float determinant = (m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]) +
	                    (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]) +
	                    (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2])

	                    - (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]) -
	                    (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]) -
	                    (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])

	                    - (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]) -
	                    (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]) -
	                    (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])

	                    + (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]) +
	                    (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]) +
	                    (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2])

	                    + (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]) +
	                    (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]) +
	                    (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2])

	                    - (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]) -
	                    (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]) -
	                    (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])

	                    - (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]) -
	                    (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]) -
	                    (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])

	                    + (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]) +
	                    (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]) +
	                    (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

	result.m[0][0] = (1.0f / determinant) *
	                 (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
	                  m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
	                  m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
	result.m[0][1] = (1.0f / determinant) *
	                 (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
	                  m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
	                  m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	result.m[0][2] = (1.0f / determinant) *
	                 (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
	                  m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
	                  m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	result.m[0][3] = (1.0f / determinant) *
	                 (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
	                  m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
	                  m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	result.m[1][0] = (1.0f / determinant) *
	                 (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
	                  m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
	                  m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
	//
	result.m[1][1] = (1.0f / determinant) *
	                 (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
	                  m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
	                  m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
	//
	result.m[1][2] = (1.0f / determinant) *
	                 (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
	                  m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
	                  m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
	//
	result.m[1][3] = (1.0f / determinant) *
	                 (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
	                  m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
	                  m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);
	//
	result.m[2][0] = (1.0f / determinant) *
	                 (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
	                  m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
	                  m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
	//
	result.m[2][1] = (1.0f / determinant) *
	                 (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
	                  m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
	                  m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);

	result.m[2][2] = (1.0f / determinant) *
	                 (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
	                  m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
	                  m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	result.m[2][3] = (1.0f / determinant) *
	                 (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
	                  m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
	                  m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	result.m[3][0] = (1.0f / determinant) *
	                 (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
	                  m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
	                  m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
	result.m[3][1] = (1.0f / determinant) *
	                 (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
	                  m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
	                  m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
	result.m[3][2] = (1.0f / determinant) *
	                 (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
	                  m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
	                  m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
	result.m[3][3] = (1.0f / determinant) *
	                 (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
	                  m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
	                  m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return result;
}

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] +
	           1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] +
	           1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] +
	           1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] +
	          1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] +
	                 m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] +
	                 m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] +
	                 m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] +
	                 m1.m[0][3] * m2.m[3][3];

	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] +
	                 m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] +
	                 m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] +
	                 m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] +
	                 m1.m[1][3] * m2.m[3][3];

	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] +
	                 m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] +
	                 m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] +
	                 m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] +
	                 m1.m[2][3] * m2.m[3][3];

	result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] +
	                 m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] +
	                 m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] +
	                 m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] +
	                 m1.m[3][3] * m2.m[3][3];

	return result;
}

// ベクトルと行列の積
Vector3 Multiply(Vector3 v, Matrix4x4 m) {
	Vector3 result{};
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1];
	result.y = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2];
	return result;
}

Vector3 Multiply(const float& scalar, const Vector3& v) {
	Vector3 result;
	result.x = v.x * scalar;
	result.y = v.y * scalar;
	result.z = v.z * scalar;
	return result;
}

Matrix4x4 Multiply(const float& scalar, const Matrix4x4& v) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = v.m[i][j] * scalar;
		}
	}

	return result;
}

float Determinant(Matrix4x4 matrix) {
	// 行列式
	return (matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3]) +
	       (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1]) +
	       (matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2]) -

	       (matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1]) -
	       (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3]) -
	       (matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2]) -

	       (matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3]) -
	       (matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1]) -
	       (matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2]) +

	       (matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1]) +
	       (matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3]) +
	       (matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2]) +

	       (matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3]) +
	       (matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1]) +
	       (matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2]) -

	       (matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1]) -
	       (matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3]) -
	       (matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2]) -

	       (matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0]) -
	       (matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0]) -
	       (matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0]) +

	       (matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0]) +
	       (matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0]) +
	       (matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0]);
}

Matrix4x4 InverseT(Matrix4x4 matrix) {
	Matrix4x4 result{};

	result.m[0][0] = (matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3]) +
	                 (matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1]) +
	                 (matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2]) -
	                 (matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1]) -
	                 (matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3]) -
	                 (matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2]);
	result.m[0][1] = -(matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][3]) -
	                 (matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][1]) -
	                 (matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][2]) +
	                 (matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][1]) +
	                 (matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][3]) +
	                 (matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][2]);
	result.m[0][2] = (matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][3]) +
	                 (matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][1]) +
	                 (matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][2]) -
	                 (matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][1]) -
	                 (matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][3]) -
	                 (matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][2]);
	result.m[0][3] = -(matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][3]) -
	                 (matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][1]) -
	                 (matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][2]) +
	                 (matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][1]) +
	                 (matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][3]) +
	                 (matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][2]);

	result.m[1][0] = -(matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3]) -
	                 (matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0]) -
	                 (matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2]) +
	                 (matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0]) +
	                 (matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3]) +
	                 (matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2]);
	result.m[1][1] = (matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][3]) +
	                 (matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][0]) +
	                 (matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][2]) -
	                 (matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][0]) -
	                 (matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][3]) -
	                 (matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][2]);
	result.m[1][2] = -(matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][3]) -
	                 (matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][0]) -
	                 (matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][2]) +
	                 (matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][0]) +
	                 (matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][3]) +
	                 (matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][2]);
	result.m[1][3] = (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][3]) +
	                 (matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][0]) +
	                 (matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][2]) -
	                 (matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][0]) -
	                 (matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][3]) -
	                 (matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][2]);

	result.m[2][0] = (matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3]) +
	                 (matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0]) +
	                 (matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1]) -
	                 (matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0]) -
	                 (matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3]) -
	                 (matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1]);
	result.m[2][1] = -(matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][3]) -
	                 (matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][0]) -
	                 (matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][1]) +
	                 (matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][0]) +
	                 (matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][3]) +
	                 (matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][1]);
	result.m[2][2] = (matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][3]) +
	                 (matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][0]) +
	                 (matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][1]) -
	                 (matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][0]) -
	                 (matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][3]) -
	                 (matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][1]);
	result.m[2][3] = -(matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][3]) -
	                 (matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][0]) -
	                 (matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][1]) +
	                 (matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][0]) +
	                 (matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][3]) +
	                 (matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][1]);

	result.m[3][0] = -(matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2]) -
	                 (matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0]) -
	                 (matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1]) +
	                 (matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0]) +
	                 (matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2]) +
	                 (matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1]);
	result.m[3][1] = (matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][2]) +
	                 (matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][0]) +
	                 (matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][1]) -
	                 (matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][0]) -
	                 (matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][2]) -
	                 (matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][1]);
	result.m[3][2] = -(matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][2]) -
	                 (matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][0]) -
	                 (matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][1]) +
	                 (matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][0]) +
	                 (matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][2]) +
	                 (matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][1]);
	result.m[3][3] = (matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2]) +
	                 (matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0]) +
	                 (matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1]) -
	                 (matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0]) -
	                 (matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2]) -
	                 (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1]);

	float constant = 1.0f / Determinant(matrix);
	return Multiply(constant, result);
}

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	// 計算結果
	Matrix4x4 result{};

	// それぞれの回転行列
	Matrix4x4 rotateXMatrix{};
	Matrix4x4 rotateYMatrix{};
	Matrix4x4 rotateZMatrix{};

	// 三次元の回転行列の積
	Matrix4x4 rotateMatrix{};

	// それぞれの回転行列を計算
	rotateXMatrix = MakeRotateXMatrix(rotate.x);
	rotateYMatrix = MakeRotateYMatrix(rotate.y);
	rotateZMatrix = MakeRotateZMatrix(rotate.z);

	// すべての回転行列の積
	rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	// アフィン変換行列の計算
	result.m[0][0] = scale.x * rotateMatrix.m[0][0];
	result.m[0][1] = scale.x * rotateMatrix.m[0][1];
	result.m[0][2] = scale.x * rotateMatrix.m[0][2];
	result.m[0][3] = 0.0f;

	result.m[1][0] = scale.y * rotateMatrix.m[1][0];
	result.m[1][1] = scale.y * rotateMatrix.m[1][1];
	result.m[1][2] = scale.y * rotateMatrix.m[1][2];
	result.m[1][3] = 0.0f;

	result.m[2][0] = scale.z * rotateMatrix.m[2][0];
	result.m[2][1] = scale.z * rotateMatrix.m[2][1];
	result.m[2][2] = scale.z * rotateMatrix.m[2][2];
	result.m[2][3] = 0.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

#pragma endregion

/// <summary>
/// ベクトルの足し算
/// </summary>
/// <param name="pos">座標</param>
/// <param name="vector">座標に対して足すもの</param>
/// <returns></returns>
Vector3 Add(const Vector3& pos, const Vector3& vector) {
	Vector3 result{};
	result.x = pos.x + vector.x;
	result.y = pos.y + vector.y;
	result.z = pos.z + vector.z;

	return result;
}

Vector3 Subtract(const Vector3& pos1, const Vector3& pos2) {
	Vector3 result{};
	result.x = pos1.x - pos2.x;
	result.y = pos1.y - pos2.y;
	result.z = pos1.z - pos2.z;

	return result;
}

// スカラー倍
Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	float result{};
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

// 長さ(ノルム)
float Length(const Vector3& v) {
	float result{};
	result = sqrt(Dot(v, v));
	return result;
}

// 正規化
Vector3 Normalize(const Vector3& v) {
	Vector3 result{};
	float length = Length(v);
	if (length != 0.0f) {
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
	}
	return result;
}

// ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;

	result.m[0][0] = width / 2.0f;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = -(height / 2.0f);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0;

	result.m[3][0] = left + (width / 2.0f);
	result.m[3][1] = top + (height / 2.0f);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1;

	return result;
}

float Lerp(const float& a, const float& b, float t) {
	float result{};

	result = a + b * t;

	return result;
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 P;
	P.x = v1.x + t * (v2.x - v1.x);
	P.y = v1.y + t * (v2.y - v1.y);
	P.z = v1.z + t * (v2.z - v1.z);
	return P;
}

Vector3 Slerp(Vector3 start, Vector3 end, float t) {
	float dot = Dot(start, end);
	float theta = acos((dot * (float)M_PI) / 180);
	float sinTheta = sin(theta);
	float weightStart = sin((1 - t) * theta) / sinTheta;
	float weightEnd = sin(t * theta) / sinTheta;

	Vector3 result;
	result.x = (weightStart * start.x + weightEnd * end.x);
	result.y = (weightStart * start.y + weightEnd * end.y);
	result.z = (weightStart * start.z + weightEnd * end.z);
	return result;
}

float LerpShortAngle(float a, float b, float t) {
	// 角度差分を求める
	float diff = b - a;

	diff = std::fmod(diff, 2 * (float)M_PI);
	if (diff < 2 * (float)-M_PI) {
		diff += 2 * (float)M_PI;
	} else if (diff >= 2 * M_PI) {
		diff -= 2 * (float)M_PI;
	}

	diff = std::fmod(diff, 2 * (float)M_PI);
	if (diff < (float)-M_PI) {
		diff += 2 * (float)M_PI;
	} else if (diff >= (float)M_PI) {
		diff -= 2 * (float)M_PI;
	}

	return Lerp(a, diff, t);
}