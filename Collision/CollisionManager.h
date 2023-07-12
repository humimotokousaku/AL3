#pragma once
#include "Collision/CollisionConfig.h"
#include "Collision/Collider.h"
#include "Player/Player.h"
#include "Enemy//Enemy.h"
#include <list>

class CollisionManager {
public:
	~CollisionManager();

	void SetPlayer(Player* player) { player_ = player; }

	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void CheckAllCollisions();

	void Initialize(Player* player,Enemy* enemy);

public:
	// コライダーリスト
	std::list<Collider*> colliders_;

	Player* player_;

	Enemy* enemy_;
};
