#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Object/Player/Player.h"
#include "Object/Enemy/Enemy.h"
#include <memory>
#include "Skydome/Skydome.h"
#include "Ground/Ground.h"
#include "DebugCamera.h"
#include "Camera/FollowCamera.h"

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

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// 3Dモデルデータ
	std::unique_ptr<Model> modelPlayer_;
	// 天球の3Dモデル
	std::unique_ptr<Model> modelSkydome_;
	std::unique_ptr<Skydome> skydome_;
	// 地面の3Dモデル
	std::unique_ptr<Model> modelGround_;
	std::unique_ptr<Ground> ground_;
	// 自機の3Dモデル
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	// 敵の3Dモデル
	std::unique_ptr<Model> modelEnemy_;
	
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 自キャラ
	std::unique_ptr<Player> player_;

	// 敵
	std::unique_ptr<Enemy> enemy_;

	// カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
};
