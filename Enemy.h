#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "EnemyBullet.h"

class Enemy {
	// 行動フェーズ
	enum class Phase {
		Approach, // 接近フェーズ
		Leave,    // 離脱フェーズ
	};

public:
	Enemy();
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos);

	/// <summary>
	/// 接近フェーズの初期化
	/// </summary>
	void ApproachInitialize();

	/// <summary>
	/// 接近フェーズの移動処理
	/// </summary>
	void MoveApproach();

	/// <summary>
	/// 離脱フェーズの移動処理
	/// </summary>
	void MoveLeave();

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

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

	// 弾
	std::list<EnemyBullet*> bullets_;
	// 発射間隔
	static const int kFireInterval = 60;
	// 発射タイマー
	int32_t fireTimer_ = 0;
};
