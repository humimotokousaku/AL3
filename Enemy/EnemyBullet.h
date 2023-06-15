#pragma once
#include "Model.h"
#include "WorldTransform.h"

class EnemyBullet
{
public: // メンバ関数
	/// <summary>
	/// 形状を設定
	/// </summary>
	void SettingScale();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="pos">初期座標</param>
	void Initialize(Model* model, const Vector3& pos, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	bool isDead() const { return isDead_; }

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

public: // メンバ変数
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t bulletTexture_ = 0u;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// 死亡タイマー
	int32_t deathTimer_ = kLifeTime;

	bool isDead_ = false;

	// 速度
	Vector3 velocity_;
};
