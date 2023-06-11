#include "Enemy.h"
#include "MyMatrix.h"
#include "WorldTransform.h"
#include <cassert>
#include <stdio.h>

Enemy::Enemy() {}
Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
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

	ApproachInitialize();
}

void Enemy::ApproachInitialize() {
	// 発射タイマーの初期化
	fireTimer_ = kFireInterval;
}

void Enemy::MoveApproach() {
	const Vector3 kMoveSpeed = {0, 0, -0.10f};
	// 移動処理
	worldTransform_.translation_ = Add(worldTransform_.translation_, kMoveSpeed);

	// 行列の更新
	worldTransform_.UpdateMatrix();

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	#pragma region 弾の処理

	// 発射タイマーカウントダウン
	fireTimer_--;
	// 指定時間に達した
	if (fireTimer_ <= 0) {
		Fire();
		// 発射タイマーの初期化
		fireTimer_ = kFireInterval;
	}
	// 弾の更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	#pragma endregion 

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

void Enemy::Fire() {
	// 弾の速度
	const float kBulletSpeed = -1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録
	bullets_.push_back(newBullet);
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
	// 弾
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}