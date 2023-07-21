#include "Camera/FollowCamera.h"
#include "math/MyMatrix.h"
#include <Input.h>
#include <Xinput.h>

void FollowCamera::Initialize() { 

	viewProjection_.Initialize(); 
}

void FollowCamera::Update() {
	if (target_) {
		// 追従対象からカメラまでのオフセット
		Vector3 offset = {0.0f, 2.0f, -10.0f};
		// 座標をコピーしてオフセット分ずらす
		viewProjection_.translation_ = Add(target_->translation_,offset);
	}
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float kRadian = 0.02f;
		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kRadian;
	}

	viewProjection_.UpdateViewMatrix(); 
	viewProjection_.TransferMatrix();
}