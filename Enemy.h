#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Enemy {
	// 行動フェーズ
	enum class Phase {
		Approach, // 接近フェーズ
		Leave,    // 離脱フェーズ
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos);

	/// <summary>
	/// 接近フェーズの移動処理
	/// </summary>
	void MoveApproach();

	/// <summary>
	/// 離脱フェーズの移動処理
	/// </summary>
	void MoveLeave();

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

	// フェーズ
	Phase phase_ = Phase::Approach;
};
