#pragma once
#include "math/Vector3.h"
#include "math/MyMatrix.h"

// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// 球面線形補間
Vector3 Slerp(Vector3 start, Vector3 end, float t);