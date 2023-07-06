#include "Enemy/Enemy.h"
#include "Collision/CollisionConfig.h"
#include "ImGuiManager.h"
#include "Player/Player.h"
#include "WorldTransform.h"
#include "math/Lerp.h"
#include "math/MyMatrix.h"
#include <cassert>
#include <stdio.h>

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Enemy::Enemy() { state_ = new EnemyStateApproach(); }

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

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;

	// 状態遷移
	state_->Initialize(this);
}

void Enemy::Move(const Vector3 velocity) {
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity);
}

void Enemy::Fire() {
	assert(player_);

	// 弾の速度(正の数だと敵の後ろから弾が飛ぶ)
	const float kBulletSpeed = -0.5f;
	Vector3 velocity{0, 0, kBulletSpeed};

	// 自キャラのワールド座標を取得する
	player_->GetWorldPosition();

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, GetWorldPosition(), velocity);
	newBullet->SetPlayer(player_);

	// 弾を登録
	bullets_.push_back(newBullet);
}

void Enemy::OnCollision() {}

void Enemy::Update() {
	// 状態遷移
	state_->Update(this);

	// 終了した弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	// 弾の更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 行列の更新
	worldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
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

EnemyStateApproach::~EnemyStateApproach() {
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

void EnemyStateApproach::FireAndResetTimer() {
	// 弾を発射する
	enemy_->Fire();
	// 発射タイマーをセットする
	timedCalls_.push_back(
	    new TimedCall(std::bind(&EnemyStateApproach::FireAndResetTimer, this), kFireInterval));
}

void EnemyStateApproach::Initialize(Enemy* enemy) {
	enemy_ = enemy;
	FireAndResetTimer();
}

void EnemyStateApproach::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = {0, 0, -0.01f};

	// 移動処理
	enemy->Move(kMoveSpeed);

	// 終了したタイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});

	// 範囲forでリストの全要素について回す
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

	// 既定の位置に到達したら離脱
	if (enemy->GetEnemyPos().z < -15.0f) {
		timedCalls_.clear();
		enemy->ChangeState(new EnemyStateLeave());
	}
}

void EnemyStateLeave::Initialize(Enemy* enemy) { enemy_ = enemy; }

void EnemyStateLeave::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = {-0.05f, 0.05f, -0.05f};

	// 移動処理
	enemy->Move(kMoveSpeed);
}