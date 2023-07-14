#include "Enemy.h"
#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"

#include "math/MyMatrix.h"
#include "WorldTransform.h"
#include <cassert>
#include <stdio.h>

Enemy::Enemy() { 
	state_ = new EnemyStateApproach();
}

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

void Enemy::Move(const Vector3 velocity) {
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity);
}

void Enemy::Update() { 
	// 行列の更新
	worldTransform_.UpdateMatrix();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// 状態遷移
	state_->Update(this); 
}

void Enemy::ChangeState(IEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// enemy
	model_->Draw(worldTransform_, viewProjection, enemyTexture_);
}

//void EnemyStateApproach::Update(Enemy* enemy) { 
//	// 移動速度
//	const Vector3 kMoveSpeed = {0, 0, -0.25f};
//
//	// 移動処理
//	enemy->Move(kMoveSpeed);
//
//	// 既定の位置に到達したら離脱
//	if (enemy->GetEnemyPos().z < 0.0f) {
//		enemy->ChangeState(new EnemyStateLeave());
//	}
//}

//void EnemyStateLeave::Update(Enemy* enemy) { 
//	// 移動速度
//	const Vector3 kMoveSpeed = {-0.25f, 0.25f, -0.25f};
//
//	// 移動処理
//	enemy->Move(kMoveSpeed);
//}
