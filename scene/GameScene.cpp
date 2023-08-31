#include "GameScene.h"
#include "AxisIndicator.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>
#include <iostream>

GameScene::GameScene() {}

GameScene::~GameScene() {
	for (Enemy* enemy : enemy_) {
		delete enemy;
	}
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		delete enemyBullet;
	}
	for (FollowEnemy* followEnemy : followEnemys_) {
		delete followEnemy;
	}
	for (PlayerBullet* playerBullet : playerBullets_) {
		delete playerBullet;
	}
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// csvデータの読み込み
	LoadCsvData("csv/blockPop.csv", blockPopCommands_);
	LoadCsvData("csv/enemyPop.csv", enemyPopCommands_);
	LoadCsvData("csv/followEnemyPop.csv", followEnemyPopCommands_);

	// 操作案内のテクスチャ取得
	uint32_t L_button = TextureManager::Load("Guide_Pad_L.png");
	uint32_t R_button = TextureManager::Load("Guide_Pad_R.png");
	//
	uint32_t startButton = TextureManager::Load("Guide_Pad_A.png");
	//
	uint32_t guideText_AllKill = TextureManager::Load("Guide_AllKill.png");
	uint32_t guideText_Step = TextureManager::Load("Guide_Step.png");
	uint32_t guideText_Shot = TextureManager::Load("Guide_Shot.png");
	// 
	uint32_t sceneText_TitleName = TextureManager::Load("Title_Name.png");
	uint32_t guideText_Gameover = TextureManager::Load("Guide_Gameover.png");
	uint32_t guideText_Clear = TextureManager::Load("Guide_Clear.png");

	// 背景
	uint32_t backGround = TextureManager::Load("backGround.png");

	// スプライト生成
	L_button_ = Sprite::Create(L_button, {40, 20}, {1, 1, 1, 1}, {0.5f, 0.5f});
	R_button_ = Sprite::Create(R_button, {40, 60}, {1, 1, 1, 1}, {0.5f, 0.5f});
	startButton_ = Sprite::Create(startButton, {640, 500}, {1, 1, 1, 1}, {0.5f, 0.5f});
	guideText_[ALLKILL] = Sprite::Create(guideText_AllKill, {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});
	guideText_[STEP] = Sprite::Create(guideText_Step, {110, 25}, {1, 1, 1, 1}, {0.5f, 0.5f});
	guideText_[SHOT] = Sprite::Create(guideText_Shot, {110, 65}, {1, 1, 1, 1}, {0.5f, 0.5f});

	sceneStateText_[TITLE_TEXT] = Sprite::Create(sceneText_TitleName, {640, 260}, {1, 1, 1, 1}, {0.5f, 0.5f});
	sceneStateText_[GAMEOVER_TEXT] = Sprite::Create(guideText_Gameover, {640, 260}, {1, 1, 1, 1}, {0.5f, 0.5f});
	sceneStateText_[CLEAR_TEXT] = Sprite::Create(guideText_Clear, {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});

	// 背景
	backGround_ = Sprite::Create(backGround, {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});

	// 3Dモデルの生成
	modelFighterBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelFighterHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));
	modelFighterGun_.reset(Model::CreateFromOBJ("gun", true));
	// Playerの弾モデルの生成
	modelBullet_.reset(Model::CreateFromOBJ("PlayerBullet", true));

	// 敵のモデル
	modelEnemy_.reset(Model::CreateFromOBJ("enemy", true));
	// 敵弾
	modelBulletEnemy_.reset(Model::Create());

	// 天球の3Dモデルの生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	// 地面の3Dモデルの生成
	modelGround_.reset(Model::CreateFromOBJ("Ground", true));

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = std::make_unique<Player>();
	player_->Initialize(
	    modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(),
	    modelFighterR_arm_.get(), modelFighterGun_.get(), modelBullet_.get());
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

	isAllKillText_ = false;
	guideTextFrame_ = 0;

	clearCount_ = 0;
}

