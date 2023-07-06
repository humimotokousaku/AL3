#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Vector3.h"

class RailCamera {
public:
	ViewProjection& GetViewProjection() { return viewProjection_; }

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WorldTransform worldTransform, const Vector3& radian);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
};