#pragma once
class EnemyMushroomMon:public NewEnemyClass
{
public:
	EnemyMushroomMon();
	~EnemyMushroomMon();

	void OnIdel(float deltaTime,float distance, const XMVECTOR& toPlayer)override;
	void OnRun(float deltaTime, float distance, const XMVECTOR& toPlayer)override;
	void OnDead()override;

	void OnFollow(float deltaTime)override;
	void OnPanic(float deltaTime)override;
	void OnCharge(float deltaTime, const XMVECTOR& toPlayer)override;
	void OnPatrol(float deltaTime, float distance)override;

	
private:
	float m_timer			 = 0;		//動く・止まるの切り替え用タイマー
	bool m_isMoving			 = false;	//今動いているか




};