#include "PlayerBullet.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include <cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& pos, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	bulletTexture_ = TextureManager::Load("black.png");

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;
	
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
}

void PlayerBullet::Update() {
	// 座標を移動させる
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	// 行列を更新
	worldTransform_.UpdateMatrix();

	// 時間経過で死ぬ
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, bulletTexture_);
}