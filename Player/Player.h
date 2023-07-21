#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Player {
public:
	Vector3 GetWorldPosition();

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	Player();
	~Player();
	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection& viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};