#pragma once
#include <memory>
#include"../object/bear.h"
#define AABB_MAX (64)
//
#define FENCE_NUM_MAIN (40)			//フェンスモデルの数
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
		GameOver,
		GameClear,
		GameFinish 
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


	// --- UI: スプライト (コンボ・操作説明) ---
	vnSprite* pComboSprites[3][10];
	vnSprite* pComboWord;
	vnSprite* pImageW;
	vnSprite* pImageA;
	vnSprite* pImageS;
	vnSprite* pImageD;

	//--説明を見やすくする為の黒い背景
	vnSprite* m_pUIBackGroundBlack;
	vnSprite* m_pUIBackGroundBlackPause;

	//スキルアイコン
	SkillButtonUI* m_SkillIcon_PullAtk;
	vnSprite* m_pImgSkillIcon_PullAtk;


	UpgradeSelectionUI *m_pUpgradeUI;


	// --- リソース (フォント・サウンド) ---
	int                 FontNum;
	IDWriteTextFormat** textFormat;
	int                 fileNum;
	vnSound** pSound;
	int                 blinkCounter;

private:
	// --- 内部処理関数 (executeの分割) ---
	void UpdateIdel();
	void UpdatePlay(float deltaTime);
	void UpdateLevelUp();
	void UpdatePause();

	// --- Play中のサブシステム ---
	void UpdatePlayer(float deltaTime);			//プレイヤー挙動・衝突判定
	void SpawnEnemies(float deltaTime);			//敵の出現管理
	void UpdateEnemies(float deltaTime);		//敵の移動・衝突・プレイヤーとの判定
	void UpdateCombo(float deltaTime);			//コンボ計算・回復
	void AddCombo();							//コンボ加算
	void UpdateGlobalSystems(float deltaTime);	// フェンス・タイマー・カメラ



	// --- GameOver,Clear処理 ---
	void UpdateGameOver();	
	void UpdateGameClear();
	void CleanUpScene();	//敵の消去やUIの非表示などの共通片付け

	// --- その他 ---
	void UpdateWaveTransition();	//Waveクリア待ち・次Waveへの遷移
	void UpdateBlocksCollision();	//ブロックとの当たり判定（プレイヤー、敵）

	// --- カメラ・演出 ---
	void StartCameraRote();
	void UpdateFencePositions();
	void setHPbarRender(bool on);

	void SetExpbarRender(bool on);

	// --- 衝突判定・計算系 ---
	eDirection colliderCtoC(CharacterBase* p1, CharacterBase* p2);
	eDirection colliderStoS(CharacterBase* p1, CharacterBase* p2);

	void OnCollider(vnCharacter* pCharacter, vnModel* pGround, float footOffset, RigidbodyComponent& rigidBody);
	void InFence(vnCharacter* pObject);
	bool CheckFenceReflection(vnCharacter* pObject);//弾の反射用

	void SetupEnemy(NewEnemyClass* enemy, const NewEnemyClass::EnemyData& data,bool isLeader);

public:
	bool initialize();
	void terminate();
	void execute();
	void render();
};