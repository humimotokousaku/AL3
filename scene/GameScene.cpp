#include "GameScene.h"
#include "AxisIndicator.h"
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
	// デバッグカメラの解放
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	playerTexture_ = TextureManager::Load("sample.png");
	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, playerTexture_);

	// enemyの生成
	enemy_ = new Enemy();
	enemy_->SetPlayer(player_);
	// enemyの初期化
	enemy_->Initialize(model_, Vector3(3,3,50));

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::CheckAllCollisions() {
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定

	// 判定対象AとBの座標
	Vector3 playerPos{};
	Vector3 enemyBulletPos{};

	// 自キャラの座標
	playerPos = player_->GetWorldPosition();

	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		enemyBulletPos = bullet->GetWorldPosition();

		// 座標AとBの距離を求める
		Vector3 p2eB = {
			playerPos.x - enemyBulletPos.x,
			playerPos.y - enemyBulletPos.y,
			playerPos.z - enemyBulletPos.z
		};
		const float kRadius = 2.0f;
		float p2eR = kRadius + kRadius;

		// 球と球の交差判定
		if ((p2eB.x * p2eB.x) + (p2eB.y * p2eB.y) + (p2eB.z * p2eB.z) <= p2eR * p2eR) {
			// 自キャラの衝突時にコールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時にコールバックを呼び出す
			bullet->OnCollision();
		}
	}

	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定

	Vector3 playerBulletPos{};
	Vector3 enemyPos = enemy_->GetWorldPosition();

	for (PlayerBullet* bullet : playerBullets) {
		// 自弾の座標
		playerBulletPos = bullet->GetWorldPosition();

		// 座標AとBの距離を求める
		Vector3 pB2e = {
		    playerBulletPos.x - enemyPos.x, playerBulletPos.y - enemyPos.y,
		    playerBulletPos.z - enemyPos.z};
		const float kRadius = 2.0f;
		float pB2eR = kRadius + kRadius;

		// 球と球の交差判定
		if ((pB2e.x * pB2e.x) + (pB2e.y * pB2e.y) + (pB2e.z * pB2e.z) <= pB2eR * pB2eR) {
			// 敵キャラの衝突時にコールバックを呼び出す
			enemy_->OnCollision();
			// 自弾の衝突時にコールバックを呼び出す
			bullet->OnCollision();
		}
	}

	#pragma endregion

	#pragma region 自弾と敵弾の当たり判定

	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets) {
			playerBulletPos = playerBullet->GetWorldPosition();
			enemyBulletPos = enemyBullet->GetWorldPosition();
			Vector3 pB2eB = {
			    playerBulletPos.x - enemyBulletPos.x, playerBulletPos.y - enemyBulletPos.y,
			    playerBulletPos.z - enemyBulletPos.z};
			const float kRadius = 2.0f;
			float pB2eBR = kRadius + kRadius;
			if ((pB2eB.x * pB2eB.x) + (pB2eB.y * pB2eB.y) + (pB2eB.z * pB2eB.z) <= pB2eBR * pB2eBR) {
				// 敵キャラの衝突時にコールバックを呼び出す
 				enemyBullet->OnCollision();
				// 自弾の衝突時にコールバックを呼び出す
				playerBullet->OnCollision();
			}
		}
	}

	#pragma endregion
}

void GameScene::Update() {
	// 当たり判定のチェック
	CheckAllCollisions();
	// 自キャラの更新
	player_->Update();

	// enemyの更新
	if (enemy_) {
		enemy_->Update();
	}

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
	player_->Draw(viewProjection_);

	if (enemy_) {
		enemy_->Draw(viewProjection_);
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