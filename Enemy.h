#pragma once
#include "WorldTransform.h"
#include "Model.h"

class Enemy {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

public:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t enemyTexture_ = 0u;

};
