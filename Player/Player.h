#pragma once
#include "Input.h"
#include "Model.h"
#include "Vector4.h"
#include "WorldTransform.h"
#include "Player/PlayerBullet.h"
#include <list>

class Player {
public:
	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

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
	void OnCollision();

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