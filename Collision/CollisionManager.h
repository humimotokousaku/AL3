#pragma once
#include "Collision/CollisionConfig.h"
#include "Collision/Collider.h"
#include "Player/Player.h"
#include "Player/PlayerBullet.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyBullet.h"
#include "Block.h"
#include <list>

class GameScene;

class CollisionManager {
public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetPlayerBullet(const std::list<PlayerBullet*>& playerBullet) { playerBullet_ = playerBullet; }
	void SetEnemy(const std::list<Enemy*>& enemy) { enemy_ = enemy; }
	void SetEnemyBullet(const std::list<EnemyBullet*>& enemyBullet) { enemyBullet_ = enemyBullet; }
	void SetBlock(const std::list<Block*>& block) { block_ = block; }

	// 当たり判定を必要とするObjectをまとめてセットする
	void SetGameObject(
	    Player* player, const std::list<PlayerBullet*>& playerBullet,
	    const std::list<Enemy*>& enemy, const std::list<EnemyBullet*>& enemyBullet,
	    const std::list<Block*>& block);

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

	// 壁
	std::list<Block*> block_;
};
