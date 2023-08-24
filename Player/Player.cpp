#include "Player.h"
#include "ImGuiManager.h"
#include "math/MyMatrix.h"
#include <Input.h>
#include <Xinput.h>
#include <cassert>
#define _USE_MATH_DEFINES
#include "GameScene.h"
#include "math.h"

Player::Player() {}
Player::~Player() { delete sprite2DReticle_; }

// Initializeの関数定義
void Player::Initialize(
    Model* modelBody, Model* modelHead, Model* modelL_arm, Model* modelR_arm, Model* modelGun,
    Model* modelBullet) {
	// NULLポインタチェック
	assert(modelBody);
	assert(modelHead);
	assert(modelL_arm);
	assert(modelR_arm);
	assert(modelGun);
	assert(modelBullet);

	// 引数として受け取ったデータをメンバ変数に記録する
	modelBody_ = modelBody;
	modelHead_ = modelHead;
	modelL_arm_ = modelL_arm;
	modelR_arm_ = modelR_arm;
	modelGun_ = modelGun;
	modelBullet_ = modelBullet;

	// テクスチャ読み込み
	// レティクル
	reticleTexture_ = TextureManager::Load("black.png");

	// レティクル用のテクスチャ取得
	uint32_t textureReticle = TextureManager::Load("reticle.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 320}, {1, 1, 1, 1}, {0.5f, 0.5f});

	input_ = Input::GetInstance();

	// 腕の座標指定
	worldTransformL_arm_.translation_.x = -1.5f;
	worldTransformR_arm_.translation_.x = 1.5f;
	worldTransformL_arm_.translation_.y = 5.0f;
	worldTransformR_arm_.translation_.y = 5.0f;
	worldTransformGun_.translation_.x = 1.5f;
	worldTransformGun_.translation_.y = 5.0f;
	worldTransformGun_.translation_.z = 1.5f;

	// 当たり判定の半径設定
	SetRadius(2.0f);

	// 身体のパーツの親子関係を結ぶ
	SetParent(&GetWorldTransformBody());

	// 浮遊ギミックの初期化
	InitializeFloatingGimmick();

	// ワールド変換の初期化
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformGun_.Initialize();

	// スプライトの現在座標を取得
	spritePosition_ = sprite2DReticle_->GetPosition();
	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();
}

// Updateの関数定義
void Player::Update() {
	XINPUT_STATE joyState;
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	};
	// Lトリガーを押したらステップ回避(長押しでブースト)
	if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		behaviorRequest_ = Behavior::kStep;
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
		case Behavior::kStep:
			BehaviorStepInitialize();
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	// 移動処理(ステップ回避中も動けるようにするためにビヘイビアの外に出した)
	BehaviorRootUpdate();
	// 更新処理
	switch (behavior_) {
	case Behavior::kRoot:
	default:

		break;
		// 攻撃行動
	case Behavior::kStep:
		BehaviorStepUpdate();
		break;
	}

	//// アフィン変換行列をワールド行列に代入
	// worldTransformBase_.matWorld_ = MakeAffineMatrix(
	//     worldTransformBase_.scale_, worldTransformBase_.rotation_,
	//     worldTransformBase_.translation_);

	// 3Dレティクルの配置
	Deploy3DReticle();
	// 2Dレティクルの配置
	Deploy2DReticle(*viewProjection_);

	worldTransform3DReticle_.UpdateMatrix();

	// 弾の処理
	Attack();

	// 行列を定数バッファに転送
	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformGun_.UpdateMatrix();

	ImGui::Begin("player");
	ImGui::Text(
	    "player.pos %f %f %f", worldTransformBase_.translation_.x,
	    worldTransformBase_.translation_.y, worldTransformBase_.translation_.z);
	ImGui::Text(
	    "player.worldTransform.rotate_ %f %f %f", worldTransformBase_.rotation_.x,
	    worldTransformBase_.rotation_.y, worldTransformBase_.rotation_.z);
	ImGui::Text("behavior %d", behaviorRequest_);
	ImGui::Text("stepFrame %d", stepFrame_);
	ImGui::Text("Vel %f %f", velocity_.x, velocity_.z);

	ImGui::End();
}

void Player::DrawUI() { sprite2DReticle_->Draw(); }
void Player::Draw(ViewProjection& viewProjection) {
	modelBody_->Draw(worldTransformBody_, viewProjection);
	modelHead_->Draw(worldTransformHead_, viewProjection);
	modelL_arm_->Draw(worldTransformL_arm_, viewProjection);
	modelR_arm_->Draw(worldTransformR_arm_, viewProjection);
	modelGun_->Draw(worldTransformGun_, viewProjection);
	// 3Dレティクルを描画
	modelBullet_->Draw(worldTransform3DReticle_, viewProjection);
}

