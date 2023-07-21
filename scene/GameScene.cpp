#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene(){

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// 3Dモデルの生成
	modelPlayer_.reset(Model::CreateFromOBJ("Player", true));
	// 天球の3Dモデルの生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	// 地面の3Dモデルの生成
	modelGround_.reset(Model::CreateFromOBJ("Ground", true));

	// デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	debugCamera_->SetFarZ(1000); // 今回は天球の大きさ的に大丈夫なので初期値
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get());
	// 天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get(), {0, 0, 0});
	// 地面
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get(), {0, 0, 0});

	// カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransform());

	player_->SetViewProjection(&followCamera_->GetViewProjection());
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
	// 天球
	skydome_->Update();
	// 地面
	ground_->Update();
	viewProjection_.UpdateMatrix();
	// カメラ
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	ImGui::Begin(" ");
	ImGui::Text("L joystick:Move   R joystick:CameraRotation");
	ImGui::End();

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
	player_->Draw(viewProjection_);

	// 天球
	skydome_->Draw(viewProjection_);
	// 地面
	ground_->Draw(viewProjection_);

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
