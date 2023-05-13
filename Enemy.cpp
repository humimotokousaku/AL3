#include "Enemy.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include <cassert>

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

void Enemy::Update() {
	// 移動速度
	const float kMoveSpeed = -0.25f;
	// 移動処理
	worldTransform_.translation_.z += kMoveSpeed;

	// 行列の更新
	worldTransform_.UpdateMatrix();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// enemy
	model_->Draw(worldTransform_, viewProjection, enemyTexture_);
}