#include "../framework.h"
#include "../framework/vn_environment.h"
#include "../public/CharacterBase.h"

CharacterBase::CharacterBase()
    :m_collision(XMVectorSet(1.0f, 1.0f, 1.0f, 0), XMVectorSet(0, 0, 0, 0))
{

}
// --- ƒ‚ƒfƒ‹ ---
void CharacterBase::SetModel(vnCharacter* model)
{
    m_pModel = model;

}

vnCharacter* CharacterBase::GetModel() const
{
    return m_pModel;
}

// --- •¨— ---
RigidbodyComponent& CharacterBase::GetRigidbody()
{
    return m_rigidBody;
}

const RigidbodyComponent& CharacterBase::GetRigidbody() const
{
    return m_rigidBody;
}
CollisionComponent& CharacterBase::GetCollision()
{
    return m_collision;
}

const CollisionComponent& CharacterBase::GetCollision() const
{
    return m_collision;
}

// --- HP ---
void CharacterBase::SetMaxHp(int maxHP)
{
    m_maxHP = maxHP;
    m_currentHP = m_maxHP;
}

int CharacterBase::GetMaxHp() const
{
    return m_maxHP;
}

int CharacterBase::GetCurrentHp() const
{
    return m_currentHP;
}

void CharacterBase::AddHP(int value)
{
    m_currentHP += value;

    if (m_currentHP > m_maxHP)
        m_currentHP = m_maxHP;
    if (m_currentHP < 0)
        m_currentHP = 0;
}

void CharacterBase::Damage(int value)
{
    if (value > 0)
        AddHP(-value);
}

bool CharacterBase::IsDead() const
{
    return m_currentHP <= 0;
}


// --- Œü‚« ---
void CharacterBase::SetMoveDirection(const XMVECTOR& dir)
{
    m_moveDir = dir;
}
XMVECTOR CharacterBase::GetMoveDir()const
{
    return m_moveDir;
}