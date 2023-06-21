#include "Collision/CollisionManager.h"

CollisionManager::~CollisionManager() {

}

void CollisionManager::Initialize(Player* player, Enemy* enemy) {
	player_ = player;
	enemy_ = enemy;
	SetPlayer(player_);
	SetEnemy(enemy_);
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	SetPlayer(player_);
	SetEnemy(enemy_);
	// 衝突フィルタリング
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
	    (colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0) {
		return;
	}

	// コライダーのワールド座標を取得
	colliderA->GetWorldPosition();
	colliderB->GetWorldPosition();

	// 座標AとBの距離を求める
	Vector3 a2b = {
	    colliderA->GetWorldPosition().x - colliderB->GetWorldPosition().x,
	    colliderA->GetWorldPosition().y - colliderB->GetWorldPosition().y,
	    colliderA->GetWorldPosition().z - colliderB->GetWorldPosition().z};
	float a2bR = colliderA->GetRadius() + colliderB->GetRadius();

	// 球と球の交差判定
	if ((a2b.x * a2b.x) + (a2b.y * a2b.y) + (a2b.z * a2b.z) <= (a2bR * a2bR)) {
		// コライダーBの衝突時コールバックを呼び出す
		colliderA->OnCollision();
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision();
	}
}

void CollisionManager::CheckAllCollisions() {

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	// リストのクリア
	colliders_.clear();

	// コライダーをリストに登録
	colliders_.push_back(player_);
	colliders_.push_back(enemy_);

	// 自弾全てについて
	for (PlayerBullet* bullet : playerBullets) {
		colliders_.push_back(bullet);
	}
	// 敵弾全てについて
	for (EnemyBullet* bullet : enemyBullets) {
		colliders_.push_back(bullet);
	}

	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// イテレータBはイテレータAの次の要素から回す(重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			// 当たり判定と応答(フレンドリーファイアしないように設定)
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}