#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "PrimitiveDrawer.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player/Player.h"
#include "Enemy/Enemy.h"
#include "Collision/Collider.h"
#include "Collision/CollisionManager.h"
#include "Skydome.h"
#include "RailCamera/RailCamera.h"

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

	Vector3 CatmullRomSpline(std::vector<Vector3> controlPoints, float t);

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

	// テクスチャハンドル
	uint32_t playerTexture_ = 0;
	// 3Dモデルデータ
	Model* model_ = nullptr;

	// 天球の3Dモデル
	Model* modelSkydome_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 自キャラ
	Player* player_ = nullptr;
	// enemy
	Enemy* enemy_ = nullptr;
	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// 天球
	Skydome* skydome_ = nullptr;
	// カメラレール
	RailCamera* railCamera_ = nullptr;

	PrimitiveDrawer* primitiveDrawer_;

	// スプライン曲線制御点（通過点）
	std::vector<Vector3> controlPoints_;

	// ImGuiで値を入力する変数
	float inputFloat[3] = {0, 0, 0};
};