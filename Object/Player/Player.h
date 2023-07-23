#pragma once
#include <optional>

#include "Model.h"
#include "Object/ICharacter.h"
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

	// 遷移行動更新
	void BehaviorRootUpdate();
	// 通常行動初期化
	void BehaviorRootInitialize();
	// 攻撃行動初期化
	void BehaviorAttackInitialize();
	// 攻撃行動更新
	void BehaviorAttackUpdate();

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
	// 振るまい
	enum class Behavior {
		kRoot,	// 通常状態
		kAttack // 攻撃中
	};
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// 攻撃フレーム
	int attackAnimationFrame;

	// ワールド変換データ

	// 身体
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	// 武器
	WorldTransform worldTransformHammer_;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	const uint16_t kMaxMoveModelParts = 2;
	// 浮遊ギミックの媒介変数
	float floatingParameter_[2];
};