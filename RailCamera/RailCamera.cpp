#pragma once
#include "RailCamera/RailCamera.h"
#include "math/MyMatrix.h"
#include "ImGuiManager.h"

void RailCamera::Initialize(WorldTransform worldTransform, const Vector3& radian) {
	// ワールドトランスフォームの初期設定
	// 引数で受け取った初期座標をセット
	worldTransform_ = worldTransform;
	worldTransform_.translation_.z = -10;

	// 引数で受け取った初期座標をセット
	worldTransform_.rotation_ = radian;
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void RailCamera::Update() {
	Vector3 radian{0.0f, 0.001f, 0.0f};
	// 回転処理
	//worldTransform_.rotation_ = Add(worldTransform_.rotation_, radian);

	// 行列の更新
	worldTransform_.UpdateMatrix();
	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	//viewProjection_.TransferMatrix();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.1f);
	ImGui::End();
}