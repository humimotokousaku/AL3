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
	player_->Initialize(model_, playerTexture_,playerPosition);
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
        {10, 10, 0},
        {10, 15, 0},
        {20, 15, 0},
        {20, 0,  0},
        {30, 0,  0}
    };

	
}

Vector3 GameScene::CatmullRomSpline(std::vector<Vector3> controlPoints, float t) {
	int numPoints = controlPoints.size();
	int segment = static_cast<int>(std::floor(t * (numPoints - 1))); // 現在のセグメントを決定
	float segmentT = t * (numPoints - 1) - segment; // セグメント内のパラメータtを計算

	// 制御点のインデックスを計算
	int p0 = (segment - 1 + numPoints) % numPoints;
	int p1 = (segment + 0) % numPoints;
	int p2 = (segment + 1) % numPoints;
	int p3 = (segment + 2) % numPoints;

	Vector3 result{};
	result.x = 0.5f * ((2 * controlPoints[p1].x) +
	                   (-controlPoints[p0].x + controlPoints[p2].x) * segmentT +
	                   (2 * controlPoints[p0].x - 5 * controlPoints[p1].x +
	                    4 * controlPoints[p2].x - controlPoints[p3].x) *
	                       segmentT * segmentT +
	                   (-controlPoints[p0].x + 3 * controlPoints[p1].x - 3 * controlPoints[p2].x +
	                    controlPoints[p3].x) *
	                       segmentT * segmentT * segmentT);
	result.y = 0.5f * ((2 * controlPoints[p1].y) +
	                   (-controlPoints[p0].y + controlPoints[p2].y) * segmentT +
	                   (2 * controlPoints[p0].y - 5 * controlPoints[p1].y +
	                    4 * controlPoints[p2].y - controlPoints[p3].y) *
	                       segmentT * segmentT +
	                   (-controlPoints[p0].y + 3 * controlPoints[p1].y - 3 * controlPoints[p2].y +
	                    controlPoints[p3].y) *
	                       segmentT * segmentT * segmentT);
	result.z = 0.5f * ((2 * controlPoints[p1].z) +
	                   (-controlPoints[p0].z + controlPoints[p2].z) * segmentT +
	                   (2 * controlPoints[p0].z - 5 * controlPoints[p1].z +
	                    4 * controlPoints[p2].z - controlPoints[p3].z) *
	                       segmentT * segmentT +
	                   (-controlPoints[p0].z + 3 * controlPoints[p1].z - 3 * controlPoints[p2].z +
	                    controlPoints[p3].z) *
	                       segmentT * segmentT * segmentT);

	return result;
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
	// 線分で描画する用の頂点リスト
	std::vector<Vector3> posintsDrawing;
	// 線分の数
	const size_t segmentCount = 100;
	// 線分の数+1個分の頂点座標の計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomSpline(controlPoints_, t);
		// 描画用頂点リストに追加
		posintsDrawing.push_back(pos);
	}

	for (size_t i = 0; i < segmentCount; i++) {
		primitiveDrawer_->DrawLine3d(
		    posintsDrawing[i], posintsDrawing[i + 1], Vector4{1.0f, 1.0f, 1.0f, 1.0f});
	}
	
	// 自機
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