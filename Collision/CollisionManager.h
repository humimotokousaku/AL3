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
	void SetEnemy(std::list<Enemy*>& enemy) { enemy_ = enemy; }

	void Initialize(Player* player, std::list<Enemy*>& enemy);

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void CheckAllCollisions();

public:
	// コライダーリスト
	std::list<Collider*> colliders_;

	Player* player_;

	std::list<Enemy*> enemy_;
};
