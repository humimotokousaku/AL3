#pragma once
#include "Audio.h"
#include "Block.h"
#include "Camera/FollowCamera.h"
#include "Collision/CollisionManager.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player/Player.h"
#include "Player/PlayerBullet.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyBullet.h"
#include "Skydome/Skydome.h"
#include "Ground/Ground.h"
#include <memory>
#include <sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// 自機を取得
	Player* GetPlayer() { return player_.get(); }
	// 弾リストを取得
	const std::list<PlayerBullet*>& GetPlayerBullets() const { return playerBullets_; }
	// 弾リストを取得
	const std::list<EnemyBullet*>& GetEnemyBullets() const { return enemyBullets_; }
	// 壁リストを取得
	const std::list<Block*>& GetBlock() const { return block_; }

	// リストに自弾を登録
	void AddPlayerBullet(PlayerBullet* playerBullet);

	// 敵弾を追加する
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	// csvのデータをもとに壁の座標と大きさを設定
	void SetBlock(Vector3 pos, Vector3 scale);

	// 敵の発生
	void SpawnEnemy(Vector3 pos);

	/// <summary>
	/// csvの読み込み
	/// </summary>
	/// <param name="csvName">ファイル名(パス名も入れる)</param>
	/// <param name="popCommands"></param>
	void LoadCsvData(const char* csvName, std::stringstream* popCommands);

	// csvに書かれている壁の座標と大きさのデータを読む
	void UpdateBlockPopCommands();

	// csvに書かれている敵の座標と発生する時間のデータを読む
	//void UpdateEnemyPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// csvデータ

	// ブロック
	std::stringstream blockPopCommands_;
	// 敵
	//std::stringstream enemyPopCommands_;

	// 3Dモデルデータ

	// 天球
	std::unique_ptr<Model> modelSkydome_;
	std::unique_ptr<Skydome> skydome_;

	// 地面
	std::unique_ptr<Model> modelGround_;
	std::unique_ptr<Ground> ground_;

	// 壁などの障害物
	std::unique_ptr<Model> modelBlock_;
	std::list<Block*> block_;
	// 壁に当たったら位置をリセット
	bool isResetPos_ = false;

	// 自機
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	std::unique_ptr<Model> modelFighterGun_;
	Model* modelBullet_;

	// 敵
	std::unique_ptr<Model> modelEnemy_;

	// 自キャラ
	std::unique_ptr<Player> player_;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;

	//// 敵
	//std::list<Enemy*> enemy_;
	//// 敵が発生待機中か
	//bool isWait_ = false;
	//// 敵が発生するまでの時間
	//int32_t waitTime_ = 0;
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;

	// カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	// 衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
};
