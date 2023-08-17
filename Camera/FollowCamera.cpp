#include "Camera/FollowCamera.h"
#include "ImGuiManager.h"
#include "math/MyMatrix.h"
#include <Input.h>
#include <Xinput.h>

void FollowCamera::Initialize() { viewProjection_.Initialize(); }

void FollowCamera::Update() {
	if (target_) {
		// 追従対象からカメラまでのオフセット
		Vector3 offset = {0.0f, 10.0f, -30.0f};
		// カメラの角度から回転行列を計算
		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);

		// オフセットをカメラの回転に合わせて回転
		offset = TransformNormal(offset, rotateMatrix);

		// 座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = Add(target_->translation_, offset);
	}
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float kRadian = 0.02f;
		if (viewProjection_.rotation_.x >= -0.26f && viewProjection_.rotation_.x <= 0.26f) {
			viewProjection_.rotation_.x -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * kRadian;
		}
		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kRadian;

	} 
	if (viewProjection_.rotation_.x <= -0.26f) {
		viewProjection_.rotation_.x = -0.2599999f;
	} else if (viewProjection_.rotation_.x >= 0.26f) {
		viewProjection_.rotation_.x = 0.2599999f;
	}
	viewProjection_.UpdateViewMatrix();
	viewProjection_.TransferMatrix();
	ImGui::Begin("camera");
	ImGui::Text("view.rotate.x %f", viewProjection_.rotation_.x);
	ImGui::End();
}