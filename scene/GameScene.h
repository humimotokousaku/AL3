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
#include "FollowEnemy.h"
#include "Skydome/Skydome.h"
#include "Ground/Ground.h"
#include "IScene.h"
#include <memory>
#include <sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public IScene {

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
	void Initialize()override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize() override;

	// 自機を取得
	Player* GetPlayer() { return player_.get(); }
	// 弾リストを取得
	const std::list<PlayerBullet*>& GetPlayerBullets() const { return playerBullets_; }
	// 弾リストを取得
	const std::list<EnemyBullet*>& GetEnemyBullets() const { return enemyBullets_; }
	// 追尾敵リストの取得
	const std::list<FollowEnemy*>& GetFollowEnemys() const { return followEnemys_; }
	// 壁リストを取得
	const std::list<Block*>& GetBlock() const { return block_; }

	int GetScene() { return scene_; }
	void SetScene(int scene) { scene_ = scene; }

	// リストに自弾を登録
	void AddPlayerBullet(PlayerBullet* playerBullet);

	// 敵弾を追加する
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	// csvのデータをもとに壁の座標と大きさを設定
	void SetBlock(Vector3 pos, Vector3 scale);

	// 敵の発生
	void SpawnEnemy(Vector3 pos);

	void SpawnFollowEnemy(Vector3 pos);

	void ResetCSVFile(std::stringstream& stream);

	/// <summary>
	/// csvの読み込み
	/// </summary>
	/// <param name="csvName">ファイル名(パス名も入れる)</param>
	/// <param name="popCommands"></param>
	void LoadCsvData(const char* csvName, std::stringstream& popCommands);

	// csvに書かれている壁の座標と大きさのデータを読む
	void UpdateBlockPopCommands();

	// csvに書かれている敵の座標と発生する時間のデータを読む
	void UpdateEnemyPopCommands();

		// csvに書かれている敵の座標と発生する時間のデータを読む
	void UpdateFollowEnemyPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// 操作説明用の画像
	Sprite* L_button_;
	Sprite* R_button_;
	// 
	Sprite* startButton_;

	enum GUIDETEXT {
		ALLKILL,
		STEP,
		SHOT
	};
	Sprite* guideText_[3];

	enum SCENETEXT {
		TITLE_TEXT,
		GAMEOVER_TEXT,
		CLEAR_TEXT
	};
	Sprite* sceneStateText_[3];

	bool isAllKillText_;
	int guideTextFrame_;

	int clearCount_;

	Sprite* backGround_;

	// csvデータ

	// ブロック
	std::stringstream blockPopCommands_;
	// 敵
	std::stringstream enemyPopCommands_;
	// 追尾敵
	std::stringstream followEnemyPopCommands_;

	// 音データ
	uint32_t damageSound_ = 0;

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
	std::unique_ptr<Model> modelBullet_;

	// 敵
	std::unique_ptr<Model> modelEnemy_;
	// 敵弾
	std::unique_ptr<Model> modelBulletEnemy_;

	// 自キャラ
	std::unique_ptr<Player> player_;
	// 自弾
	std::list<PlayerBullet*> playerBullets_;

	// 敵
	std::list<Enemy*> enemy_;
	// 敵が発生待機中か
	bool isWait_[2] = {false, false};
	// 敵が発生するまでの時間
	int32_t waitTime_[2] = {0, 0};
	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;
	// 追尾敵
	std::list<FollowEnemy*> followEnemys_;

	// カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	// 衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	enum SCENE {
		TITLE_SCENE,
		GAME_SCENE,
		GAMEOVER_SCENE,
		CLEAR_SCENE
	};
	int scene_;
};
