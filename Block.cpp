#include "Block.h"
#include <cassert>

Vector3 Block::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

Block::Block() {}

Block::~Block() {}

void Block::Initialize(Model* model, const Vector3& pos, const Vector3& scale) {
	// NULLポインタチェック
	assert(model);
	model_ = model;

	// テクスチャ読み込み
	blockTexture_ = TextureManager::Load("black.png");

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = pos;
	// 大きさ
	worldTransform_.scale_ = scale;
}

void Block::Update() {
	// 行列の更新
	worldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Block::Draw(ViewProjection& viewProjection) {
	// Block
	model_->Draw(worldTransform_, viewProjection, blockTexture_);
}