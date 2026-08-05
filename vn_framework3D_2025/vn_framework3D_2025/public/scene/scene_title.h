#pragma  once
#include <vector>
#include <memory>

enum TitleUI
{
	TITLE_MAIN = 0,		// タイトル画面
	TITLE_RULE1,		// 説明1
	TITLE_RULE2,		// 説明2
	TITLE_RULE3,		// 説明3
	TITLE_RULE4,		// 説明4

	TITLE_MAX
};



class SceneTitle :public vnScene
{
public:
	enum class TitleState
	{
		MAIN,
		RULE,
		//RULE2,
		//RULE3,
		//RULE4,

	}m_titleState = TitleState::MAIN;

	//押されたボタンの種類（ルールボタンを除く）
	enum class StartType
	{
		Normal,
		Tutorial,
		Endless
	};

	//enum class TitleState
	//{
	//	MAIN,
	//	RULE,
	//}m_titleState = TitleState::MAIN;


	//初期化
	bool initialize();
	//終了
	void terminate();

	//処理
	void execute();
	//描画
	void render();


	//void ChangeBackGround(TitleUI ui);
	void ChangeBackGround(int index);


	void OnStartButton(StartType type, float& buttonScale,vnSprite*pButton);

private:
	//-----フォント----------
	int FontNum;	//使用できるフォントの数
	IDWriteTextFormat** textFormat_score;	//作成されたフォント

	//BGM
	int fileNum;
	vnSound** pSound;
	std::unique_ptr<SoundManager> m_soundManager;


	// --- 画像 ---
	std::vector<vnSprite*> m_pBackGround;
	//vnSprite* pBackGround;
	vnSprite* pStartButton;
	vnSprite* pTutorialStartButton;
	vnSprite* pEndlessStartButton;
	vnSprite* pRuleButton;
	vnSprite* pLeftButton;
	vnSprite* pRightButton;

	//ボタン設定
	bool isOnStartButton;
	bool isOnTutorialStartButton;
	bool isOnEndlessStartButton;
	bool isOnRuleButton;
	bool isOnLeftButton;
	bool isOnRightButton;

	float startButtonScale = 1.0f;
	float tutorialStartButtonScale = 1.0f;
	float endlessStartButtonScale = 1.0f;
	float ruleButtonScale = 1.0f;
	float leftButtonScale = 1.0f;
	float rightButtonScale = 1.0f;

	// --- 文字の点滅 ---
	int blinkCounter = 0;	//文字が点滅するのに使う時間
	float buttonScale = 1.0f;

	// --- 演出用 ---
	bool isStarting = false; // シーン遷移演出中か
	float titleRotation = 0.0f; // 回転角度


	int m_currentUI = TITLE_MAIN;	//最初はメインの画像
	int m_rulePage = 0;			//ルールのページ番号

	StartType startType = StartType::Normal;	//どのボタンを押したかを保存
};
