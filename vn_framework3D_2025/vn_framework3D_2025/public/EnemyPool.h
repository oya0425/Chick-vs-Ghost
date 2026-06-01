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


	// --- 表示オン・オフ ---
	enum class eDisplayMode
	{
		None, // 初期状態用
		AllOff,	//全敵の文字の表示オフ
		LeaderOnly,
		OthersOnly,
		AllOn,
	};

	//棒グラフ
	struct UIBar
	{
		vnSprite* pBackBlack = nullptr;
		vnSprite* pBack = nullptr;
		vnSprite* pFront = nullptr;
	};




	//リーダーを探す
	NewEnemyClass* FindClosestLeader(NewEnemyClass* requester, float radius);

	// --- 生成したやつを追加 ---
	void AddEnemy(NewEnemyClass* enemy);

	// --- 更新 ---
	void Update(float deltaTime);

	// --- スポーン命令 ---
	void Spawn(const XMVECTOR& position/*,int globalLimit*/);
	void Spawn(const XMVECTOR& position,int currentWave,int maxWave);

	// --- 非アクティブ敵取得 ---
	NewEnemyClass* GetInactiveEnemy();
	NewEnemyClass* GetInactiveBoss();	//ボスだけ取ってくる

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
	std::unique_ptr<NewEnemyClass::GroupData>& GetBossGroupData() { return m_bossGroupData; }


	//自身のグループIDをもとに学習データを持ってくる　
	NewEnemyClass::GroupData* GetGroupData(int id);

	eDisplayMode GetDisplayMode()const { return m_displayMode; }
	void DrawGroupDebugInfo();
	void DrawGroupDebugArrow();
	void ChangeDebugGroupIndex(int direction);	//表示する群れを変更
	void DebugPause();

	void ReStartEnemyGroupData();

	// --- ボスの学習データを入れる ---
	void SetBossData();
	void DrawBossDebugInfo();
	void DebugBossPause();


	//棒グラフ設定
	void SetMeleeBar(const UIBar& bar);
	void SetRangeBar(const UIBar& bar);
	void SetPullBar(const UIBar& bar);
	const UIBar& GetMeleeBar() const { return m_meleeBar; }
	const UIBar& GetRangeBar() const { return m_rangeBar; }
	const UIBar& GetPullBar() const { return m_pullBar; }

	void ShowHideBar(bool isShow);


private:
	EnemyPool();
	std::vector<NewEnemyClass*> _enemies;

	std::vector<std::unique_ptr<NewEnemyClass::GroupData>> m_groupDatas;

	std::unique_ptr<NewEnemyClass::GroupData>m_bossGroupData;

	float m_spawnTimer = 0.0f;
	const float SPAWN_INTERVAL = 0.1f; // 0.1秒待機

	eDisplayMode m_displayMode = eDisplayMode::LeaderOnly;

	void DebugSetting();
	void ChangeDisplayMode(eDisplayMode nextMode);

	int	m_debugGroupIndex = 0;//表示する



	//棒グラフ
	UIBar m_meleeBar;
	UIBar m_rangeBar;
	UIBar m_pullBar;

};