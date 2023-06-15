#include "Player/Player.h"
#include "WorldTransform.h"
#include "math/MyMatrix.h"
#include <cassert>
#include "ImGuiManager.h"

Vector3 Player::GetWorldPosition() { 
	// ワールド座標を入れる変数
	Vector3 worldPos{}; 
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

Player::Player() {}
Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}

// Initializeの関数定義
void Player::Initialize(Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	playerTexture_ = textureHandle;

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
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_,velocity);

		// 弾を登録
		bullets_.push_back(newBullet);
	}
}

// Updateの関数定義
void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

#pragma region Rotate

	// 旋回処理
	Rotate();

	//// スケーリング行列の生成
	//Matrix4x4 playerScale;
	//playerScale = MakeScaleMatrix(worldTransform_.scale_);

	//// Z,X,Y軸の回転行列の生成
	//Matrix4x4 zAxis;
	//Matrix4x4 xAxis;
	//Matrix4x4 yAxis;
	//xAxis = MakeRotateXMatrix(worldTransform_.rotation_.x);
	//yAxis = MakeRotateYMatrix(worldTransform_.rotation_.y);
	//zAxis = MakeRotateZMatrix(worldTransform_.rotation_.z);

	//// 回転行列の合成
	//Matrix4x4 playerRotate;
	//playerRotate = Multiply(zAxis, Multiply(xAxis, yAxis));

	//// 平行移動行列
	//Matrix4x4 playerTranslate;
	//playerTranslate = MakeTranslateMatrix(worldTransform_.translation_);

	// アフィン変換行列をワールド行列に代入
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

#pragma endregion

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