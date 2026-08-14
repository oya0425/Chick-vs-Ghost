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
    virtual bool IsAreaAttack()const { return false; }  //範囲攻撃中か
    virtual bool CanAreaAttack()const { return false; }  //範囲攻撃を撃てるか
    // --- HP ---
    void SetMaxHp(float maxHp);
    void SetCurrentHP(float currentHP);
    float GetCurrentHp() const;
    float GetMaxHp() const;
    void AddHP(float addhp,bool isTutorial);
    void Damage(float value, bool isTutorial);
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
    float m_currentHP = 100;
    float m_maxHP = 100;

protected:
    CollisionComponent m_collision;

};