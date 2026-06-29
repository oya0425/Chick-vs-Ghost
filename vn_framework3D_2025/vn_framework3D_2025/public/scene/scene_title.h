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
		RULE1,
		RULE2,
		RULE3,
		RULE4,

	}m_titleState=TitleState::MAIN;



	//初期化
	bool initialize();
	//終了
	void terminate();

	//処理
	void execute();
	//描画
	void render();


	//ボタンの当たり判定
	bool OnButton(float x, float y);
	bool UpdateButton(
		float x,
		float y,
		vnSprite* pButton,
		bool& isOnButton,
		float& buttonScale);

	void ChangeBackGround(TitleUI ui);

	void ChangeButtonTextSize(float x, float y, float fontScale, bool isOnButton,const WCHAR* text);

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
	vnSprite* pRuleButton;
	vnSprite* pLeftButton;
	vnSprite* pRightButton;

	//ボタン設定
	bool isOnStartButton;
	bool isOnRuleButton;
	bool isOnLeftButton;
	bool isOnRightButton;

	float startButtonScale = 1.0f;
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

};
