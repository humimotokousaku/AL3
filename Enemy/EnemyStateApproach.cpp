#include "EnemyStateApproach.h"
#include "EnemyStateLeave.h"
#include "Enemy/Enemy.h"
#include "Vector3.h"

void EnemyStateApproach::Update(Enemy* enemy) {
	// 移動速度
	const Vector3 kMoveSpeed = {0, 0, -0.25f};

	// 移動処理
	enemy->Move(kMoveSpeed);

	// 既定の位置に到達したら離脱
	if (enemy->GetEnemyPos().z < 0.0f) {
		enemy->ChangeState(new EnemyStateLeave());
	}
}