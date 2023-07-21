#include "Player.h"
#include "WorldTransform.h"
#include "math/MyMatrix.h"
#include <Input.h>
#include <Xinput.h>
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

Player::Player() {}
Player::~Player() {}

// Initializeの関数定義
void Player::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

// Updateの関数定義
void Player::Update() {
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

	// 速さ
		const float speed = 0.3f;
		// 移動量
		Vector3 move{(float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};
		move.x = Normalize(move).x * speed;
		move.y = Normalize(move).y * speed;
		move.z = Normalize(move).z * speed;

		// 移動量
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	}
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();

	
}

// Drawの関数定義
void Player::Draw(ViewProjection& viewProjection) { model_->Draw(worldTransform_, viewProjection); }