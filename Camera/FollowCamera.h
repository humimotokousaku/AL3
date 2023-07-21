#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"

class FollowCamera {
public:
	void SetTarget(const WorldTransform* target) { target_ = target; }

	void Initialize();

	void Update();

private:
	ViewProjection viewProjection_;
	const WorldTransform* target_ = nullptr;
};