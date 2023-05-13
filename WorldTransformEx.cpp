#include "WorldTransform.h"
#include "Player.h"

void WorldTransform::UpdateMatrix() {
	// スケール、回転、平行移動を合成して行列を計算する
	matWorld_ = Player::MakeAffineMatrix(scale_, rotation_, translation_);
	// 定数バッファに転送
	TransferMatrix();
}