#pragma once
#include "Model.h"
#include "WorldTransform.h"

class Enemy;	// 前方宣言

// 基底クラス
class BaseEnemyState {
public:
	// 純粋仮想関数
	virtual void Update(Enemy* enemy) = 0;

public:

};

// 接近フェーズのクラス
class EnemyStateApproach : public BaseEnemyState{
public:

	void Update(Enemy* enemy);
};

// 離脱フェーズのクラス
class EnemyStateLeave : public BaseEnemyState {
public:

	void Update(Enemy* enemy);
};

class Enemy{
public:
	// メンバ関数

	// Getter
	Vector3 GetEnemyPos() { return this->worldTransform_.translation_; }

	Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos);

	void Move(const Vector3 velocity);

	/// <summary>
	/// 更新
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
};

