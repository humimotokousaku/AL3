#include "Player.h"
#include "ImGuiManager.h"
#include "math/MyMatrix.h"
#include <Input.h>
#include <Xinput.h>
#include <cassert>
#define _USE_MATH_DEFINES
#include "math.h"

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
	worldTransformHead_.parent_ = parent;
	worldTransformL_arm_.parent_ = parent;
	worldTransformR_arm_.parent_ = parent;
}

void Player::InitializeFloatingGimmick() {
	for (int i = 0; i < kMaxMoveModelParts; i++) {
		floatingParameter_[i] = 0.0f;
	}
}

void Player::UpdateFloatingGimmick() {
	// 浮遊移動のサイクル<frame>
	uint16_t floatingCycle[2]{};
	floatingCycle[0] = 30;
	floatingCycle[1] = 60;
	// 1フレームでのパラメータ加算値
	float step[2]{};
	for (int i = 0; i < kMaxMoveModelParts; i++) {
		step[i] = 2.0f * (float)M_PI / floatingCycle[i];
		// パラメータを1ステップ分加算
		floatingParameter_[i] += step[i];
		// 2πを超えたら0に戻す
		floatingParameter_[i] = (float)std::fmod(floatingParameter_[i], 2.0f * M_PI);
	}
	// 浮遊の振幅<m>
	const float floatingAmplitude = 0.5f;
	// 浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_[0]) * floatingAmplitude;

	// 腕の動き
	worldTransformL_arm_.rotation_.x = std::sin(floatingParameter_[1]) * 0.75f;
	worldTransformR_arm_.rotation_.x = -std::sin(floatingParameter_[1]) * 0.75f;
}

Player::Player() {}
Player::~Player() {}

// Initializeの関数定義
void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	ICharacter::Initialize(models);

	// 引数として受け取ったデータをメンバ変数に記録する
	models_[kModelIndexBody] = models[kModelIndexBody];
	models_[kModelIndexHead] = models[kModelIndexHead];
	models_[kModelIndexL_arm] = models[kModelIndexL_arm];
	models_[kModelIndexR_arm] = models[kModelIndexR_arm];

	// 腕の座標指定
	worldTransformL_arm_.translation_.x = -1.5f;
	worldTransformR_arm_.translation_.x = 1.5f;
	worldTransformL_arm_.translation_.y = 5.0f;
	worldTransformR_arm_.translation_.y = 5.0f;

	// 身体のパーツの親子関係を結ぶ
	SetParent(&GetWorldTransformBody());
	worldTransformBody_.parent_ = worldTransform_.parent_;

	// 浮遊ギミックの初期化
	InitializeFloatingGimmick();

	worldTransform_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
}

// Updateの関数定義
void Player::Update() {
	//// 基底クラスの更新処理
	//ICharacter::Update();

	XINPUT_STATE joyState;
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 速さ
		const float speed = 0.3f;
		// 移動量
		Vector3 move{
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};
		// 移動量の速さを反映
		move = Multiply(speed, Normalize(move));

		// 回転行列
		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
		// 移動ベクトルをカメラの角度だけ回転
		move = TransformNormal(move, rotateMatrix);

		// 移動量
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		worldTransformBody_.translation_ = worldTransform_.translation_;

		// playerのY軸周り角度(θy)
		worldTransform_.rotation_.y = std::atan2(move.x, move.z);
		worldTransformBody_.rotation_.y = worldTransform_.rotation_.y;
	}

	// 浮遊ギミックの更新処理
	UpdateFloatingGimmick();

	// 基底クラスの更新処理
	ICharacter::Update();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

// Drawの関数定義
void Player::Draw(const ViewProjection& viewProjection) {
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexHead]->Draw(worldTransformHead_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);
}