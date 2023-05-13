#include "PlayerBullet.h"
#include "WorldTransform.h"
#include <cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& pos) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	bulletTexture_ = TextureManager::Load("black.png");

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;
}

void PlayerBullet::Update() {
	worldTransform_.UpdateMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, bulletTexture_);
}