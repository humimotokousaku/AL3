#pragma once
#include "Collision/Collider.h"
#include "WorldTransform.h"
#include "Model.h"
#include <stdint.h>
#include "Player/Player.h"

class Block : public Collider{
public:
	Block();
	~Block();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& pos, const Vector3& scale);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	// 衝突してない場合
	bool NonCollision() override;
	bool OnCollision() override;

	Vector3 GetWorldPosition() override;

	bool GetIsCollision() { return isCollision_; }

	void ResetPlayerPos(Player* player);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t blockTexture_ = 0u;

	bool isCollision_;
};
