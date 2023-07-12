#pragma once
#include "Collision/CollisionConfig.h"
#include "Collision/Collider.h"
#include "Player/Player.h"
#include "Enemy//Enemy.h"
#include "Enemy/EnemyBullet.h"
#include <list>

class GameScene;

class CollisionManager {
public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetPlayerBullet(const std::list<PlayerBullet*>& playerBullet) { playerBullet_ = playerBullet; }
	void SetEnemy(const std::list<Enemy*>& enemy) { enemy_ = enemy; }
	void SetEnemyBullet(const std::list<EnemyBullet*>& enemyBullet) { enemyBullet_ = enemyBullet; }

	// 当たり判定を必要とするObjectをまとめてセットする
	void SetGameObject(
	    Player* player, const std::list<Enemy*>& enemy, const std::list<EnemyBullet*>& enemyBullet,
	    const std::list<PlayerBullet*>& playerBullet);

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void CheckAllCollisions(GameScene* gameScene, Player* player);

private:
	// コライダーリスト
	std::list<Collider*> colliders_;

	Player* player_;

	std::list<PlayerBullet*> playerBullet_;

	std::list<Enemy*> enemy_;

	std::list<EnemyBullet*> enemyBullet_;
};
