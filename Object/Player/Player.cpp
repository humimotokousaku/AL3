#include "Player.h"
#include "ImGuiManager.h"
#include "math/MyMatrix.h"
#include <Input.h>
#include <Xinput.h>
#include <cassert>
#define _USE_MATH_DEFINES
#include "math.h"
#include "GlobalVariables.h"

Player::Player() {}
Player::~Player() {}

// Initializeの関数定義
void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	ICharacter::Initialize(models);

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

	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformHammer_.Initialize();

	globalVariables_ = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables_->GetVector3Value(groupName, "Head Translation");
	globalVariables_->GetVector3Value(groupName, "ArmL Translation");
	globalVariables_->GetVector3Value(groupName, "ArmR Translation");
	globalVariables_->GetIntValue(groupName, "floatingCycle_Arms");
	globalVariables_->GetIntValue(groupName, "floatingCycleBody");
	globalVariables_->GetFloatValue(groupName, "floatingAmplitude");
}

// Updateの関数定義
void Player::Update() {
	// 基底クラスの更新処理
	ICharacter::Update();

	XINPUT_STATE joyState;
	// ゲームパッド状態取得
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	// Rトリガーを押したら攻撃
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		behaviorRequest_ = Behavior::kAttack;
	}

	// 初期化
	if (behaviorRequest_) {
		//  振るまいを変更
		behavior_ = behaviorRequest_.value();
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
			// 攻撃行動
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	// 更新処理
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
		// 攻撃行動
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	worldTransformHammer_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	globalVariables_->SaveFile("Player");
}

// Drawの関数定義
void Player::Draw(const ViewProjection& viewProjection) {
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexHead]->Draw(worldTransformHead_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);
	if (behavior_ == Behavior::kAttack) {
		models_[kModelIndexWeapon]->Draw(worldTransformHammer_, viewProjection);
	}
}

void Player::BehaviorRootInitialize() {
	worldTransformL_arm_.rotation_.x = 0.0f;
	worldTransformR_arm_.rotation_.x = 0.0f;
	worldTransformHammer_.rotation_.x = 0.0f;
	// 浮遊ギミックの初期化
	InitializeFloatingGimmick();

	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformHammer_.Initialize();
}

void Player::BehaviorRootUpdate() {
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
}

void Player::BehaviorAttackInitialize() {
	worldTransformL_arm_.rotation_.x = (float)M_PI;
	worldTransformR_arm_.rotation_.x = (float)M_PI;
	worldTransformHammer_.rotation_.x = 0.0f;
	attackAnimationFrame = 0;
}

void Player::BehaviorAttackUpdate() {
	if (attackAnimationFrame < 10) {
		// 腕の挙動
		worldTransformL_arm_.rotation_.x -= 0.05f;
		worldTransformR_arm_.rotation_.x -= 0.05f;

		// 武器の挙動
		worldTransformHammer_.rotation_.x -= 0.05f;
	} else if (worldTransformHammer_.rotation_.x <= 2.0f * (float)M_PI / 4) {
		// 腕の挙動
		worldTransformL_arm_.rotation_.x += 0.1f;
		worldTransformR_arm_.rotation_.x += 0.1f;
		// 武器の挙動
		worldTransformHammer_.rotation_.x += 0.1f;
	} else {
		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kRoot;
	}
	attackAnimationFrame++;
}

void Player::InitializeFloatingGimmick() {
	for (int i = 0; i < kMaxMoveModelParts; i++) {
		floatingParameter_[i] = 0.0f;
	}
}

void Player::UpdateFloatingGimmick() {
	// 浮遊移動のサイクル<frame>
	//uint16_t floatingCycle[2]{};
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
	
	// 浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_[0]) * floatingAmplitude;

	// 腕の動き
	worldTransformL_arm_.rotation_.x = std::sin(floatingParameter_[1]) * 0.75f;
	worldTransformR_arm_.rotation_.x = -std::sin(floatingParameter_[1]) * 0.75f;
}

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
	worldTransformHammer_.parent_ = parent;
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	worldTransformHead_.translation_ = globalVariables->GetVector3Value(groupName, "Head Translation"); 
	worldTransformL_arm_.translation_ = globalVariables->GetVector3Value(groupName, "ArmL Translation");
	worldTransformR_arm_.translation_ = globalVariables->GetVector3Value(groupName, "ArmR Translation");
	floatingCycle[0] = globalVariables->GetIntValue(groupName, "floatingCycle_Arms");
	floatingCycle[1] = globalVariables->GetIntValue(groupName, "floatingCycleBody");
	floatingAmplitude = globalVariables->GetFloatValue(groupName, "floatingAmplitude");
}