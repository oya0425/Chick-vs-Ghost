#pragma once
#include"vn_model.h"
#include"RigidbodyComponent.h"


class CharacterBase
{
public:
    CharacterBase();
    virtual ~CharacterBase() = default;

    virtual void Update(float dt) = 0;

    // --- モデル ---
    void SetModel(vnCharacter* pModel);
    vnCharacter* GetModel() const;

    // --- 移動 ---
    void SetMoveDirection(const XMVECTOR& dir);
    XMVECTOR GetMoveDir()const;

    // --- 物理 ---
    RigidbodyComponent& GetRigidbody();
    const RigidbodyComponent& GetRigidbody() const;

    CollisionComponent& GetCollision();
    const CollisionComponent& GetCollision()const;

    virtual float GetEffectiveRadius() {
        // デフォルトは Collision のサイズを返す
        return XMVectorGetX(m_collision.GetSize()) * 0.5f;
    }

    virtual float GetAreaAttackRadius()const { return 0.0f; }
    virtual bool IsAreaAttack()const { return false; }

    // --- HP ---
    void SetMaxHp(int maxHp);
    int GetCurrentHp() const;
    int GetMaxHp() const;
    void AddHP(int addhp);
    void Damage(int value);

    // --- 生死 ---
    bool IsDead()const;



private:

    // モデル
    vnCharacter* m_pModel = nullptr;

    // 向き
    XMVECTOR m_moveDir = XMVectorZero();

    // 物理
    RigidbodyComponent m_rigidBody;
    XMVECTOR m_velocity = XMVectorZero();   //現在の速度
    float m_friction = 0.95f;   //摩擦係数（1.0で減速無し）
    float m_restitution = 0.5f; //反発係数




    // HP
    int m_currentHP = 100;
    int m_maxHP = 100;

protected:
    CollisionComponent m_collision;

};