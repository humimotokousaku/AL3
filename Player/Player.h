#pragma once
#include "Input.h"
#include "Model.h"
#include "Vector4.h"
#include "WorldTransform.h"
#include "Player/PlayerBullet.h"
#include <list>

class Player {
public:
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	// playerの回転
	void Rotate();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model, uint32_t textureHandle);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

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