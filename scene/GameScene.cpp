#include "GameScene.h"
#include "AxisIndicator.h"
#include "Collision/CollisionManager.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	// 3Dモデル
	delete model_;
	// 自キャラの解放
	delete player_;
	// enemyの解放
	for (Enemy* enemy : enemy_) {
		delete enemy;
	}
	// 衝突マネージャーの解放
	delete collisionManager_;
	// 3Dモデル
	delete modelSkydome_;
	// 天球
	delete skydome_;
	// カメラレール
	delete railCamera_;
}

void GameScene::AddEnemeyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	playerTexture_ = TextureManager::Load("sample.png");
	// 3Dモデルの生成
	model_ = Model::Create();
	// 天球の3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// カメラレールの生成
	railCamera_ = new RailCamera();
	// カメラレールの初期化
	railCamera_->Initialize(worldTransform_, worldTransform_.rotation_);

	// 自キャラの生成
	player_ = new Player();
	// 初期位置
	Vector3 playerPosition(0, -2, 10);
	// 自キャラの初期化
	player_->Initialize(model_, playerTexture_, playerPosition);
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	for (int i = 0; i < 2; i++) {
		Enemy* enemy = new Enemy();
		enemy->SetPlayer(player_);
		enemy->Initialize(model_, {float(i) * 3, float(i) * 3, 70});
		enemy->SetGameScene(this);
		enemy_.push_back(enemy);
	}

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, {0, 0, 0});

	// 衝突マネージャーの生成
	collisionManager_ = new CollisionManager();
	collisionManager_->Initialize(player_, enemy_);
}

void GameScene::Update() {
	viewProjection_.UpdateMatrix();
	// 自キャラの更新
	player_->Update();
	// enemyの更新
	for (Enemy* enemy : enemy_) {
		enemy->Update();
	}
	// 敵の削除
	//enemy_.remove_if([](Enemy* enemy) {
	//	if (enemy->isDead()) {
	//		delete enemy;
	//		return true;
	//	}
	//	return false;
	//});

	// 天球
	skydome_->Update();
	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions();

	// デバッグカメラの更新
	railCamera_->Update();
	viewProjection_.matView = railCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;

	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();

	// 軸方向の表示を有効
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 自機
	player_->Draw(viewProjection_);

	// 敵
	for (Enemy* enemy : enemy_) {
		enemy->Draw(viewProjection_);
	}

	// 天球
	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}