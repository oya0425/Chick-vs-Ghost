#pragma once

//メインで表示するテキストに関するもののみ
class SceneMainTextUI
{
public:
	//待機中の文字の表示
	void RenderIdleText(float fontOffset, float text_RIGHT_CLICK_x, IDWriteTextFormat* pFormat);
	
	//ゲーム中の文字の表示
	void RenderPlayText(
		WaveManager* waveManager, bool isTutorial, bool isEndless,
		float fontOffset, float text_RIGHT_CLICK_x,
		float baseX, float baseY, bool& showBossText, float& bossTextTimer,
		IDWriteTextFormat* pFormat);

	//ゲームオーバー時の文字の表示
	void RenderGameOverText(bool isEndless, int maxKillCount, float fontOffset,float text_RIGHT_CLICK_x,IDWriteTextFormat*pFormat);

	//ゲームクリア時の文字の表示
	void RenderGameClearText(float fontOffset, float text_RIGHT_CLICK_x, IDWriteTextFormat* pFormat);

	//ゲーム中のポーズ中の文字の表示
	void RenderPauseText(float fontOffset, IDWriteTextFormat* pFormat);

	//チュートリアルクリア時の文字の表示
	void RenderTutorialClearText(
		float textScale,
		float positionX,
		float positionY,
		const wchar_t* text,
		float fontOffset,
		IDWriteTextFormat* pFormat);

private:
	//文字を影付きで表示
	void PrintShadow(float x, float y, unsigned int color, unsigned int shadowColor, const wchar_t* text);

	//数値を影付きで表示
	void PrintShadowValue_int(float x, float y, unsigned int color, unsigned int shadowColor, const wchar_t* text, int value);

	int blinkCounter = 0;	//文字の点滅用


};