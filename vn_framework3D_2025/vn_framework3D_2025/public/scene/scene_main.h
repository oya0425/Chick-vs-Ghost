#pragma once
#include <memory>
#include"../object/bear.h"
#define AABB_MAX (64)
//
#define FENCE_NUM_MAIN (40)			//フェンスモデルの数
#define TREE_NUM (10)
constexpr float GROUND_OFFSET = 0.1f;

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
		Opening,
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

	
	//チュートリアル用
	enum class TutorialState
	{
		None,
		StartMessage,          // 最初の説明表示「敵にたいあたりしよう！」
		WaitSkillUse,          // 両方1回ずつ使用
		WaitSkillSelect,       // スキル選択待ち


		//説明UIを出すチュートリアルが頭に「Explain」をつける
		ExplainEnemyLeader,    // 初めて敵を倒すまで

		ExplainExp,            // 経験値・特攻・パニック説明
		ExplainLevelUp,        // レベルアップ待ち

		ExplainSkills,         // 引き寄せ・範囲攻撃説明

		ExplainPlayerOperation,//プレイヤーの基本操作説明

		ExplainPlayerDamage,   //プレイヤーがダメージを受けたとき

		Finish,
	};
	
	//タイトルに戻るボタンを押したときに確認をするため
	enum class ReturnTitleState
	{
		None,
		Confirm
	}m_returnTitleState = ReturnTitleState::None;

	//チュートリアル振り返りボタン（ポーズ中に出す）
	enum class TutorialReviewState
	{
		None,
		Select,
		Explanation,

	};



	struct UIColor
	{
		XMVECTOR colorBackBlack;
		XMVECTOR colorBack;
		XMVECTOR colorFront;
		XMVECTOR colorIcon;

	};

	//ボタンの種類
	enum class UIButton
	{
		TITLEBACK = 0,		//タイトルに戻るボタン
		MESSAGE_LEFT,		//戻る
		MESSAGE_CLOSE,		//閉じる
		MESSAGE_RIGHT,		//進む
		MESSAGE_YES,		//はい
		MESSAGE_NO,			//いいえ

		//チュートリアル振り返り用
		TUTORIAL_REVIEW,			//振り返り
		TUTORIAL_BACK,				//振り返りボタンを押した後に戻るよう
		TUTORIAL_ENEMY_LEADER,		//リーダーを倒したときに出てくる敵の説明
		TUTORIAL_EXP,				//敵を倒したときに出てくる説明（経験値獲得）
		TUTORIAL_LEVEL_UP,			//レベルアップ時に出てくる説明
		TUTORIAL_SKILLS,			//スキルを獲得したときの説明
		TUTORIAL_PLAYER_OPERATION,	//プレイヤーの基本操作説明
		TUTORIAL_PLAYER_DAMAGE,		//プレイヤーがダメージを受けたときの説明


		MaxNum				//最大数
	};
	//ボタンの情報
	struct ButtonData
	{
		vnSprite* sprite = nullptr;
		bool isOn = false;
		float scale = 1.0f;


		const WCHAR* text = L"";	 //表示文字

		float position_x = 0;        //位置 X
		float position_y = 0;        //位置 Y

		float font_pos_offset_x = 0; //表示する文字の位置調整用

		bool visible = true;          //表示するか

		SE_ID se_id = SE_ID::NONE;
	};
	//ボタンの初期化
	void InitButton(
		UIButton type,
		float x, float y,
		const wchar_t* text,
		float fontOffsetX = 0.0f);

	bool UpdateButton(UIButton id);

	//説明ウィンドウ
	enum class WindowMode
	{
		None,
		Open,
		Close
	} m_windowMode = WindowMode::None;

	// --- 説明のUI構造体 ---
	enum class ExplanationType
	{
		EnemyLeader,		//リーダーを倒したときに出てくる敵の説明
		Exp,				//敵を倒したときに出てくる説明（経験値獲得）
		LevelUp,			//レベルアップ時に出てくる
		Skills,				//スキルを獲得したときの説明
		PlayerOperation,	//プレイヤーの基本操作説明
		PlayerDamage,		//プレイヤーがダメージを受けたときの説明
		MaxNum
	};
	struct ExplanationUIData
	{
		std::vector<vnSprite*> images;
		float position_x = 0.0f;
		float position_y = 0.0f;

		float position_new_x = 0.0f;	//もとの位置を保存しておく
		float position_new_y = 0.0f;

		bool visible = true;          //表示するか
	
		bool isOne = false;			  //一度表示したか
	};

	//説明の画像をスライドさせるアニメーション用
	enum class ExplanationSlideState
	{
		None,
		SlideLeft,
		SlideRight
	};





	// --- 定数 (constexpr) ---
	static constexpr float COMBO_BASE_TIME = 3.0f;
	static constexpr float COMBO_MIN_TIME = 1.0f;
	static constexpr float COMBO_DECREASE = 0.1f;

	// --- ゲーム全体の状態 ---
	GameState m_gameState;
	GameState currentState = m_gameState;
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

	vnSprite* m_pGameOverPlayer;

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

	//HPバーのシェイク
	float m_hpShakeTimer = 0.0f;     // シェイクの残り時間
	float m_hpShakeDuration = 0.25f; // 揺れる時間
	float m_hpShakeIntensity = 6.0f; // 揺れの強さ
	float m_prevHp = -1.0f;          // 初期化チェック用の前回HP

	//=======================================
	// --- スキルUI用スプライト ---
	//=======================================
	// 範囲攻撃（Area Attack）用UI
	vnSprite* pAreaAtkBtnBackBlack; // 一番下の黒い座布団（または外枠）
	vnSprite* pAreaAtkBtnBack;      // スキルアイコン画像本体
	vnSprite* pAreaAtkBtnFront;     // 上から重ねるクールダウン用の影
	vnSprite* pAreaSkillIcon;		//範囲攻撃のアイコン
	bool m_bIsAreaSkillMaxPrev = true; // 前回MAXだったか
	float m_areaSkillIconScale = 1.0f;           // 現在のアイコンのスケール（初期値 1.0）
	float m_areaSkillTargetScale = 1.0f;         // 目標のスケール（初期値 1.0）
	UIColor m_areaAtkUIColor;			//色を保存しておく用

	//=======================================
	// 引き寄せ攻撃（Pull）用UI
	//=======================================
	vnSprite* pPullBtnBackBlack;
	vnSprite* pPullBtnBack;
	vnSprite* pPullBtnFront;
	vnSprite* pPullSkillIcon;
	bool m_bIsPullSkillMaxPrev = true;			 // 前回MAXだったか
	float m_pullSkillTargetScale = 1.0f;         // 目標のスケール（初期値 1.0）
	float m_pullSkillIconScale = 1.0f;           // 現在のアイコンのスケール（初期値 1.0）
	UIColor m_pullUIColor;


	//===============================================
	// --- UI: スプライト (コンボ・操作説明) ---
	//===============================================
	vnSprite* pComboSprites[3][10];
	vnSprite* pComboWord;
	vnSprite* pImageW;
	vnSprite* pImageA;
	vnSprite* pImageS;
	vnSprite* pImageD;
	vnSprite* pImageTab;

	vnSprite* pImageE;	//スキルボタン範囲攻撃
	vnSprite* pImageQ;	//引き寄せ攻撃

	//時間表示の後ろに置く背景（円形の）
	vnSprite* pTimeBackGround;

	// --- ポーズ中画面に出すもの ---
	vnSprite* m_pPauseFrame;		//ポーズ中に出るフレーム
	vnSprite* m_pPauseFrame2;		//ポーズ中に出るフレーム

	//--説明を見やすくする為の黒い背景
	vnSprite* m_pUIBackGroundBlack[2];
	vnSprite* m_pUIBackGroundBlackPause;


	// --- ボタン ---
	ButtonData m_buttonData[(int)UIButton::MaxNum];	//ボタンの種類とその分のデータ

	//====================================
	// --- スキル獲得選択ボタン ---
	//====================================
	bool m_isOnSelectButton[3] = { false };

	float m_SelectButtonScale[3] = { 1.0f };

	// --- ボス登場時に出す文字 ---
	bool m_isBossAppearanceTriggered = false; // ボス演出が既にトリガーされたか
	bool m_showBossText = false;              // 現在ボス文字を表示中か
	float m_bossTextTimer = 0.0f;             // 表示時間をカウントするタイマー





	UpgradeSelectionUI *m_pUpgradeUI;


	// --- リソース (フォント・サウンド) ---
	int                 FontNum;
	IDWriteTextFormat** textFormat;
	int                 fileNum;
	int                 blinkCounter;	//文字の点滅用

	//vnSound** pSound;

	std::unique_ptr<SoundManager> soundManager;

	

	//======================================================
	// チュートリアル関係、エンドレスモード関係
	//======================================================
	float m_windowOpenTimer = 0;							//ウィンドウの拡大する時間（変化する変数）
	bool m_isTutorial = false;								//チュートリアルかどうか
	TutorialState m_state_tutorial = TutorialState::None;	//無しにして置き、タイトルでチュートリアルが選択されたときに設定する
	void ChangeTutorialState(ExplanationType type, bool isReview);

	//振り返りボタンを押した後のボタンの表示非表示
	void SetTutorialReviewButtonVisible(bool visible);

	bool m_isEndless = false;								//エンドレスモードかどうか（プレイヤーが死ぬまで終わらない）

	//チュートリアル振り返りボタン処理用
	TutorialReviewState m_tutorialReviewState = TutorialReviewState::None;


	//表示するウィンドウ（説明）
	vnSprite* m_messageBackground;					//説明の画像の後ろの背景
	float m_windowScale = 0.0f;
	bool m_isOpen = false;
	bool m_isClosing = false;
	bool UpdateMessageWindow(float targetScale, const WCHAR* text, WindowMode mode, ExplanationUIData* explanation);


	//説明の画像の管理
	ExplanationUIData m_explanationUI[(int)(ExplanationType::MaxNum)];
	int m_explanationPage = 0;	//説明のページ（切り替えよう）
	void UpdateExplanationButtons(ExplanationType type);	//説明の画像の切り替え用のボタンの制御
	void UpdateExplanation(ExplanationType type);			//チュートリアルの説明の制御

	//説明の画像のアニメーション用
	ExplanationSlideState m_explanationSlideState = ExplanationSlideState::None;
	float m_explanationSlideTime = 0.0f;
	void UpdateExplanationSlide(ExplanationType type);
	int m_explanationSlidePage = 0;					//もともとのページ


	// デバッグ
	int m_leaderCount = 0;
	int m_activeCount = 0;
	int m_spawnNum = 0;

