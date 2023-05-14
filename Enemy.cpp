#include "Enemy.h"
#include "MyMath.h"
#include "WorldTransform.h"
#include <cassert>
#include <stdio.h>
#include "ImGuiManager.h"

void Enemy::Initialize(Model* model, const Vector3& pos) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	enemyTexture_ = TextureManager::Load("black.png");

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;
}

void Enemy::MoveApproach() {
	const Vector3 kMoveSpeed = {0, 0, -0.25f};
	// 移動処理
	worldTransform_.translation_ = Add(worldTransform_.translation_, kMoveSpeed);

	// 既定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::MoveLeave() {
	// 移動速度
	const Vector3 kMoveSpeed = {-0.25f, 0.25f, -0.25f};

	// 移動処理
	worldTransform_.translation_ = Add(worldTransform_.translation_, kMoveSpeed);
}

// フェーズの関数テーブル
void (Enemy::*Enemy::spFuncTable[])() = {&Enemy::MoveApproach, &Enemy::MoveLeave};

void Enemy::Update() {
	// メンバ関数ポインタの呼び出し
	if (static_cast<size_t>(phase_ == Phase::Approach)) {
		(this->*spFuncTable[0])();
	} 
	else if (static_cast<size_t>(phase_ == Phase::Leave)) {
		(this->*spFuncTable[1])();
	}

	// 行列の更新
	worldTransform_.UpdateMatrix();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

}

void Enemy::Draw(ViewProjection& viewProjection) {
	// enemy
	model_->Draw(worldTransform_, viewProjection, enemyTexture_);
}