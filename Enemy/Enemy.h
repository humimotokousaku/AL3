#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "IEnemyState.h"

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
	void ChangeState(IEnemyState* pState);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

private:
	
	IEnemyState* state_;

	// ワールド変換データ
	WorldTransform worldTransform_;
	
	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t enemyTexture_ = 0u;
};

