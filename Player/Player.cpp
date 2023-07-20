#include "Player.h"
#include <cassert>
#include "WorldTransform.h"

Player::Player() {}
Player::~Player() {}

// Initializeの関数定義
void Player::Initialize(Model* model) { 
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

// Updateの関数定義
void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

// Drawの関数定義
void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}