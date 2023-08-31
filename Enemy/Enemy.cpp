#include "Enemy/Enemy.h"
#include "Collision/CollisionConfig.h"
#include "ImGuiManager.h"
#include "Player/Player.h"
#include "WorldTransform.h"
#include "math/Lerp.h"
#include "math/MyMatrix.h"
#include "GameScene.h"
#include <cassert>
#include <stdio.h>

Enemy::Enemy() { 
	state_ = new EnemyStateApproach(); 
}
Enemy::~Enemy() {}


void Enemy::Initialize(Model* model, Model* modelBullet, const Vector3& pos) {
	audio_ = Audio::GetInstance();
	// NULLポインタチェック
	assert(model);

	model_ = model;
	modelBullet_ = modelBullet;
	// テクスチャ読み込み
	// テクスチャ読み込み
	//normalTexture_ = TextureManager::Load("enemy.png");
	//hitTexture_ = TextureManager::Load("white1x1.png");
	//enemyTexture_ = normalTexture_;

	damageSound_ = audio_->LoadWave("se_moa05.wav");

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);

	SetRadius(8.0f);

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;

	worldTransform_.scale_ = {4,4,4};

	isHitReaction_ = false;
	hitFrame_ = 0;

	// 体力
	hp_ = 10;
	isDecrementHp_ = false;
	isDead_ = false;

	// 状態遷移
	state_->Initialize(this);
}

void Enemy::Update() {
	// 状態遷移
	state_->Update(this);

	// hpの減る処理
	if (isDecrementHp_) {
		hp_--;
		isDecrementHp_ = false;
		isHitReaction_ = true;
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
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection) {
	// enemy
	model_->Draw(worldTransform_, viewProjection);
}

bool Enemy::NonCollision() { return false; }
bool Enemy::OnCollision() {
	isDecrementHp_ = true;
	if (hp_ <= 0) {
		isDead_ = true;
	}
	return true;
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::ChangeState(BaseEnemyState* pState) {
	delete state_;
	state_ = pState;
}

void Enemy::Move(const Vector3 velocity) {
	worldTransform_.UpdateMatrix();
	Vector3 origin{};
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 2.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset{1.0f, 0, 0};
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, worldTransform_.matWorld_);
	offset = Normalize(offset);
	// ベクトルの長さを整える
	offset.x *= kDistancePlayerTo3DReticle;
	offset.y *= kDistancePlayerTo3DReticle;
	offset.z *= kDistancePlayerTo3DReticle;

	// 3Dレティクルの座標を設定
	worldTransform_.translation_.x = GetWorldPosition().x + offset.x;
	origin.y = GetWorldPosition().y + offset.y;
	worldTransform_.translation_.z = GetWorldPosition().z + offset.z;

	worldTransform_.rotation_.y += 0.01f;
}

void Enemy::Fire() {
	assert(player_);

	// 弾の速度(正の数だと敵の後ろから弾が飛ぶ)
	const float kBulletSpeed = 3;
	Vector3 velocity{1, 1, kBulletSpeed};

	// 自キャラのワールド座標を取得する
	player_->GetWorldPosition();

	velocity = Subtract(
	    {player_->GetWorldPosition().x, player_->GetWorldPosition().y + 1,
	     player_->GetWorldPosition().z},
	    GetWorldPosition());

	// ベクトルの正規化
	velocity = Normalize(velocity);

	// ベクトルの長さを、速さに合わせる
	velocity.x *= kBulletSpeed;
	velocity.y *= kBulletSpeed;
	velocity.z *= kBulletSpeed;

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(modelBullet_, GetWorldPosition(), velocity);
	newBullet->SetPlayer(player_);

	// 弾を登録
	gameScene_->AddEnemyBullet(newBullet);
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
	// 発射タイマーをセットする
	timedCalls_.push_back(new TimedCall(std::bind(&EnemyStateApproach::FireAndResetTimer, this), kFireInterval));
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
}

void EnemyStateLeave::Initialize(Enemy* enemy) { enemy_ = enemy; }

void EnemyStateLeave::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = {-0.05f, 0.05f, -0.05f};

	// 移動処理
	enemy->Move(kMoveSpeed);
}