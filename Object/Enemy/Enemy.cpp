#include "Enemy.h"
#include "math/MyMatrix.h"

void Enemy::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	ICharacter::Initialize(models);
}

void Enemy::Update() {
	// 速さ
	const float kSpeed = 0.3f;
	Vector3 velocity{0.0f, 0.0f, kSpeed};

	// 移動ベクトルをカメラの角度だけ回転
	velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	// 移動量
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity);
	// 自機のY軸周り角度(θy)
	worldTransform_.rotation_.y += 0.03f;

	// 基底クラスの更新処理
	ICharacter::Update();
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	ICharacter::Draw(viewProjection);
}