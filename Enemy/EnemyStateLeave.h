#pragma once
#include "IEnemyState.h"

class Enemy;

// 離脱フェーズのクラス
class EnemyStateLeave : public IEnemyState {
public:
	void Update(Enemy* enemy);
};