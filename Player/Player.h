#pragma once
#include "Collision/Collider.h"
#include "Audio.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include <optional>

class GameScene;

class Player : public Collider {
public:
	// 衝突してない場合
	bool NonCollision() override;
	// 衝突を検出したら呼び出されるコールバック関数
	bool OnCollision() override;

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;

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

	// 攻撃
	void Attack();
	// ステップ移動
	void StepMove();

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();
	// 浮遊ギミック更新
	void UpdateFloatingGimmick();

	// 遷移行動更新
	void BehaviorRootUpdate();
	// 通常行動初期化
	void BehaviorRootInitialize();
	// 攻撃行動初期化
	void BehaviorStepInitialize();
	// 攻撃行動更新
	void BehaviorStepUpdate();

	// 3Dレティクル
	void Deploy3DReticle();
	 // 2Dレティクル
	void Deploy2DReticle(const ViewProjection& viewProjection);

	const WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	const WorldTransform& GetWorldTransformBase() { return worldTransformBase_; }
	Vector3 GetWorld3DReticlePosition();

	// ワールド座標をセット
	void SetWorldPos(Vector3 prePos);
	// ビュー行列をセット
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	// ゲームシーンのアドレスをセット
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
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

	// 目標角度
	float goalAngle_;

	// 浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;

	// レティクルハンドル
	uint32_t reticleTexture_ = 0u;
	// 2Dレティクル用のスプライト
	Sprite* sprite2DReticle_ = nullptr;

	Matrix4x4 matViewport_;

	GameScene* gameScene_;

	Vector2 spritePosition_;

	Input* input_;
	Audio* audio_;
	uint32_t shotSound_ = 0;

	// 壁に当たった時に今の移動ベクトルと逆の方向のベクトルを入れる
	Vector3 velocity_;

	int stepFrame_;

	// 振るまい
	enum class Behavior {
		kRoot,  // 通常状態
		//kAttack, // 攻撃中
		kStep    // 回避
	};
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	struct WorkDash {
		// ダッシュ用の媒介変数
		uint32_t dashParameter_ = 0;
	};
	WorkDash workDash_;

	bool isDead_ = false;
	bool isDecrementHp_;
	int hp_;

	bool isHit_;
	int invincibleFrame_;

	bool isShot_;
	int shotCoolTime_;
};