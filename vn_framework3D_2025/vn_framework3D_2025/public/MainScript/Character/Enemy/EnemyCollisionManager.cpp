#include"../../../../framework.h"	
#include"../../../../framework/vn_environment.h"

void EnemyCollisionManager::UpdateEnemyCollision(NewPlayerClass* player, vnModel* ground)
{

	// WAVEクリア時の敵処理
	RemoveEnemiesOnWaveClear();

	// 敵の移動・地面判定・引き寄せ判定
	UpdateEnemyMovement(player, ground);

	// 敵同士の衝突
	UpdateEnemyEnemyCollision();
}


//======================================================================
// --- Waveクリア時に敵を消す ---
//======================================================================
void EnemyCollisionManager::RemoveEnemiesOnWaveClear()
{
	if (m_enemyPool == nullptr) return;
	if (m_waveManager == nullptr) return;

	if (m_waveManager->GetWaveTimer() > m_waveManager->GetWaveTimeLimit())
	{
		return;
	}
	m_enemyPool->AllEnemyDeSpawn();
}

//======================================================================
// --- 動き（引き寄せられるを含む）--- 
//======================================================================
void EnemyCollisionManager::UpdateEnemyMovement(NewPlayerClass* player, vnModel* ground)
{
	if (m_enemyPool == nullptr)return;

	auto& enemies = m_enemyPool->GetEnemies();

	for (auto enemy : enemies)
	{
		if (!enemy->GetActive())
		{
			continue;
		}

		//========================================
		// 引き寄せ判定
		//========================================
		XMVECTOR enemyPos =
			*enemy->GetModel()->getPosition();

		XMVECTOR toPlayerVec =
			*player->GetModel()->getPosition()
			- enemyPos;

		float dist =
			XMVectorGetX(
				XMVector3LengthSq(toPlayerVec));

		enemy->CheckPullTrigger(
			player->IsPulling(),
			player->GetPullRadius(),
			dist);


		//========================================
		// 移動
		//========================================
		XMVECTOR moveEnemy =
			enemy->GetRigidbody().getMoveDelta();

		enemy->GetModel()->addPosition(&moveEnemy);


		//========================================
		// 地面判定
		//========================================
		Common::OnCollider(
			enemy->GetModel(),
			ground,
			1.0f,
			enemy->GetRigidbody());
	}
}


//======================================================================
// --- 敵と敵の当たり判定 ---
//======================================================================
void EnemyCollisionManager::UpdateEnemyEnemyCollision()
{
	if (m_enemyPool == nullptr)return;

	auto& enemies = m_enemyPool->GetEnemies();

	for (size_t i = 0; i < enemies.size(); ++i)
	{
		NewEnemyClass* a = enemies[i];

		if (!a->GetActive() || !a->GetRigidbody().GetIsGround() || a->IsAttracted())
		{
			continue;
		}

		for (size_t j = i + 1; j < enemies.size(); ++j)
		{
			NewEnemyClass* b = enemies[j];

			if (!b->GetActive() || !b->GetRigidbody().GetIsGround() || b->IsAttracted())
			{
				continue;
			}

			//個体同士が遠ければスキップ
			XMVECTOR diff = *a->GetModel()->getPosition() - *b->GetModel()->getPosition();
			float distSq = XMVectorGetX(XMVector3LengthSq(diff));
			if (distSq > 25.0f) //取りたい距離の２乗
			{
				continue;
			}
			// 同じグループのその他の敵同士
			bool sameGroup = (a->GetGroupID() == b->GetGroupID());

			if (sameGroup)
			{
				//同じ群れ内のリーダーとその他の敵は当たらない
				if (!a->GetIsLeader() && !b->GetIsLeader())
				{
					Common::colliderCtoC(a, b);
					Common::colliderCtoC(b, a);
				}
			}
			else
			{
				if (a->GetIsLeader() && b->GetIsLeader())
				{
					//リーダー同士の衝突判定
					Common::colliderCtoC(a, b);
					Common::colliderCtoC(b, a);
				}
			}
		}
	}
}



//======================================================================
// --- 敵とプレイヤーの当たり判定（当たったかどうか） ---
//======================================================================
bool EnemyCollisionManager::CheckEnemyPlayerCollision(NewEnemyClass* enemy,NewPlayerClass* player)
{
	auto dir =
		Common::colliderStoS(enemy, player);

	return dir != Common::eDirection::None;
}