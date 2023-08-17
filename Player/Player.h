#pragma once
#include "Collision/Collider.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"

class GameScene;

class Player  {
public:
	// 衝突を検出したら呼び出されるコールバック関数
	//void OnCollision() override;

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition();

	Player();
	~Player();
	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(
	    Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm, Model* modelGun, Model* modelBullet);

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection& viewProjection);

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();

	// 浮遊ギミック更新
	void UpdateFloatingGimmick();

	void Deploy3DReticle();

	void Deploy2DReticle(const ViewProjection& viewProjection);

	void Attack();

	Vector3 GetWorld3DReticlePosition();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	const WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	const WorldTransform& GetWorldTransformBase() { return worldTransformBase_; }

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	// パーツの親子関係
	void SetParent(const WorldTransform* parent);

private:
	// ワールド変換データ
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformGun_;
	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	// モデル
	Model* modelBody_;
	Model* modelHead_;
	Model* modelL_arm_;
	Model* modelR_arm_;
	Model* modelGun_;
	Model* modelBullet_;

	// 浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;

		// レティクルハンドル
	uint32_t reticleTexture_ = 0u;
	// 2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;

	Matrix4x4 matViewport_;

	GameScene* gameScene_;

	bool isDead_ = true;

	Vector2 spritePosition_;

	bool isBullet_;

	Input* input_;
};