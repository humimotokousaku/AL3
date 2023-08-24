#include "Block.h"
#include "ImGuiManager.h"
#include <cassert>
#include "Collision/CollisionConfig.h"

bool Block::NonCollision() { 
	isCollision_ = false;
	return false;
}
bool Block::OnCollision() { 
	isCollision_ = true;
	return true;
}

void Block::ResetPlayerPos(Player* player) { 
	player->SetWorldPos(Vector3{0,0,3}); 
}

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

	// 当たり判定の半径設定
	SetRadius(6.0f);
	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeBlock);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeBlock);

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

	ImGui::Begin("isCollision");
	ImGui::Text("%d", isCollision_);
	ImGui::End();
}

void Block::Draw(ViewProjection& viewProjection) {
	// Block
	model_->Draw(worldTransform_, viewProjection, blockTexture_);
}