#pragma once
#include "Collision/CollisionConfig.h"
#include "Collision/Collider.h"
#include "Player/Player.h"
#include "Player/PlayerBullet.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyBullet.h"
#include "Enemy/FollowEnemy.h"
#include <list>

class GameScene;

class CollisionManager {
public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetPlayerBullet(const std::list<PlayerBullet*>& playerBullet) { playerBullet_ = playerBullet; }
	void SetEnemy(const std::list<Enemy*>& enemy) { enemy_ = enemy; }
	void SetEnemyBullet(const std::list<EnemyBullet*>& enemyBullet) { enemyBullet_ = enemyBullet; }
	void SetFollowEnemy(const std::list<FollowEnemy*>& followEnemy) { followEnemy_ = followEnemy; }

	// 当たり判定を必要とするObjectをまとめてセットする
	void SetGameObject(
	    Player* player, const std::list<PlayerBullet*>& playerBullet,
	    const std::list<Enemy*>& enemy, const std::list<EnemyBullet*>& enemyBullet,
		const std::list<FollowEnemy*> followEnemy);

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void CheckAllCollisions(GameScene* gameScene);

private:
	// コライダーリスト
	std::list<Collider*> colliders_;

	// 自機
	Player* player_;
	std::list<PlayerBullet*> playerBullet_;

	// 敵
	std::list<Enemy*> enemy_;
	std::list<EnemyBullet*> enemyBullet_;
	std::list<FollowEnemy*> followEnemy_;
};