private:
	void RegisterCharacter(vnCharacter* character);

	//初期化関数
	void InitializeVariables();      // 変数初期化
	void InitializePlayer();         // プレイヤー・弾
	void InitializeEnemies();        // EnemyPool・敵生成
	void InitializeField();          // 地形・木・フェンス
	void InitializeEffects();        // エフェクト
	void InitializeUI();             // HP・Exp・スキルUI
	void InitializePauseUI();        // ポーズ画面
	void InitializeUpgradeUI();      // レベルアップUI
	void InitializeFont();           // フォント
	void InitializeSound();          // サウンド
	void InitializeTutorial();       // チュートリアルに関する設定
	void InitializeExplanationUI();  // ボタン、説明の画像 UI

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

	void UpdateTutorial(float deltaTime);		//チュートリアル


	// --- GameOver,Clear処理 ---
	void UpdateGameOver();	
	void UpdateGameClear();
	void CleanUpScene();	//敵の消去やUIの非表示などの共通片付け

	// --- その他 ---
	void UpdateWaveTransition();	//Waveクリア待ち・次Waveへの遷移
	void UpdateBlocksCollision();	//ブロックとの当たり判定（プレイヤー、敵）

	void HandleBackgroundFade(bool isFadeOut, float& scale, float speed);	//黒い画像を拡大縮小


	void DebugDraw();				//デバッグ表示

	// --- カメラ・演出 ---
	void StartCameraRote();
	void UpdateFencePositions();
	
	// --- バーの関連 ---
	void setHPbarRender(bool on);

	void SetExpbarRender(bool on);

	void SetSkillUIRender(bool on); 

	//スキルのバーのUpdate
	void UpdateSkillBar(
		float currentCoolTime,	//現在のクールタイム
		float maxCoolTime,		//最大クールタイム
		vnSprite* pSkillIcon,	//スキルアイコン
		vnSprite* pSkillBar,	//ゲージ前面
		bool& isSkillMaxPrev,	//前のフレームで満タンだったか
		float& skillIconScale,	//アイコン倍率
		float& skillTargetScale,//目標倍率
		float barLeftEdge,		//バー左端設定
		float maxWidth,			//バー最大幅
		float heightY			//Y座標
	);

	//Hpバーのシェイク
	void ShakeHpBar(
		float currentHp,
		float deltaTime,
		float& outOffsetX,
		float& outOffsetY
	);


	//バーの設定
	void CreateUIBar(
		EnemyPool::UIBar& ui,
		float centerX,
		float posY,
		float width,
		float height);

	//？マークと吹き出しの設定
	EnemyPool::UIQuestionExplain CreateQuestionUI(const WCHAR* text, DWORD color, float offsetSize);


	// --- レベルアップ時ボタン ---
	bool UpdateUpgradeButton(
		float x,
		float y,
		vnSprite* pFrame,
		vnSprite* pBg,
		vnSprite* pMain,
		bool& isOnButton,
		float& buttonScale,
		bool isMax);
		
	bool OnButton(float x, float y);


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