#include "Enemy/EnemyBullet.h"
#include "Collision/CollisionConfig.h"
#include "ImGuiManager.h"
#include "WorldTransform.h"
#include "math/Lerp.h"
#include "math/MyMatrix.h"
#include <cassert>

void EnemyBullet::OnCollision() { isDead_ = true; }

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
	worldTransform_.scale_.x = 1.0f;
	worldTransform_.scale_.y = 1.0f;
	worldTransform_.scale_.z = 1.0f;
}

void EnemyBullet::Initialize(Model* model, const Vector3& pos, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	bulletTexture_ = TextureManager::Load("white1x1.png");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

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
	Vector3 toPlayer = Subtract(player_->GetWorldPosition(), worldTransform_.translation_);
	toPlayer = Normalize(toPlayer);
	velocity_ = Normalize(velocity_);
	// 球面線形保管により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	velocity_ = Slerp(velocity_, toPlayer, 0.1f);
	velocity_.x *= 0.5f;
	velocity_.y *= 0.5f;
	velocity_.z *= 0.5f;

#pragma region 弾の角度

	// Y軸周り角度(θy)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ;
	velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	// X軸周りの角度(θx)
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, velocityXZ);

#pragma endregion

	// 座標を移動させる
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	// 行列を更新
	worldTransform_.UpdateMatrix();

	// 時間経過で死ぬ
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, bulletTexture_);
}