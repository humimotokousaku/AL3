#pragma once
#include "Vector3.h"

class Collider {
public:
	// Getter
	float GetRadius() { return radius_; }
	// Setter
	void SetRadius(float radius) { radius_ = radius; }

	// 衝突時に呼ばれる関数
	virtual void OnCollision() = 0;

	// ワールド座標を取得
	virtual Vector3 GetWorldPosition() = 0;

private:
	// 衝突半径
	float radius_ = 1.0f;
};
