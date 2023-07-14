#pragma once
#include "IEnemyState.h"

class Enemy; // 前方宣言

// 接近フェーズのクラス
class EnemyStateApproach : public IEnemyState {
public:
	void Update(Enemy* enemy);
};