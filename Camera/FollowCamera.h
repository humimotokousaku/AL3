#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"

class FollowCamera {
public:


	void Initialize();

	void Update();

	void SetTarget(const WorldTransform* target);

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	float GetDstinationAngleY() { return destinationAngleY_; }

	// 追従対象からのオフセットを計算
	Vector3 TargetOffset() const;

	void Reset();

private:
	ViewProjection viewProjection_;
	const WorldTransform* target_ = nullptr;
	// ｔ移住対象の残像座標
	Vector3 interTarget_ = {};
	float destinationAngleY_ = 0.0f;
};