#pragma once
#include "Input.h"
#include "Model.h"
#include "Vector4.h"
#include "WorldTransform.h"

    class Player {
public:
	Player();
	~Player();

#pragma region Matrix

	// X軸回転行列
	Matrix4x4 MakeRotateXMatrix(float radius);
	// Y軸回転行列
	Matrix4x4 MakeRotateYMatrix(float radius);
	// Z軸回転行列
	Matrix4x4 MakeRotateZMatrix(float radius);

	// 行列の積
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	// 平行移動行列
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

	// 拡大縮小行列
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	// 座標変換
	Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	// 3次元アフィン変換行列
	Matrix4x4
	    MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

#pragma endregion

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection& viewProjection);

private:
	// キーボード入力
	Input* input_ = nullptr;

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t playerTexture_ = 0u;
};