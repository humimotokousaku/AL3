#pragma once
#include "Collision/Collider.h"
#include "Model.h"
#include "Player/Player.h"
#include "WorldTransform.h"

class EnemyBullet : public Collider {
public: // メンバ関数
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;

	void SetPlayer(Player* player) { player_ = player; }

	// 完了ならtrueを返す
	bool isDead() const { return isDead_; }

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

	// プレイヤーの情報
	Player* player_ = nullptr;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// 死亡タイマー
	int32_t deathTimer_ = kLifeTime;
	// 弾の有無
	bool isDead_ = false;

	// 速度
	Vector3 velocity_;
};
