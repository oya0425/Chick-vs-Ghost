#pragma once

/// <summary>
/// 敵同士の当たり判定など、敵の動きの
/// </summary>
class EnemyCollisionManager
{
public:
	//セット
	void SetEnemyPool(EnemyPool* enemyPool) { m_enemyPool = enemyPool; }
	void SetWaveManager(WaveManager* waveManager) { m_waveManager = waveManager; }

	// WAVEクリア時の敵処理
	// 敵の移動・地面判定・引き寄せ判定
	// 敵同士の衝突   
	//の呼び出し
	void UpdateEnemyCollision(NewPlayerClass* player, vnModel* ground);

	// 敵とプレイヤー・弾の衝突　当たったかどうかの確認
	bool UpdateEnemyAttackCollision();

	//敵とプレイヤーの当たり判定
	bool CheckEnemyPlayerCollision(NewEnemyClass* enemy, NewPlayerClass* player);

private:
	EnemyPool* m_enemyPool = nullptr;
	WaveManager* m_waveManager = nullptr;


	// WAVEクリア時の敵処理
	void RemoveEnemiesOnWaveClear();

	// 敵の移動・地面判定・引き寄せ判定
	void UpdateEnemyMovement(NewPlayerClass* player, vnModel* ground);

	// 敵同士の衝突
	void UpdateEnemyEnemyCollision();



};