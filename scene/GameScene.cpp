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
	delete enemy_;
	// 衝突マネージャーの解放
	delete collisionManager_;
	// 3Dモデル
	delete modelSkydome_;
	// 天球
	delete skydome_;
	// カメラレール
	delete railCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	primitiveDrawer_ = PrimitiveDrawer::GetInstance();

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

	// enemyの生成
	enemy_ = new Enemy();
	enemy_->SetPlayer(player_);
	// enemyの初期化
	enemy_->Initialize(model_, Vector3(3, 3, 70));

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, {0, 0, 0});

	// 衝突マネージャーの生成
	collisionManager_ = new CollisionManager();
	collisionManager_->Initialize(player_, enemy_);

	// スプライン曲線制御点（通過点）の初期化
	controlPoints_ = {
	    {0,  0,  0},
        {10, 10, 10},
        {10, 15, 0},
        {20, 15, 20},
        {20, 0,  0},
        {30, 0,  -10}
    };

	// 3Dライン
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	targetT_ = 1.0f / segmentCount;
}

Vector3 GameScene::CatmullRomSpline(const std::vector<Vector3>& controlPoints, float t) {
	int n = (int)controlPoints.size();
	int segment = static_cast<int>(t * (n - 1));
	float tSegment = t * (n - 1) - segment;

	Vector3 p0 = controlPoints[segment > 0 ? segment - 1 : 0];
	Vector3 p1 = controlPoints[segment];
	Vector3 p2 = controlPoints[segment < n - 1 ? segment + 1 : n - 1];
	Vector3 p3 = controlPoints[segment < n - 2 ? segment + 2 : n - 1];

	Vector3 interpolatedPoint;
	interpolatedPoint.x =
	    0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * tSegment +
	            (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * (tSegment * tSegment) +
	            (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * (tSegment * tSegment * tSegment));
	interpolatedPoint.y =
	    0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * tSegment +
	            (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * (tSegment * tSegment) +
	            (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * (tSegment * tSegment * tSegment));
	interpolatedPoint.z =
	    0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * tSegment +
	            (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * (tSegment * tSegment) +
	            (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * (tSegment * tSegment * tSegment));

	return interpolatedPoint;
}

void GameScene::UpdatePlayerPosition(float t) {
	Vector3 cameraPosition{};
	// Catmull-Romスプライン関数で補間された位置を取得
	cameraPosition = CatmullRomSpline(controlPoints_, t);
	railCamera_->SetTranslation(cameraPosition);
}

void GameScene::Update() {
	viewProjection_.UpdateMatrix();
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

	// 線分の数+1個分の頂点座標の計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		
		Vector3 pos = CatmullRomSpline(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing_.push_back(pos);
	}
	
	// カメラの移動
	if (t_ < 0.99f) {
		t_ += 1.0f / segmentCount / 10;
	} else {
		t_ = 0.99f;
	}
	if (targetT_ < 0.99f) {
		targetT_ += 1.0f / segmentCount / 10;
	} else {
		targetT_ = 1.0f;
	}
	target_ = CatmullRomSpline(controlPoints_, targetT_);
	UpdatePlayerPosition(t_);

	// デバッグカメラの更新
	railCamera_->Update(target_);
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
	if (enemy_) {
		enemy_->Draw(viewProjection_);
	}

	// 天球
	skydome_->Draw(viewProjection_);

	// 3Dライン
	for (int i = 0; i < segmentCount - 1; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(
		    pointsDrawing_[i], pointsDrawing_[i + 1], {1.0f, 0.0f, 0.0f, 1.0f});
	}

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