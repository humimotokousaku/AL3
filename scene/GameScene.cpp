#include "GameScene.h"
#include "AxisIndicator.h"
#include "Collision/CollisionManager.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	// 3Dモデル
	delete model_;
	// 自キャラの解放
	delete player_;
	// enemyの解放
	delete enemy_;
	// 衝突マネージャーの解放
	delete collisionManager_;
	// デバッグカメラの解放
	delete debugCamera_;
	// 3Dモデル
	delete modelSkydome_;
	// 天球
	delete skydome_;
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

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	debugCamera_->SetFarZ(1000); // 今回は天球の大きさ的に大丈夫なので初期値
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, playerTexture_);

	// enemyの生成
	enemy_ = new Enemy();
	enemy_->SetPlayer(player_);
	// enemyの初期化
	enemy_->Initialize(model_, Vector3(3, 3, 50));

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, {0, 0, 0});

	// 衝突マネージャーの生成
	collisionManager_ = new CollisionManager();
	collisionManager_->Initialize(player_, enemy_);
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();

	// enemyの更新
	if (enemy_) {
		enemy_->Update();
	}

	// 天球
	skydome_->Update();

	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions();

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_C)) {
		if (!isDebugCameraActive_) {
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}
#endif
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}
	// 軸方向の表示を有効
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
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
	//自機
	player_->Draw(viewProjection_);

	// 敵
	if (enemy_) {
		enemy_->Draw(viewProjection_);
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