#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"

#pragma region Matrix

// 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

// 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);


// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// 3次元アフィン変換行列
Matrix4x4
    MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

#pragma endregion

/// <summary>
/// 位置とベクトルの足し算
/// </summary>
/// <param name="pos">座標</param>
/// <param name="vector">ベクトル</param>
/// <returns></returns>
Vector3 Add(const Vector3& pos, const Vector3& vector);

Vector3 Subtract(const Vector3& pos1, const Vector3& pos2);

// ベクトルと行列の積
Vector3 Multiply(Vector3 v, Matrix4x4 m);

// スカラー倍
Vector3 Multiply(const Vector3& v1, const Vector3& v2);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);

// 長さ(ノルム)
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);

// ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);


// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);