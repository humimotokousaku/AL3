#include "Enemy/EnemyBullet.h"
#include "Collision/CollisionConfig.h"
#include "ImGuiManager.h"
#include "WorldTransform.h"
#include "../math/Lerp.h"
#include "math/MyMatrix.h"
#include <cassert>

bool EnemyBullet::NonCollision() {
	return false;
}
bool EnemyBullet::OnCollision() {
	isDead_ = true;
	return true;
}

Vector3 EnemyBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void EnemyBullet::SettingScale() {
	worldTransform_.scale_.x = 2.0f;
	worldTransform_.scale_.y = 2.0f;
	worldTransform_.scale_.z = 2.0f;
}

void EnemyBullet::Initialize(Model* model, const Vector3& pos, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	bulletTexture_ = TextureManager::Load("red.png");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

	SetRadius(4.0f);

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;

	// 形状を設定
	SettingScale();

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void EnemyBullet::Update() {
	// 座標を移動させる
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	// 行列を更新
	worldTransform_.UpdateMatrix();

	// 時間経過で死ぬ
	if (--deathTimer_ <= 0 || worldTransform_.translation_.y < -1) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	if (!isDead_) {
		model_->Draw(worldTransform_, viewProjection, bulletTexture_);
	}
}