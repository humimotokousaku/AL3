﻿#pragma once
#include "Player/PlayerBullet.h"
#include "Collision/Collider.h"
#include "WorldTransform.h"
#include "Vector4.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include <list>

class GameScene;

class Player : public Collider {
public:
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;

	Vector3 GetWorld3DReticlePosition();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	// playerの回転
	void Rotate();

	/// <summary>
	/// 3Dレティクルの配置
	/// </summary>
	void Deploy3DReticle();

	/// <summary>
	/// 2Dレティクルの配置
	/// </summary>
	void Deploy2DReticle(const ViewProjection& viewProjection);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();
	
	Player();
	~Player();
	
	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(
	    Model* model, uint32_t textureHandle, const Vector3& pos);

	/// <summary>
	/// 更新
	/// <summary>
	void Update(const ViewProjection& viewProjection);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

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
	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t playerTexture_ = 0u;
	// レティクルハンドル
	uint32_t reticleTexture_ = 0u;
	// 2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;

	Matrix4x4 matViewport_;

	GameScene* gameScene_;

	bool isDead_ = true;

	Vector2 spritePosition_;
};