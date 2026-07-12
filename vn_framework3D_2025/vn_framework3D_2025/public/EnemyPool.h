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

	//？マークと吹き出しの表示
	struct UIQuestionExplain
	{
		vnSprite* pQuestionIcon;  // ？マークの画像
		vnSprite* pBalloonBg;     // 吹き出しの背景画像

		// 表示するテキスト情報
		const WCHAR* explainText; // 表示したい説明文（L"：特攻確率に加算" など）
		DWORD textColor;       // 文字の色

		// 座標とサイズ（当たり判定や描画に使用）
		float qX, qY;             // ？マークの中心座標
		float qW, qH;             // ？マークの幅と高さ（当たり判定のサイズ）

		float textX, textY;       // テキストを表示する座標

		// 状態管理フラグ
		bool isHovered;           // 今マウスが？マークの上にあるか
		bool isShowExplain;       // 今説明（吹き出しと文字）を表示中か
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
	NewEnemyClass* GetInactiveFinalWaveEnemy();	//最終WAVEの時にリーダー以外を取ってくる


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

	void ShowHideUI(bool isShow);

	//ボタンの画像をセット
	void SetImageTab(vnSprite* sprite) { m_ImageTab = sprite; }
	void SetImageA(vnSprite* sprite) { m_ImageA = sprite; }
	void SetImageD(vnSprite* sprite) { m_ImageD = sprite; }
	vnSprite* GetImageTab()const { return m_ImageTab; }
	vnSprite* GetImageA()const { return m_ImageA; }
	vnSprite* GetImageD()const { return m_ImageD; }
	
	//スラッシュの画像
	void SetImageSlash(vnSprite* sprite) { m_ImageSlash = sprite; }
	vnSprite* GetImageSlash()const { return m_ImageSlash; }

	//お化けの画像
	void SetImageGhost(vnSprite* sprite) { m_ImageGhost = sprite; }
	vnSprite* GetImageGhost()const { return m_ImageGhost; }

	//？マークと吹き出し
	void SetMeleeQus(UIQuestionExplain ui) { m_meleeQus = ui; }
	void SetRangeQus(UIQuestionExplain ui) { m_rangeQus = ui; };
	void SetPullQus(UIQuestionExplain ui) { m_pullQus = ui; };

	//？マークとの当たり判定用
	bool IsMouseOver(const UIQuestionExplain& ui, int mx, int my);

	//？マーク、吹き出しの位置設定
	void SetQuestionUIPos(UIQuestionExplain& ui, float qX, float qY);

	//吹き出しの表示
	void UpdateQuestionHover(UIQuestionExplain& ui, int mx, int my);

	//デリート用
	void ReleaseQuestionUI(vnScene* scene);
	
	//UIの状態のリセット
	void ResetQuestionUI();

	//soundmanagerセット
	void SetSoundManager(SoundManager* soundManager) { m_soundManager = soundManager; }

private:
	EnemyPool();
	std::vector<NewEnemyClass*> _enemies;

	std::vector<std::unique_ptr<NewEnemyClass::GroupData>> m_groupDatas;

	std::unique_ptr<NewEnemyClass::GroupData>m_bossGroupData;

	float m_spawnTimer = 0.0f;
	const float SPAWN_INTERVAL = 0.1f; // 0.1秒待機

	eDisplayMode m_displayMode = eDisplayMode::AllOff;

	void DebugSetting();
	void ChangeDisplayMode(eDisplayMode nextMode);

	int	m_debugGroupIndex = 0;//表示する


	//音
	SoundManager* m_soundManager;

	//棒グラフ
	UIBar m_meleeBar;
	UIBar m_rangeBar;
	UIBar m_pullBar;
	//棒グラフの更新
	void UpdateUIBarHelper(UIBar ui, float currentVal, float maxVal, float leftEdge, float maxW, float posY, XMVECTOR color);

	void UpdateAndRenderQuestionUI(float baseX, float baseY, float lineYPitch,
		UIQuestionExplain& melee, UIQuestionExplain& range, UIQuestionExplain& pull);



	//ボタンの画像
	vnSprite* m_ImageTab;
	vnSprite* m_ImageA;
	vnSprite* m_ImageD;

	//スラッシュの画像
	vnSprite* m_ImageSlash;

	//お化けの画像
	vnSprite* m_ImageGhost;

	//？マークと吹き出し
	UIQuestionExplain m_meleeQus;
	UIQuestionExplain m_rangeQus;
	UIQuestionExplain m_pullQus;
	float m_balloonScale = 0.0f; // 吹き出しの現在のスケール（最初は0）

	//ボスWAVEの表示切り替え
	bool m_isFinalWave = false;

};