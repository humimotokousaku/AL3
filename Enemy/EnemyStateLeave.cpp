#include "EnemyStateLeave.h"
#include "Enemy/Enemy.h"
#include "Vector3.h"

void EnemyStateLeave::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = {-0.25f, 0.25f, -0.25f};

	// 移動処理
	enemy->Move(kMoveSpeed);
}