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

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetPlayerBullets() const { return playerBullets_; }
	// 壁リストを取得
	const std::list<Block*>& GetBlock() const { return block_; }

	Player* GetPlayer() { return player_.get(); }

	void AddPlayerBullet(PlayerBullet* playerBullet);

	// csvのデータをもとに壁の座標と大きさを設定
	void SetBlock(Vector3 pos, Vector3 scale);

	// csvの読み込み
	void LoadBlockPopData();

	// csvに書かれている壁の座標と大きさのデータを読む
	void UpdateBlockPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	std::stringstream blockPopCommands_;

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
	// 自機
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	std::unique_ptr<Model> modelFighterGun_;
	Model* modelBullet_;
	// 自キャラ
	std::unique_ptr<Player> player_;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	// 衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_;

	bool isResetPos_ = false;
};
