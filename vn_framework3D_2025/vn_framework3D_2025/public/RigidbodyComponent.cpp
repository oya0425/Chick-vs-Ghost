#include"../framework.h"
#include"../framework/vn_environment.h"
#include"RigidbodyComponent.h"
RigidbodyComponent::RigidbodyComponent()
{
	m_baseVelocity = XMVectorZero();
	m_speedMultiplier = 1.0f;

	m_verticalVelocity = 0.0f;
	m_gravity = -0.98f;     // 例
	m_IsUseGravity = true;
    m_isGround = false;

    m_externalVelocity = XMVectorZero();

	m_moveDelta = XMVectorZero();
}

void RigidbodyComponent::SetBaseVelocity(const XMVECTOR& v)
{
    m_baseVelocity = v;
}

void RigidbodyComponent::SetSpeedMultiplier(float m)
{
    m_speedMultiplier = m;
}

void RigidbodyComponent::AddExternalVelocity(const XMVECTOR& v)
{
    m_externalVelocity += v;
}

//======================================================================
// --- 物理更新 ---
//======================================================================
void RigidbodyComponent::Update(float dt)
{
    // --- 重力（速度にだけ加算） ---

    if (m_IsUseGravity&&!m_isGround)
    {
        m_verticalVelocity += (m_gravity * 50) * dt;
    }
    else if (m_isGround && m_verticalVelocity < 0)
    {
        m_verticalVelocity = 0.0f;  //地面なら下方向の速度をリセット
    }

    // --- 全ての速度を合算 ---
    // 合計＝（入力*倍率）+重力ベクトル+外部からの衝撃
    XMVECTOR gravityVec = XMVectorSet(0, m_verticalVelocity, 0, 0);
    m_moveDelta = (m_baseVelocity * m_speedMultiplier * dt)
                + (gravityVec * dt)
                + (m_externalVelocity * dt);

    // --- 外力は1フレームで消す ---
    m_externalVelocity = XMVectorZero();
}

void RigidbodyComponent::AddVerticalVelocity(float v)
{
 	m_verticalVelocity += v;
}
void RigidbodyComponent::SetVerticalVelocity(float v)
{
	m_verticalVelocity = v;
}


XMVECTOR RigidbodyComponent::getMoveDelta() const
{
	//return baseVelocity * speedMultiplier;
	return m_moveDelta;
}