#pragma  once

class SceneTitle :public vnScene
{
public:
	//初期化
	bool initialize();
	//終了
	void terminate();

	//処理
	void execute();
	//描画
	void render();



private:
	//-----フォント----------
	int FontNum;	//使用できるフォントの数
	IDWriteTextFormat** textFormat_score;	//作成されたフォント

	//BGM
	int fileNum;
	vnSound** pSound;


	// --- 画像 ---
	vnSprite* pBackGround;
	vnSprite* pButton;

	// --- 文字の点滅 ---
	int blinkCounter = 0;	//文字が点滅するのに使う時間
	float buttonScale = 1.0f;

	// --- 演出用 ---
	bool isStarting = false; // シーン遷移演出中か
	float titleRotation = 0.0f; // 回転角度

	// --- ボタンの当たり判定 ---
	bool isOnButton;
};
