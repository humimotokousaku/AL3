#pragma once
#include <vector>
#include <Model.h>
#include "WorldTransform.h"
#include "ViewProjection.h"

enum ModelParts {
	kModelIndexBody,
	kModelIndexHead,
	kModelIndexL_arm,
	kModelIndexR_arm,
	kModelIndexWeapon
};

class ICharacter {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルデータ配列</param>
	virtual void Initialize(const std::vector<Model*>& models);

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	virtual void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// ワールド変換データ
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

protected:
	// モデルデータ配列
	std::vector<Model*> models_;
	// ワールド変換データ
	WorldTransform worldTransform_;
};
