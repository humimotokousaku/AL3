#pragma once
#include "Enemy/EnemyBullet.h"
#include "Model.h"
#include "TimedCall.h"
#include "WorldTransform.h"
#include <functional>

class Enemy; // 前方宣言

class Player;
// 基底クラス
class BaseEnemyState {
public:
	// 純粋仮想関数
	virtual void Initialize(Enemy* enemy) = 0;
	virtual void Update(Enemy* enemy) = 0;
};

// 接近フェーズのクラス
class EnemyStateApproach : public BaseEnemyState {
public:
	~EnemyStateApproach();

	/// <summary>
	/// 弾を発射してタイマーをリセット
	/// </summary>
	void FireAndResetTimer();

	// 初期化
	void Initialize(Enemy* enemy);

	// 更新処理
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
	// 発射間隔
	static const int kFireInterval = 60;
	// 時限発動
	std::list<TimedCall*> timedCalls_;
};

// 離脱フェーズのクラス
class EnemyStateLeave : public BaseEnemyState {
public:
	// 初期化
	void Initialize(Enemy* enemy);

	// 更新処理
	void Update(Enemy* enemy);

public:
	Enemy* enemy_;
};

class Enemy {
public:
	// メンバ関数

	// Getter
	Vector3 GetEnemyPos() { return this->worldTransform_.translation_; }
	Vector3 GetWorldPosition();

	Enemy();
	~Enemy();

	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos);

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="velocity">移動量</param>
	void Move(const Vector3 velocity);

	/// <summary>
	/// 発射処理
	/// </summary>
	void Fire();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// stateの変更
	/// </summary>
	/// <param name="pState">state</param>
	void ChangeState(BaseEnemyState* pState);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

private:
	// 状態遷移
	BaseEnemyState* state_;

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t enemyTexture_ = 0u;

	// 弾
	std::list<EnemyBullet*> bullets_;

	// 自キャラ
	Player* player_ = nullptr;
};