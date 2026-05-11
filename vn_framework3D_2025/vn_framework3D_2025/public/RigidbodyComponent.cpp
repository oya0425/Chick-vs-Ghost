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

void RigidbodyComponent::Update(float dt)
{
    //１.
    // --- 重力（速度にだけ加算） ---

    if (m_IsUseGravity&&!m_isGround)
    {
        m_verticalVelocity += (m_gravity * 50) * dt;
    }
    else if (m_isGround && m_verticalVelocity < 0)
    {
        m_verticalVelocity = 0.0f;  //地面なら下方向の速度をリセット
    }
    //２.
    // --- 全ての速度を合算 ---
    // 合計＝（入力*倍率）+重力ベクトル+外部からの衝撃
    XMVECTOR gravityVec = XMVectorSet(0, m_verticalVelocity, 0, 0);
    m_moveDelta = (m_baseVelocity * m_speedMultiplier * dt)
                + (gravityVec * dt)
                + (m_externalVelocity * dt);

    //// --- 水平移動（速度） ---
    //XMVECTOR vMove = m_baseVelocity * m_speedMultiplier;

    //// --- 外力を加算 ---
    //vMove += m_externalVelocity;


    //// --- 移動量計算（ここでだけ dt） ---
    //XMVECTOR vY = XMVectorSet(0.0f, m_verticalVelocity, 0.0f, 0.0f);
    //m_moveDelta = (vMove + vY) * dt;

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