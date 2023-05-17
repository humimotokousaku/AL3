#pragma once
#include "Model.h"
#include "WorldTransform.h"

//class Enemy {
	// 行動フェーズ
	enum Phase {
		Approach, // 接近フェーズ
		Leave,    // 離脱フェーズ
	};
//
//public:
//	/// <summary>
//	/// 初期化
//	/// </summary>
//	void Initialize(Model* model, const Vector3& pos);
//
//	/// <summary>
//	/// 接近フェーズの移動処理
//	/// </summary>
//	void MoveApproach();
//
//	/// <summary>
//	/// 離脱フェーズの移動処理
//	/// </summary>
//	void MoveLeave();
//
//	/// <summary>
//	/// 更新
//	/// </summary>
//	void Update();
//
//	/// <summary>
//	/// 描画
//	/// </summary>
//	void Draw(ViewProjection& viewProjection);
//
//public:
//	// ワールド変換データ
//	WorldTransform worldTransform_;
//
//	// モデル
//	Model* model_ = nullptr;
//	// テクスチャハンドル
//	uint32_t enemyTexture_ = 0u;
//
//	// フェーズ
//	Phase phase_ = Phase::Approach;
//};

class Enemy;	// 前方宣言

// 基底クラス
class BaseEnemyState 
{
protected:
	// 純粋仮想関数
	virtual void Update() = 0;

public:
	// メンバ変数
	Enemy* enemy_;
};

// 接近フェーズのクラス
class EnemyStateApproach : public BaseEnemyState
{
public:

	void Update();
};

// 離脱フェーズのクラス
class EnemyStateLeave : public BaseEnemyState {
public:

	void Update();
};

class Enemy
{
public:
	// メンバ関数

	// getter
	Vector3 GetEnemyPos(Vector3 pos) { return worldTransform_.translation_ = pos; }

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos);

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
	/// 接近フェーズの移動処理
	/// </summary>
	void MoveApproach();

	/// <summary>
	/// 離脱フェーズの移動処理
	/// </summary>
	void MoveLeave();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

public:
	BaseEnemyState* state_;
	// ワールド変換データ
	WorldTransform worldTransform_;
	
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t enemyTexture_ = 0u;
	
	// フェーズ
	int phase_ = Approach;
};

