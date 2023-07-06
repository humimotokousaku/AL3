#include "Player/Player.h"
#include "math/MyMatrix.h"
#include "Collision/CollisionConfig.h"
#include "WorldTransform.h"
#include <cassert>
#include "ImGuiManager.h"

Vector3 Player::GetWorldPosition() { 
	// ワールド座標を入れる変数
	Vector3 worldPos; 
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

Player::Player() {}
Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

// Initializeの関数定義
void Player::Initialize(Model* model, uint32_t textureHandle, const Vector3& pos) {
	// NULLポインタチェック
	assert(model);

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	playerTexture_ = textureHandle;

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributePlayer);

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

// playerの回転
void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

// 攻撃
void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		Vector3 worldPos = GetWorldPosition();
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldPos, velocity);

		// 弾を登録
		bullets_.push_back(newBullet);
	}
}

void Player::OnCollision() {

}

// Updateの関数定義
void Player::Update() {
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

#pragma region Move

	// 押した方向で移動ベクトルを変更

	// 左右
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	// 上下
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	// 移動限界座標
	const Vector2 kMoveLimit = {40 - 10, 30 - 15};

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimit.x);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, kMoveLimit.x);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimit.y);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, kMoveLimit.y);

	// 座標移動
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

#pragma endregion

#pragma region Rotate

	// 旋回処理
	Rotate();

	// アフィン変換行列をワールド行列に代入
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

#pragma endregion

	worldTransform_.UpdateMatrix();

	// 弾の処理
	Attack();

	// 死亡フラグの立った球を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	// 弾の更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// playerの座標表示
	ImGui::Begin(" ");
	ImGui::Text("KeysInfo   SPACE:bullet  A,D:Rotate  C:DebugCamera  ");
	// float3スライダー
	ImGui::SliderFloat3("Player", *inputFloat3, -30.0f, 30.0f);
	ImGui::End();
}

// Drawの関数定義
void Player::Draw(ViewProjection& viewProjection) {
	// player
	model_->Draw(worldTransform_, viewProjection, playerTexture_);
	// 弾
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}