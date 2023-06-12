#include "Enemy/Enemy.h"
#include "WorldTransform.h"
#include "math/MyMatrix.h"
#include "ImGuiManager.h"
#include <cassert>
#include <stdio.h>

Enemy::Enemy() { 
	state_ = new EnemyStateApproach();
}

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

	//ApproachInitialize();
}

void Enemy::Move(const Vector3 velocity) {
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity);
}

void Enemy::ApproachInitialize() {
	// 発射タイマーの初期化
	fireTimer_ = kFireInterval;
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

	// 行列の更新
	worldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// 状態遷移
	state_->Update(this);
}

void Enemy::ChangeState(BaseEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// enemy
	model_->Draw(worldTransform_, viewProjection, enemyTexture_);
	// 弾
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void EnemyStateApproach::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = {0, 0, -0.25f};

	// 移動処理
	enemy->Move(kMoveSpeed);

	// 既定の位置に到達したら離脱
	if (enemy->GetEnemyPos().z < 0.0f) {
		enemy->ChangeState(new EnemyStateLeave());
	}
}

void EnemyStateLeave::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = {-0.25f, 0.25f, -0.25f};

	// 移動処理
	enemy->Move(kMoveSpeed);
}