void GameScene::Update() {
	XINPUT_STATE joyState{};
	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}
	switch (scene_) {
	case TITLE_SCENE:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
#pragma region 初期化
			ResetCSVFile(enemyPopCommands_);
			ResetCSVFile(followEnemyPopCommands_);
			Initialize();
			player_->Initialize(
			    modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(),
			    modelFighterR_arm_.get(), modelFighterGun_.get(), modelBullet_.get());
			player_->SetWorldPos({0, 0, 0});
			followCamera_->Initialize();

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
				bullet->SetIsDead(true);
			}
			playerBullets_.clear();
			// 敵の出現するタイミングと座標
			// 敵の削除
			enemy_.remove_if([](Enemy* enemy) {
				if (enemy->isDead()) {
					delete enemy;
					return true;
				}
				return false;
			});
			// enemyの更新
			for (Enemy* enemy : enemy_) {
				enemy->SetIsDead(true);
			}
			enemy_.clear();

			followEnemys_.remove_if([](FollowEnemy* followEnemy) {
				if (followEnemy->isDead()) {
					delete followEnemy;
					return true;
				}
				return false;
			});
			for (FollowEnemy* followEnemy : followEnemys_) {
				followEnemy->SetIsDead(true);
			}
			followEnemys_.clear();

			// 終了した弾を削除
			enemyBullets_.remove_if([](EnemyBullet* bullet) {
				if (bullet->isDead()) {
					delete bullet;
					return true;
				}
				return false;
			});
			// 弾の更新
			for (EnemyBullet* bullet : enemyBullets_) {
				bullet->SetIsDead(true);
			}
			enemyBullets_.clear();
			for (int i = 0; i < 2; i++) {
				waitTime_[i] = 0;
				isWait_[i] = true;
			}
