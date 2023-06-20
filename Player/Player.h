#pragma once
#include "Player/PlayerBullet.h"
#include "Collision/Collider.h"
#include "WorldTransform.h"
#include "Vector4.h"
#include "Input.h"
#include "Model.h"
#include <list>

class Player : public Collider {
public:
	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

public:
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model, uint32_t textureHandle);
	
	// playerの回転
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection& viewProjection);

public:
	// キーボード入力
	Input* input_ = nullptr;

	// ImGuiで値を入力する変数
	float* inputFloat3[3] = {
	    &worldTransform_.translation_.x, 
		&worldTransform_.translation_.y,
	    &worldTransform_.translation_.z
	};

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t playerTexture_ = 0u;

	// 弾
	std::list<PlayerBullet*> bullets_;
};