#pragma once
#include <memory>
#include"../object/bear.h"
#define AABB_MAX (64)
//
#define FENCE_NUM_MAIN (40)			//フェンスモデルの数
#define TREE_NUM (10)
constexpr float GROUND_OFFSET = 0.1f;

//#define FENCE_RADIUS (40)	//フェンスを配置する円周の半径
// --- UI用アセットパスの定義リスト（外に出す） ---
namespace {
	const wchar_t* ASSET_PATHS[] = {
		L"data/image/IMG_E6067.png",   // ID 0
		L"data/image/IMG_E6067.png",   // ID 1
		L"data/image/IMG_E6067.png",   // ID 2
		L"data/image/IMG_E6067.png",  // ID 3
	};
	// 要素数を自動計算しておく
	const int ASSET_COUNT = sizeof(ASSET_PATHS) / sizeof(ASSET_PATHS[0]);
}
struct UpgradeUIResource
{
	const wchar_t* framePath;
	const wchar_t* backGroundPath;
	const wchar_t* mainPath;
};


class SceneMain : public vnScene
{
private:
	// --- 列挙型定義 ---
	enum eDirection 
	{
		None,
		X_Pos,
		X_Neg,
		Y_Pos,
		Y_Neg,
		Z_Pos,
		Z_Neg
	};
	enum GameState 
	{ 
		IdelPlay,
		Play,
		LevelUp,
		Pause,
		BossPause,	//ボス戦のボス情報
		GameOver,
		GameClear,
		GameFinish,
		TimeStop,
	};

	struct UIColor
	{
		XMVECTOR colorBackBlack;
		XMVECTOR colorBack;
		XMVECTOR colorFront;
		XMVECTOR colorIcon;

	};


	// --- 定数 (constexpr) ---
	static constexpr float COMBO_BASE_TIME = 3.0f;
	static constexpr float COMBO_MIN_TIME = 1.0f;
	static constexpr float COMBO_DECREASE = 0.1f;

	// --- ゲーム全体の状態 ---
	GameState m_gameState;
	float     totalClearTime;
	bool      isTimerActive;
	bool      isWaveClear;
	bool      isGameFinish;
	bool	  m_isBossStage = false;


	// --- プレイヤー関連 ---
	NewPlayerClass* m_pNewPlayer;
	XMVECTOR        startPos;
	vnEmitter* pDustEmitter;	// 移動時の土埃
	vnEmitter* pEmitter;		// 撃破エフェクト用

	Bullet* m_pBullet;
	bool isHittingAnyBlock = false;

	// --- 敵・ウェーブ関連 ---
	EnemyPool* enemyPool;
	//std::unique_ptr<EnemyPool> enemyPool;
	WaveManager* waveManager;
	int           oldWaveCount;

	// --- フィールド・地形 ---
	vnModel* pGround;
	vnModel* pSky;
	vnModel* pFence[FENCE_NUM_MAIN];
	vnModel* pTree[TREE_NUM*2];
	float         FenceRadius;//敵の移動のためにstatic
	BlockManager* m_pBlockManager;

	// --- カメラ (極座標) ---
	float radius; // 半径	  （保存用）
	float theta;  // 経度	  （保存用）
	float phi;    // 緯度	  （保存用）

	float m_radius; // 半径
	float m_theta;  // 経度
	float m_phi;    // 緯度


	//レベルアップ時の向き
	float m_levelUpCameraTargetTheta = 0.0f;
	bool m_isLevelUpStarted = false;


	// --- コンボ・スコアシステム ---
	int   m_comboCount;
	float m_comboTimer;
	float m_currentComboLimit;
	int   m_killCounter;
	float m_comboScale;

	// --- 経験値・強化システム ---
	ExperienceManager* m_pExpManager;


	// --- UI: スプライト (ゲージ・アイコン) ---
	vnSprite* pHpBarBackBlack;
	vnSprite* pHpBarBack;
	vnSprite* pHpBarFront;
	vnSprite* pIconPlayer;
	vnSprite* pBackGroundBlack;
	float     backGroundBlackScale;

	vnSprite* pExpBarBackBlack;
	vnSprite* pExpBarBack;
	vnSprite* pExpBarFront;


	// --- スキルUI用スプライト ---
	// 範囲攻撃（Area Attack）用UI
	vnSprite* pAreaAtkBtnBackBlack; // 一番下の黒い座布団（または外枠）
	vnSprite* pAreaAtkBtnBack;      // スキルアイコン画像本体
	vnSprite* pAreaAtkBtnFront;     // 上から重ねるクールダウン用の影（時計回りに減る）
	vnSprite* pAreaSkillIcon;		//範囲攻撃のアイコン
	bool m_bIsAreaSkillMaxPrev = true; // 前回MAXだったか
	float m_areaSkillIconScale = 1.0f;           // 現在のアイコンのスケール（初期値 1.0）
	float m_areaSkillIargetScale = 1.0f;         // 目標のスケール（初期値 1.0）
	UIColor m_areaAtkUIColor;			//色を保存しておくよう


