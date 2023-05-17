#include "Enemy.h"
#include "WorldTransform.h"
#include "MyMatrix.h"
#include <cassert>
#include <stdio.h>

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
	const Vector3 kMoveSpeed = {0,0,-0.25f};
	// 移動処理
	worldTransform_.translation_ = Add(worldTransform_.translation_, kMoveSpeed);

	// 行列の更新
	worldTransform_.UpdateMatrix();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

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

	// 行列の更新
	worldTransform_.UpdateMatrix();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Enemy::Update() {
	switch (phase_) {
	case Phase::Approach:
	default:
		MoveApproach();
		break;
	case Phase::Leave:
		MoveLeave();
		break;
	}
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// enemy
	model_->Draw(worldTransform_, viewProjection, enemyTexture_);
}