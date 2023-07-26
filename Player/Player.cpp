#include "Player/Player.h"
#include "Collision/CollisionConfig.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "WorldTransform.h"
#include "math/MyMatrix.h"
#include "math/Lerp.h"
#include <cassert>

void Player::OnCollision() {}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

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

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
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

void Player::DeployReticle() {
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 自機から3Dレティクルへのオフセット(Z+向き)
	Vector3 offset{0, 0, 1.0f};
	// 自機のワールド行列の回転を反映する
	offset = TransformNormal(offset, worldTransform_.matWorld_);
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

void Player::LockOnReticle(Enemy* enemy, const ViewProjection& viewProjection) { 
	Vector2 enemyScreenPos = enemy->GetEnemyScreenPos(viewProjection);
	Vector2 e2r{};
	e2r.x = enemyScreenPos.x - positionReticle_.x;
	e2r.y = enemyScreenPos.y - positionReticle_.y;
	float e2rR = 80;
	if ((e2r.x * e2r.x) + (e2r.y * e2r.y) <= e2rR * e2rR) {
		worldTransform3DReticle_.translation_ = enemy->GetEnemyPos();
		worldTransform3DReticle_.UpdateMatrix();
		sprite2DReticle_->SetPosition(enemyScreenPos);
	}
	else {
		//Lerp()
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
		newBullet->Initialize(model_, worldPos, velocity);

		// 弾を登録
		gameScene_->AddPlayerBullet(newBullet);
	}
}

Player::Player() {}
Player::~Player() { delete sprite2DReticle_; }

// Initializeの関数定義
void Player::Initialize(Model* model, uint32_t textureHandle, const Vector3& pos) {
	// NULLポインタチェック
	assert(model);

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// テクスチャ読み込み
	// レティクル
	reticleTexture_ = TextureManager::Load("black.png");

	
	// レティクル用のテクスチャ取得
	uint32_t textureReticle = TextureManager::Load("reticle.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 320}, {1, 1, 1, 1}, {0.5f, 0.5f});

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
	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();
}

// Updateの関数定義
void Player::Update(const ViewProjection& viewProjection) {

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
	// 3Dレティクルの配置
	DeployReticle();

	// 3Dレティクルのワールド座標を取得
	positionReticle_ = GetWorld3DReticlePosition();
	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport{};
	matViewProjectionViewport = Multiply(viewProjection.matView, Multiply(viewProjection.matProjection, matViewport));

	// ワールド→スクリーン座標変換
	positionReticle_ = Transform(positionReticle_, matViewProjectionViewport);

	sprite2DReticle_->SetPosition(Vector2(positionReticle_.x, positionReticle_.y));


	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// playerの座標表示
	ImGui::Begin(" ");
	ImGui::Text("KeysInfo   SPACE:bullet  A,D:Rotate");
	// float3スライダー
	ImGui::SliderFloat3("Player", *inputFloat3, -30.0f, 30.0f);
	ImGui::End();
}

void Player::DrawUI() {
	sprite2DReticle_->Draw(); 
}

// Drawの関数定義
void Player::Draw(ViewProjection& viewProjection) {
	// player
	model_->Draw(worldTransform_, viewProjection, playerTexture_);
// 3Dレティクルを描画
	model_->Draw(worldTransform3DReticle_, viewProjection);
}