	// 引き寄せ攻撃（Pull）用UI
	vnSprite* pPullBtnBackBlack;
	vnSprite* pPullBtnBack;
	vnSprite* pPullBtnFront;
	vnSprite* pPullSkillIcon;
	bool m_bIsPullSkillMaxPrev = true; // 前回MAXだったか
	float m_pullSkillIargetScale = 1.0f;         // 目標のスケール（初期値 1.0）
	float m_pullSkillIconScale = 1.0f;           // 現在のアイコンのスケール（初期値 1.0）
	UIColor m_pullUIColor;


	// --- UI: スプライト (コンボ・操作説明) ---
	vnSprite* pComboSprites[3][10];
	vnSprite* pComboWord;
	vnSprite* pImageW;
	vnSprite* pImageA;
	vnSprite* pImageS;
	vnSprite* pImageD;
	
	vnSprite* pImageE;	//スキルボタン範囲攻撃
	vnSprite* pImageQ;	//引き寄せ攻撃


	// --- ボス登場時に出す文字 ---
	bool m_isBossAppearanceTriggered = false; // ボス演出が既にトリガーされたか
	bool m_showBossText = false;              // 現在ボス文字を表示中か
	float m_bossTextTimer = 0.0f;             // 表示時間をカウントするタイマー



	// --- ポーズ中画面に出すもの ---
	vnSprite* m_pPauseFrame;		//ポーズ中に出るフレーム
	vnSprite* m_pPauseFrame2;		//ポーズ中に出るフレーム

	//--説明を見やすくする為の黒い背景
	vnSprite* m_pUIBackGroundBlack[2];
	vnSprite* m_pUIBackGroundBlackPause;



	UpgradeSelectionUI *m_pUpgradeUI;


	// --- リソース (フォント・サウンド) ---
	int                 FontNum;
	IDWriteTextFormat** textFormat;
	int                 fileNum;
	int                 blinkCounter;	//文字の点滅用

	//vnSound** pSound;

	std::unique_ptr<SoundManager> soundManager;


	// デバッグ
	int m_leaderCount = 0;
	int m_activeCount = 0;
	int m_spawnNum = 0;

private:
	// --- 内部処理関数 (executeの分割) ---
	void UpdateIdel();
	void UpdatePlay(float deltaTime);
	void UpdateLevelUp();
	void UpdatePause();
	void UpdateBossPause();

	// --- Play中のサブシステム ---
	void UpdatePlayer(float deltaTime);			//プレイヤー挙動・衝突判定
	void SpawnEnemies(float deltaTime);			//敵の出現管理
	void UpdateEnemies(float deltaTime);		//敵の移動・衝突・プレイヤーとの判定
	void UpdateCombo(float deltaTime);			//コンボ計算・回復
	void AddCombo(NewEnemyClass* enemy);							//コンボ加算
	void UpdateGlobalSystems(float deltaTime);	// フェンス・タイマー・カメラ



	// --- GameOver,Clear処理 ---
	void UpdateGameOver();	
	void UpdateGameClear();
	void CleanUpScene();	//敵の消去やUIの非表示などの共通片付け

	// --- その他 ---
	void UpdateWaveTransition();	//Waveクリア待ち・次Waveへの遷移
	void UpdateBlocksCollision();	//ブロックとの当たり判定（プレイヤー、敵）

	void DebugDraw();				//デバッグ表示

	// --- カメラ・演出 ---
	void StartCameraRote();
	void UpdateFencePositions();
	
	// --- バーの関連 ---
	void setHPbarRender(bool on);

	void SetExpbarRender(bool on);

	void SetSkillUIRender(bool on); 

	//バーの設定
	void CreateUIBar(
		EnemyPool::UIBar& ui,
		float centerX,
		float posY,
		float width,
		float height);

	//？マークと吹き出しの設定
	EnemyPool::UIQuestionExplain CreateQuestionUI(const WCHAR* text, DWORD color, float offsetSize);


	// --- 衝突判定・計算系 ---
	eDirection colliderCtoC(CharacterBase* p1, CharacterBase* p2);
	eDirection colliderStoS(CharacterBase* p1, CharacterBase* p2);

	void OnCollider(vnCharacter* pCharacter, vnModel* pGround, float footOffset, RigidbodyComponent& rigidBody);
	void InFence(vnCharacter* pObject);
	bool CheckFenceReflection(vnCharacter* pObject);//弾の反射用
	void SetWAVETree();

	void SetupEnemy(NewEnemyClass* enemy, const NewEnemyClass::EnemyData& data,bool isLeader, bool isBoss);

public:
	bool initialize();
	void terminate();
	void execute();
	void render();
};