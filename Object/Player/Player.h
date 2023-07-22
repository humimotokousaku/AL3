#pragma once
#include "Object/ICharacter.h"
#include "Model.h"
#include "WorldTransform.h"

class Player : public ICharacter {
public:
	Player();
	~Player();
	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(const std::vector<Model*>& models) override;

	/// <summary>
	/// 更新
	/// <summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(const ViewProjection& viewProjection) override;

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();

	// 浮遊ギミック更新
	void UpdateFloatingGimmick();

	// 自機のワールド座標
	Vector3 GetWorldPosition();

	const WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	const WorldTransform& GetWorldTransformBase() { return worldTransform_; }

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	// パーツの親子関係
	void SetParent(const WorldTransform* parent);

private:
	// ワールド変換データ
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	const uint16_t kMaxMoveModelParts = 2;
	// 浮遊ギミックの媒介変数
	float floatingParameter_[2];
};