#pragma endregion
			isAllKillText_ = true;
			scene_ = GAME_SCENE;
		}
		break;
	case GAME_SCENE:

		if (isAllKillText_) {
			if (guideTextFrame_ >= 120) {
				isAllKillText_ = false;
				guideTextFrame_ = 0;
			}
			guideTextFrame_++;
		}
		if (guideTextFrame_ <= 1) {
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
			// 敵の出現するタイミングと座標
			UpdateEnemyPopCommands();
			// enemyの更新
			for (Enemy* enemy : enemy_) {
				enemy->Update();
				if (enemy->isDead()) {
					clearCount_++;
				}
			}
			// 敵の削除
			enemy_.remove_if([](Enemy* enemy) {
				if (enemy->isDead()) {
					delete enemy;
					return true;
				}
				return false;
			});

			// 追尾敵
			UpdateFollowEnemyPopCommands();
			for (FollowEnemy* followEnemy : followEnemys_) {
				followEnemy->Update();
				if (followEnemy->isDead()) {
					clearCount_++;
				}
			}
			followEnemys_.remove_if([](FollowEnemy* followEnemy) {
				if (followEnemy->isDead()) {
					delete followEnemy;
					return true;
				}
				return false;
			});

			// 終了した弾を削除
			enemyBullets_.remove_if([](EnemyBullet* bullet) {
				if (bullet->isDead()) {
					delete bullet;
					return true;
				}
				return false;
			});
			// 弾の更新
			for (EnemyBullet* bullet : enemyBullets_) {
				bullet->Update();
			}

			// 天球
			skydome_->Update();
			// 地面
			ground_->Update();

			// 当たり判定を必要とするObjectをまとめてセットする
			collisionManager_->SetGameObject(
			    player_.get(), playerBullets_, enemy_, enemyBullets_, followEnemys_);
			// 衝突マネージャー(当たり判定)
			collisionManager_->CheckAllCollisions(this);

			viewProjection_.UpdateMatrix();
			// カメラ
			followCamera_->Update();
			viewProjection_.matView = followCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;

			viewProjection_.TransferMatrix();

			if (clearCount_ >= 68) {
				scene_ = CLEAR_SCENE;
			}
		}

		break;
	case GAMEOVER_SCENE:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			scene_ = TITLE_SCENE;
		}
		break;

	case CLEAR_SCENE:
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			scene_ = TITLE_SCENE;
		}
		break;
	}
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
	switch (scene_) {
case TITLE_SCENE:
		backGround_->Draw();
	break;
case GAME_SCENE:

	break;
case GAMEOVER_SCENE:
	backGround_->Draw();
	break;

case CLEAR_SCENE:
	backGround_->Draw();
	break;
}

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
	switch (scene_) {
	case TITLE_SCENE:

		break;
	case GAME_SCENE:

		// 自機
		player_->Draw(viewProjection_);
		// 自弾
		for (PlayerBullet* bullet : playerBullets_) {
			bullet->Draw(viewProjection_);
		}
		for (FollowEnemy* followEnemy : followEnemys_) {
			followEnemy->Draw(viewProjection_);
		}

		// 敵
		for (Enemy* enemy : enemy_) {
			enemy->Draw(viewProjection_);
		}
		// 敵弾
		for (EnemyBullet* bullet : enemyBullets_) {
			bullet->Draw(viewProjection_);
		}

		// 天球
		skydome_->Draw(viewProjection_);

		// 地面
		ground_->Draw(viewProjection_);
		break;
	case GAMEOVER_SCENE:

		break;

	case CLEAR_SCENE:

		break;
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
	switch (scene_) {
	case TITLE_SCENE:
		sceneStateText_[TITLE_TEXT]->Draw();
		startButton_->Draw();
		break;
	case GAME_SCENE:
		// L
		L_button_->Draw();
		// R
		R_button_->Draw();
		// 2Dレティクル
		player_->DrawUI();

		///
		/// 文字
		/// 
		// すべて倒せ!!
		if (isAllKillText_) {
			guideText_[ALLKILL]->Draw();
		}
		guideText_[STEP]->Draw();
		guideText_[SHOT]->Draw();
		break;
	case GAMEOVER_SCENE:
		sceneStateText_[GAMEOVER_TEXT]->Draw();
		startButton_->Draw();
		break;

	case CLEAR_SCENE:
		sceneStateText_[CLEAR_TEXT]->Draw();
		startButton_->Draw();
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::Finalize() {

}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::SpawnEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	// 自機の位置をもらう
	enemy->SetPlayer(player_.get());
	// 初期化
	enemy->Initialize(modelEnemy_.get(), modelBulletEnemy_.get(), pos);
	enemy->SetGameScene(this);
	// リストに登録
	enemy_.push_back(enemy);
}

void GameScene::SpawnFollowEnemy(Vector3 pos) {
	FollowEnemy* enemy = new FollowEnemy();
	// 自機の位置をもらう
	enemy->SetPlayer(player_.get());
	// 初期化
	enemy->Initialize(modelEnemy_.get(), pos);
	enemy->SetGameScene(this);
	// リストに登録
	followEnemys_.push_back(enemy);
}

void GameScene::ResetCSVFile(std::stringstream& stream) {
	// ストリームの位置を先頭に移動
	stream.seekg(0, std::ios::beg);
}

void GameScene::LoadCsvData(const char* csvName, std::stringstream& popCommands) {
	// ファイルを開く
	std::ifstream file;
	file.open(csvName);
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	popCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateFollowEnemyPopCommands() {
	// 待機処理
	if (isWait_[1]) {
		waitTime_[1]--;
		if (waitTime_[1] <= 0) {
			// 待機完了
			isWait_[1] = false;
		}
		return;
	}

	// 1桁分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(followEnemyPopCommands_, line)) {
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
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SpawnFollowEnemy(Vector3(x, y, z));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待ち時間
			isWait_[1] = true;
			waitTime_[1] = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateEnemyPopCommands() {
	// 待機処理
	if (isWait_[0]) {
		waitTime_[0]--;
		if (waitTime_[0] <= 0) {
			// 待機完了
			isWait_[0] = false;
		}
		return;
	}

	// 1桁分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands_, line)) {
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
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SpawnEnemy(Vector3(x, y, z));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待ち時間
			isWait_[0] = true;
			waitTime_[0] = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}
