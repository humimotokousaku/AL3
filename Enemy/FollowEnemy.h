#pragma once
#include "../Utility/TimedCall.h"
#include "Collision/Collider.h"
#include "Model.h"
#include "Player/Player.h"
#include "WorldTransform.h"
#include <functional>
#include <random>

class GameScene;

class FollowEnemy : public Collider {
public: // メンバ関数
	// 衝突してない場合
	bool NonCollision() override;
	// 衝突を検出したら呼び出されるコールバック関数
	bool OnCollision() override;

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;
	// 座標取得
	Vector3 GetEnemyPos() { return this->worldTransform_.translation_; }

	// Setter
	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// 完了ならtrueを返す
	bool isDead() const { return isDead_; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	/// <summary>
	/// stateの変更
	/// </summary>
	/// <param name="pState">state</param>
	// void ChangeState(BaseEnemyState* pState);

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="velocity">移動量</param>
	void Move(const Vector3 velocity);

	/// <summary>
	/// 発射処理
	/// </summary>
	void Fire();

	FollowEnemy();
	~FollowEnemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

private:
	// 状態遷移
	// BaseEnemyState* state_;
	Audio* audio_ = nullptr;

	// ワールド変換データ
	WorldTransform worldTransform_;
	WorldTransform hitWorldTransform_;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t followEnemyTexture_ = 0u;
	// 
	uint32_t normalTexture_ = 0u;
	// 攻撃を食らった時の画像
	uint32_t hitTexture_ = 0u; 

	uint32_t damageSound_;

	// 自キャラ
	Player* player_ = nullptr;

	bool isHitReaction_;
	float hitFrame_;

	bool isDead_ = false;
	bool isDecrementHp_;
	int hp_;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	// 速度
	Vector3 velocity_;
};
