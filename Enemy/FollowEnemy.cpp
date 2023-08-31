#include "FollowEnemy.h"
#include "Collision/CollisionConfig.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "Player/Player.h"
#include "WorldTransform.h"
#include "math/Lerp.h"
#include "math/MyMatrix.h"
#include <cassert>
#include <stdio.h>

FollowEnemy::FollowEnemy() {}
FollowEnemy::~FollowEnemy() {}

void FollowEnemy::Initialize(Model* model, const Vector3& pos) {
	audio_ = Audio::GetInstance();
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	normalTexture_ = TextureManager::Load("black.png");
	hitTexture_ = TextureManager::Load("white1x1.png");

	followEnemyTexture_ = normalTexture_;

	damageSound_ = audio_->LoadWave("se_moa05.wav");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

	SetRadius(8.0f);

	// ワールド変換の初期化
	worldTransform_.Initialize();
	hitWorldTransform_.Initialize();

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;
	hitWorldTransform_.translation_ = worldTransform_.translation_;

	worldTransform_.scale_ = {4, 4, 4};

	isHitReaction_ = false;
	hitFrame_ = 0;

	isDead_ = false;
	isDecrementHp_ = false;
	hp_ = 10;

	const float kSpeed = -0.1f;
	velocity_ = {0, 0, kSpeed};
}

void FollowEnemy::Update() {
	// 状態遷移
	Vector3 toPlayer = Subtract(player_->GetWorldPosition(), worldTransform_.translation_);
	toPlayer = Normalize(toPlayer);
	velocity_ = Normalize(velocity_);
	// 球面線形保管により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	velocity_ = Slerp(velocity_, toPlayer, 0.05f);
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

	// hpの減る処理
	if (isDecrementHp_) {
		hp_--;
		isDecrementHp_ = false;
	}

	if (isHitReaction_) {
		if (hitFrame_ <= 3) {
			audio_->PlayWave(damageSound_, false, 0.01f);
		}
		if (hitFrame_ <= 5) {
			
		} else {
			isHitReaction_ = false;
			hitFrame_ = 0;
		}
		hitFrame_++;
	}

	// 行列の更新
	worldTransform_.UpdateMatrix();
	hitWorldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void FollowEnemy::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}

bool FollowEnemy::NonCollision() { return false; }
bool FollowEnemy::OnCollision() {
	isDecrementHp_ = true;
	isHitReaction_ = true;
	if (hp_ <= 0) {
		isDead_ = true;
	}
	return true;
}

Vector3 FollowEnemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void FollowEnemy::Move(const Vector3 velocity) {
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity);
}

void FollowEnemy::Fire() {

}