bool Player::NonCollision() {
	isDead_ = false;
	return false;
}
bool Player::OnCollision() {
	// 移動ベクトルを反転
	velocity_ = Multiply(-1.0f, velocity_);
	//
	worldTransformBase_.translation_ = Add(velocity_, worldTransformBase_.translation_);
	worldTransformBase_.UpdateMatrix();
	isDead_ = true;
	return true;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransformBody_.matWorld_.m[3][0];
	worldPos.y = worldTransformBody_.matWorld_.m[3][1];
	worldPos.z = worldTransformBody_.matWorld_.m[3][2];

	return worldPos;
}
Vector3 Player::GetWorld3DReticlePosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetWorldPos(Vector3 prePos) {
	worldTransformBase_.matWorld_.m[3][0] = prePos.x;
	worldTransformBase_.matWorld_.m[3][1] = prePos.y;
	worldTransformBase_.matWorld_.m[3][2] = prePos.z;
	worldTransformBody_.matWorld_.m[3][0] = prePos.x;
	worldTransformBody_.matWorld_.m[3][1] = prePos.y;
	worldTransformBody_.matWorld_.m[3][2] = prePos.z;
}
void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransformBase_.parent_ = parent;
	worldTransformHead_.parent_ = parent;
	worldTransformL_arm_.parent_ = parent;
	worldTransformR_arm_.parent_ = parent;
	worldTransformGun_.parent_ = parent;
}

void Player::InitializeFloatingGimmick() { floatingParameter_ = 0.0f; }
void Player::UpdateFloatingGimmick() {
	// 浮遊移動のサイクル<frame>
	const uint16_t T = 120;
	// 1フレームでのパラメータ加算値
	const float step = 2.0f * (float)M_PI / T;
	// パラメータを1ステップ分加算
	floatingParameter_ += step;
	// 2πを超えたら0に戻す
	floatingParameter_ = (float)std::fmod(floatingParameter_, 2.0f * M_PI);
	// 浮遊の振幅<m>
	const float floatingAmplitude = 0.2f;
	// 浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingAmplitude;

	// 腕の動き
	worldTransformL_arm_.rotation_.x = std::sin(floatingParameter_) * 0.75f;
	worldTransformR_arm_.rotation_.x = std::sin(floatingParameter_) * 0.75f;
}

void Player::BehaviorRootInitialize() {}
void Player::BehaviorRootUpdate() {
	Matrix4x4 rotateMatrix{};
	Vector3 move{};
	XINPUT_STATE joyState;
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 速さ
		const float speed = 0.7f;
		// 移動量
		move = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};
		// 移動量の速さを反映
		move = Multiply(speed, Normalize(move));

		// 回転行列
		rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
		// 移動ベクトルをカメラの角度だけ回転
		move = TransformNormal(move, rotateMatrix);
		// Playerは見ている方向に進んでいくので地面にも進んでしまう。なのでy軸方向には進まないようにする
		move.y = 0;
		// 移動ベクトルをメンバ変数に反映
		velocity_ = move;
		if (!isDead_) {
			// 移動量
			worldTransformBase_.translation_ = Add(worldTransformBase_.translation_, move);
			worldTransformBody_.translation_ = worldTransformBase_.translation_;

			// playerのY軸周り角度(θy)
			// moveが0じゃないときに値を渡さないとplayerが止まった時に挙動がおかしくなる
			if (move.x != 0 && move.z != 0) {
				worldTransformBase_.rotation_.y = std::atan2(move.x, move.z);
				worldTransformBody_.rotation_.y = worldTransformBase_.rotation_.y;
			}
		}
	}

	// 浮遊ギミックの更新処理
	UpdateFloatingGimmick();
}

void Player::BehaviorStepInitialize() {}

void Player::BehaviorStepUpdate() {
	// ステップ回避
	StepMove();
}

void Player::Attack() {
	XINPUT_STATE joyState;
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	// Rトリガーを押していたら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ||
	    input_->TriggerKey(DIK_SPACE)) {
		//  弾の速度
		const float kBulletSpeed = 6.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		Vector3 worldPos;
		worldPos.x = worldTransformGun_.matWorld_.m[3][0];
		worldPos.y = worldTransformGun_.matWorld_.m[3][1];
		worldPos.z = worldTransformGun_.matWorld_.m[3][2];

		velocity = TransformNormal(velocity, worldTransformBase_.matWorld_);
		// 自機から照準オブジェクトへのベクトル
		Vector3 worldReticlePos = {
		    worldTransform3DReticle_.matWorld_.m[3][0], worldTransform3DReticle_.matWorld_.m[3][1],
		    worldTransform3DReticle_.matWorld_.m[3][2]};
		velocity = Subtract(worldReticlePos, worldPos);
		velocity = Normalize(velocity);
		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(modelBullet_, worldPos, velocity);

		// 弾を登録
		gameScene_->AddPlayerBullet(newBullet);

		// 射撃中はカメラの向いている方向に身体を向ける
		worldTransformBase_.rotation_ = viewProjection_->rotation_;
		worldTransformBase_.rotation_.x = 0;

		worldTransformBody_.rotation_ = viewProjection_->rotation_;
		worldTransformBody_.rotation_.x = 0;
	}
}

