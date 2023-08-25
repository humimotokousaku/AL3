#include "math/Lerp.h"
#define _USE_MATH_DEFINES
#include <cmath> 

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