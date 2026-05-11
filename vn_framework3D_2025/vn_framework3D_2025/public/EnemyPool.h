//--------------------------------------------------------------//
//	"EnemyClass.h"												//
//		敵クラス												//
//													2025/02/09	//
//														Oya  	//
//--------------------------------------------------------------//
#pragma once
#include"../public/RigidbodyComponent.h"

class EnemyPool
{
public:

	//インスタンス取得用の静的関数（敵のリーダーを探すため）
	static EnemyPool& GetInstance()
	{
		static EnemyPool instance;
		return instance;
	}
	EnemyPool(const EnemyPool&) = delete;
	EnemyPool& operator=(const EnemyPool&) = delete;
	~EnemyPool();

	//リーダーを探す
	NewEnemyClass* FindClosestLeader(NewEnemyClass* requester, float radius);

	// --- 生成したやつを追加 ---
	void AddEnemy(NewEnemyClass* enemy);

	// --- 更新 ---
	void Update(float deltaTime);

	// --- スポーン命令 ---
	void Spawn(const XMVECTOR& position/*,int globalLimit*/);

	// --- 非アクティブ敵取得 ---
	NewEnemyClass* GetInactiveEnemy();

	// --- アクティブ数取得 ---
	int GetActiveCount()const;

	// --- 生成した数返す ---
	std::vector<NewEnemyClass*>& GetEnemies() { return _enemies; }

	// --- プレイヤーの位置をセット ---
	void SetPlayerPosAll(CharacterBase& pos);

	// --- 出現する敵のロックを解除 ---
	void UnlockEnemyType(NewEnemyClass::EnemyType type);

	// --- 画面上にいる敵を全て非表示 --- 
	void HideAllActiveEnemies();
	void ShowAllEnemies();

	std::vector<std::unique_ptr<NewEnemyClass::GroupData>>& GetLatestGroupData() { return m_groupDatas; }

private:
	EnemyPool();
	std::vector<NewEnemyClass*> _enemies;

	std::vector<std::unique_ptr<NewEnemyClass::GroupData>> m_groupDatas;

	float m_spawnTimer = 0.0f;
	const float SPAWN_INTERVAL = 10.0f; // 0.1秒待機
};