void Player::StepMove() {
	XINPUT_STATE joyState{};

	Matrix4x4 rotateMatrix{};
	// 速さ
	const float speed = 4.0f;
	Vector3 stepVelocity_ = velocity_;
	if (behavior_ == Behavior::kStep) {
		if (stepVelocity_.x == 0 && stepVelocity_.z == 0) {
			stepVelocity_ = {0, 0, 1};
			// 移動量の速さを反映
			stepVelocity_ = Multiply(speed, Normalize(stepVelocity_));
			//  回転行列
			rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
			// 移動ベクトルをカメラの角度だけ回転
			stepVelocity_ = TransformNormal(stepVelocity_, rotateMatrix);
			// Playerは見ている方向に進んでいくので地面にも進んでしまう。なのでy軸方向には進まないようにする
			stepVelocity_.y = 0;
		} else {
			stepVelocity_ = velocity_;
			stepVelocity_.x *= speed;
			stepVelocity_.z *= speed;
		}
		if (stepFrame_ <= 14) {
			// 移動量
			worldTransformBase_.translation_ = Add(worldTransformBase_.translation_, stepVelocity_);
			worldTransformBody_.translation_ = worldTransformBase_.translation_;
			// playerのY軸周り角度(θy)
			worldTransformBase_.rotation_.y = std::atan2(stepVelocity_.x, stepVelocity_.z);
			worldTransformBody_.rotation_.y = worldTransformBase_.rotation_.y;
		}
		stepFrame_++;
	}

	if (stepFrame_ >= 15) {
		stepFrame_ = 0;
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::Deploy3DReticle() {
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 100.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset{0, 0, 1.0f};
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, worldTransformBase_.matWorld_);
	offset = Normalize(offset);
	// ベクトルの長さを整える
	offset.x *= kDistancePlayerTo3DReticle;
	offset.y *= kDistancePlayerTo3DReticle;
	offset.z *= kDistancePlayerTo3DReticle;

	// 3Dレティクルの座標を設定
	worldTransform3DReticle_.translation_.x = GetWorldPosition().x + offset.x;
	worldTransform3DReticle_.translation_.y = GetWorldPosition().y + offset.y;
	worldTransform3DReticle_.translation_.z = GetWorldPosition().z + offset.z;

	worldTransform3DReticle_.UpdateMatrix();
}

void Player::Deploy2DReticle(const ViewProjection& viewProjection) {
	// 3Dレティクルのワールド座標を取得
	Vector3 positionReticle = GetWorld3DReticlePosition();
	// ビューポート行列
	matViewport_ = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport =
	    Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewport_));
	// ワールド→スクリーン座標変換
	positionReticle = Transform(positionReticle, matViewProjectionViewport);
	// スプライトのレティクルに座標設定
	sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

	XINPUT_STATE joyState;
	Vector2 joyRange{};
	// ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		joyRange.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 0.02f;
		joyRange.y += (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 0.04f;
		spritePosition_.x += joyRange.x;
		spritePosition_.y -= joyRange.y;
		// スプライトへの座標変更を反映
		sprite2DReticle_->SetPosition(spritePosition_);
	}

	Matrix4x4 matViewport =
	    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matVPV =
	    Multiply(viewProjection_->matView, Multiply(viewProjection_->matProjection, matViewport));
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);
	// matInverseVPV * matVPV = 単位行列になっているかチェック
	Matrix4x4 checkInverse = Multiply(matInverseVPV, matVPV);

	// スクリーン座標
	Vector3 posNear = Vector3(
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 0);
	Vector3 posFar = Vector3(
	    (float)sprite2DReticle_->GetPosition().x, (float)sprite2DReticle_->GetPosition().y, 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);
	// マウスレイの方向
	Vector3 mouseDirection = Subtract(posFar, posNear);
	mouseDirection = Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	// 3Dレティクルを2Dカーソルに配置
	worldTransform3DReticle_.translation_.x = posNear.x + mouseDirection.x * kDistanceTestObject;
	worldTransform3DReticle_.translation_.y = posNear.y + mouseDirection.y * kDistanceTestObject;
	worldTransform3DReticle_.translation_.z = posNear.z + mouseDirection.z * kDistanceTestObject;
}
