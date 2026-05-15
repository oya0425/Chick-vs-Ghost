#pragma once
class EnemyGhost:public NewEnemyClass
{
public:
	EnemyGhost();
	~EnemyGhost();

	void OnIdel(float deltaTime,float distance, const XMVECTOR& toPlayer)override;
	void OnRun(float deltaTime, float distance, const XMVECTOR& toPlayer)override;
	void OnDead()override;

	void OnFollow(float deltaTime)override;
	void OnPanic(float deltaTime)override;
	void OnCharge(float deltaTime, const XMVECTOR& toPlayer)override;
	void OnPatrol(float deltaTime, float distance)override;
private:
	float m_escapeTimer = 0;
	// 前回の移動方向（カクつき防止の補完用）
	XMVECTOR m_lastMoveDir = XMVectorZero();

	XMVECTOR GetInFence(XMVECTOR vInput,XMVECTOR myPos);


	// --- リーダーを設定 ---
	void LeaderSet(float searchRadius);

	// --- パトロール中関数 ---
	void CheckSurroundings(float distance);		//範囲攻撃が可能の状態に入ったか
	void StartEscapeTransition(bool can);				//逃げ始める
	void MoveAlongPath(float deltaTime, float distance);//メインの移動処理
};