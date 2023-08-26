#include "Camera/FollowCamera.h"
#include "ImGuiManager.h"
#include "math/MyMatrix.h"
#include "math/Lerp.h"
#include <Input.h>
#include <Xinput.h>

void FollowCamera::Initialize() { viewProjection_.Initialize(); }

void FollowCamera::Update() {
	if (target_) {
		// 追従対象からカメラまでのオフセット
		Vector3 offset = TargetOffset();
		// カメラの角度から回転行列を計算
		//Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);

		// オフセットをカメラの回転に合わせて回転
		//offset = TransformNormal(offset, rotateMatrix);

		// 追従座標の補間
		interTarget_ = Lerp(interTarget_, target_->translation_, 0.1f);

		// 座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = Add(interTarget_, offset);
	}
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float kRadian = 0.04f;
		if (viewProjection_.rotation_.x >= -0.26f && viewProjection_.rotation_.x <= 0.26f) {
			viewProjection_.rotation_.x -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 0.02f;
		}
		destinationAngleY_ += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kRadian;
	} 
	viewProjection_.rotation_.y =
	    LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.1f);

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

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

Vector3 FollowCamera::TargetOffset() const {
	// 追従対象からのオフセット
	Vector3 offset = {0, 8, -50};
	// 回転行列を合成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);

	// オフセットをカメラの回転に合わせて回転
	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::Reset() { 
	if (target_) {
		// 追従座標・角度の初期化
		interTarget_ = target_->translation_;
		viewProjection_.rotation_.y = target_->rotation_.y;
	}
	destinationAngleY_ = viewProjection_.rotation_.y;

	// 追従対象からのオフセット
	Vector3 offset = TargetOffset();
	viewProjection_.translation_ = Add(interTarget_, offset);
}