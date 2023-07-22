#pragma once
#include "Object/ICharacter.h"
#include "Model.h"
#include "WorldTransform.h"

class Enemy : public ICharacter{
public:
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
};
