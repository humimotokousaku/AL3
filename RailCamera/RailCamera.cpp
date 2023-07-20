#pragma once
#include "RailCamera/RailCamera.h"
#include "ImGuiManager.h"
#include "math/MyMatrix.h"
#include <Input.h>
#include <Xinput.h>

void RailCamera::Initialize(WorldTransform worldTransform, const Vector3& radian) {
	// ワールドトランスフォームの初期設定
	// 引数で受け取った初期座標をセット
	worldTransform_ = worldTransform;
	worldTransform_.translation_.z = -10;

	// 引数で受け取った初期座標をセット
	worldTransform_.rotation_ = radian;

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void RailCamera::Update() {
	Vector3 velocity{0.0f, 0.0f, 0.02f};
	Vector3 move{};
	Vector3 radian{};


	// 回転処理
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		radian.x -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 0.02f;
		radian.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 0.02f;
		
	}
	// 移動処理
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * 0.2f;
		move.z += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * 0.2f;
	}
	// 向いている方向に移動
	velocity = TransformNormal(move, worldTransform_.matWorld_);

	worldTransform_.rotation_ = Add(worldTransform_.rotation_, radian);
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity);

	// 行列の更新
	worldTransform_.UpdateMatrix();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.1f);
	ImGui::End();
}