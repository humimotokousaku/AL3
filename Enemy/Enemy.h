#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Enemy/EnemyBullet.h"
#include "TimedCall.h"
#include <functional>

class Enemy; // 前方宣言

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

	void Initialize(Enemy* enemy);

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
	void Initialize(Enemy* enemy);
	void Update(Enemy* enemy);
};

class Enemy {
public:
	// メンバ関数

	// Getter
	Vector3 GetEnemyPos() { return this->worldTransform_.translation_; }

	Enemy();
	~Enemy();

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
	BaseEnemyState* state_;

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t enemyTexture_ = 0u;

	// 弾
	std::list<EnemyBullet*> bullets_;
};