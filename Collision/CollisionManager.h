#pragma once
#include "Collision/CollisionConfig.h"
#include "Collision/Collider.h"
#include "Player/Player.h"
#include "Player/PlayerBullet.h"
#include "Block.h"
#include <list>

class GameScene;

class CollisionManager {
public:
	void SetPlayer(Player* player) { player_ = player; }
	void SetPlayerBullet(const std::list<PlayerBullet*>& playerBullet) { playerBullet_ = playerBullet; }
	void SetBlock(const std::list<Block*>& block) { block_ = block; }

	// 当たり判定を必要とするObjectをまとめてセットする
	void SetGameObject(
	    Player* player, const std::list<Block*>& block,
	    const std::list<PlayerBullet*>& playerBullet);

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void CheckAllCollisions(GameScene* gameScene);

private:
	// コライダーリスト
	std::list<Collider*> colliders_;

	Player* player_;
	std::list<PlayerBullet*> playerBullet_;

	std::list<Block*> block_;
};
