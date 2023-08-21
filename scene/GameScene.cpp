#include "GameScene.h"
#include "AxisIndicator.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelBullet_;
	// 自機の弾
	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}
	// 壁
	for (Block* block : block_) {
		delete block;
	}
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	LoadBlockPopData();

	// 3Dモデルの生成
	modelFighterBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelFighterHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));
	modelFighterGun_.reset(Model::CreateFromOBJ("gun", true));

	// Playerの弾モデルの生成
	modelBullet_ = Model::Create();

	// 天球の3Dモデルの生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	// 地面の3Dモデルの生成
	modelGround_.reset(Model::CreateFromOBJ("Ground", true));
	// 壁
	modelBlock_.reset(Model::Create());

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = std::make_unique<Player>();
	player_->Initialize(
	    modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(),
	    modelFighterR_arm_.get(), modelFighterGun_.get(), modelBullet_);
	// 天球
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get(), {0, 0, 0});
	// 地面
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get(), {0, 0, 0});
	player_->SetGameScene(this);

	// カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 追従するオブジェクトのワールドトランスフォームをセット
	followCamera_->SetTarget(&player_->GetWorldTransformBase());
	// 自機に追従カメラのビュープロジェクションをアドレス渡し
	player_->SetViewProjection(&followCamera_->GetViewProjection());

	// 衝突マネージャーの生成
	collisionManager_ = std::make_unique<CollisionManager>();
}

void GameScene::Update() {
	// 敵の出現するタイミングと座標
	UpdateBlockPopCommands();
	// 壁の更新
	for (Block* block : block_) {
		block->Update();
	}
	for (Block* block : block_) {
		if (block->GetIsCollision()) {
			isResetPos_ = true;
		}
	}

	// 自機
	player_->Update();
	// 終了した弾を削除
	playerBullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	// 弾の更新
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Update();
	}

	// 天球
	skydome_->Update();
	// 地面
	ground_->Update();

	// 当たり判定を必要とするObjectをまとめてセットする
	collisionManager_->SetGameObject(player_.get(), block_, playerBullets_);
	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions(this);

	viewProjection_.UpdateMatrix();
	// カメラ
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;

	viewProjection_.TransferMatrix();

	// 軸方向の表示を有効
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定
	AxisIndicator::GetInstance()->SetTargetViewProjection(&followCamera_->GetViewProjection());
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
	// 自弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw(viewProjection_);
	}
	// 壁
	for (Block* block : block_) {
		block->Draw(viewProjection_);
	}
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
	// 2Dレティクル
	player_->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}

void GameScene::SetBlock(Vector3 pos, Vector3 scale) {
	Block* block = new Block();
	// 初期化
	block->Initialize(modelBlock_.get(), pos, scale);
	// リストに登録
	block_.push_back(block);
}

void GameScene::LoadBlockPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("csv/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	blockPopCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateBlockPopCommands() {
	// 1桁分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(blockPopCommands_, line)) {
		// 1桁の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// 座標
			// x
			getline(line_stream, word, ',');
			float posX = (float)std::atof(word.c_str());
			// y
			getline(line_stream, word, ',');
			float posY = (float)std::atof(word.c_str());
			// z
			getline(line_stream, word, ',');
			float posZ = (float)std::atof(word.c_str());

			// スケール
			// x
			getline(line_stream, word, ',');
			float scaleX = (float)std::atof(word.c_str());
			// y
			getline(line_stream, word, ',');
			float scaleY = (float)std::atof(word.c_str());
			// z
			getline(line_stream, word, ',');
			float scaleZ = (float)std::atof(word.c_str());

			// 敵を発生させる
			SetBlock(Vector3(posX, posY, posZ), Vector3(scaleX, scaleY, scaleZ));

			// コマンドループを抜ける
			break;
		}
	}
}
