#pragma once
#include<DirectXMath.h>
using namespace DirectX;

class RigidbodyComponent
{
public:
	RigidbodyComponent();

	//入力由来の基礎速度
	void SetBaseVelocity(const XMVECTOR& v);

	//速度倍率（加速処理用）
	void SetSpeedMultiplier(float m);

	//物理更新
	void Update(float dt);

	//移動量取得
	XMVECTOR getMoveDelta()const;

	// --- 重力 ---
	void SetIsUseGravity(bool enable) { m_IsUseGravity = enable; }
	bool GetIsUseGravity()const { return m_IsUseGravity; }
	void SetGravity(float g) { m_gravity = g; }
	float GetGracity() const{ return m_gravity; }
	void AddVerticalVelocity(float v);
	void SetVerticalVelocity(float v);
	float GetVerticalVelocity()const { return m_verticalVelocity; }

	void SetIsGround(bool ground) { m_isGround = ground; }
	bool GetIsGround()const { return m_isGround;}

	void AddExternalVelocity(const XMVECTOR& v);

private:
	// --- 移動 ---
	XMVECTOR m_baseVelocity;	//入力ベース
	float m_speedMultiplier;	//1.0f通常

	// --- 重力 ---
	float m_verticalVelocity = 0.0f;          // Y方向速度
	float m_gravity = -9.8f;                  // m/s^2
	bool m_IsUseGravity = true;

	XMVECTOR m_externalVelocity; //外部からの衝撃・跳ね返り（物理演算用）
	// --- 物理特性 ---
	float m_restitution = 0.5f;  // 反発係数
	float m_friction = 0.9f;     // 摩擦係数

	XMVECTOR m_moveDelta = XMVectorZero();

	bool m_isGround;
};