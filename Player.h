#pragma once
#include "Input.h"
#include "Model.h"
#include "Vector4.h"
#include "WorldTransform.h"
#include "PlayerBullet.h"
#include <list>

class Player {
public:
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

#pragma region Matrix

	// 行列の加法
	static Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

	// 行列の減法
	static Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

	// X軸回転行列
	static Matrix4x4 MakeRotateXMatrix(float radius);
	// Y軸回転行列
	static Matrix4x4 MakeRotateYMatrix(float radius);
	// Z軸回転行列
	static Matrix4x4 MakeRotateZMatrix(float radius);

	// 行列の積
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	// 平行移動行列
	static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

	// 拡大縮小行列
	static Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	// 座標変換
	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	// 3次元アフィン変換行列
	static Matrix4x4
	    MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

#pragma endregion

	// playerの回転
	void Rotate();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model, uint32_t textureHandle);

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection& viewProjection);

public:
	// キーボード入力
	Input* input_ = nullptr;

	// ImGuiで値を入力する変数
	float* inputFloat3[3] = {
	    &worldTransform_.translation_.x, 
		&worldTransform_.translation_.y,
	    &worldTransform_.translation_.z
	};

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t playerTexture_ = 0u;

	// 弾
	//PlayerBullet* bullet_ = nullptr;
	std::list<PlayerBullet*> bullets_;
};