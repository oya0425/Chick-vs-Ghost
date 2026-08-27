#pragma once
//--------------------------------------
//弾のクラス
//--------------------------------------
class Bullet :public CharacterBase
{
public:
	Bullet();
	~Bullet() = default;
	
	void SetVelocity(XMVECTOR v) { m_velocity = v; }
	XMVECTOR GetVelocity()const { return m_velocity; }

	void SetIsActive(bool active) { m_isActive = active; }

	// --- この弾が生きているか ---
	bool IsActive()const { return m_isActive; }

	// --- 反射最大回数を変更 ---
	void SetMaxBounce(int count) { m_maxBounce = count; }
	int GetCurrentBounce()const { return m_bounceCount; }
	int GetMaxBounce()const { return m_maxBounce; }

	// 反射が必要か（当たり判定）のチェック
	bool IsReflectionRequired(Bullet& bullet);
	void SetIsHitWall(bool isHit) { m_isHitWall = isHit; }
	bool GetIsHitWall()const { return m_isHitWall; }

	// --- 弾の更新 ---
	void Update(float deltaTime)override;
	// --- 弾の発射 ---
	void Shoot(XMVECTOR playerPos, XMVECTOR playerDir, float playerRotY);

	// --- 弾の速度を設定 ---
	void SetSpeed(float multiplier);



private:

	int m_bounceCount = 0;	//現在の反射回数
	//int m_maxBounce = 1;	//最大反射回数
	int m_maxBounce = 100;	//最大反射回数

	XMVECTOR m_velocity = XMVectorZero();
	bool m_isActive =false;

	// --- 壁に当たったか ---
	bool m_isHitWall = false;

	// --- 弾の速度 ---
	float m_currentSpeed	   = 3.0f;	//変化する弾の速度
	//float m_currentSpeed	   = 3.0f;	//変化する弾の速度
	const float m_defaultSpeed = 3.0f;	//デフォルトの速度

};

