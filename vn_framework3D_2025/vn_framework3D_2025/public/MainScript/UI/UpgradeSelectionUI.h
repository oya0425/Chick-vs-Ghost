#pragma once
#include <map>
#include <array>
class UpgradeSelectionUI
{
public:
	struct SlotUI
	{
		vnSprite* backGroundImg = nullptr;		//一番後ろの背景
		vnSprite* freamImg = nullptr;			//フレーム
		vnSprite* mainImg = nullptr;			//強化項目のアイコン

		float currentY;							//現在位置
		float targetY;							//目標位置

		const wchar_t* label;					//強化の項目（速度など）
		const wchar_t* suffix;					//付属（～％アップなど）
			
		const wchar_t* explanation;				//軽い説明（速度が速くなるぞ　など）

		float value;							//強化される量（10など）
		int currentLv;							//現在のスキルのレベル
		int maxLv;								//最大のスキルレベル
	};


	enum class UIPhase {
		LogoFadeIn,      // 「レベルアップ！」ロゴが大きく表示されている期間
		SlotsSlideIn,    // ロゴが出終わり、3つの選択肢が滑り込んでくる期間
		Closing          // 選択されて、画面外へ消えていく期間
	};


	UpgradeSelectionUI();
	~UpgradeSelectionUI();
	void SetBackGroundImg(int index, vnSprite* img);
	void SetFreamImg(int index, vnSprite* img);
	void SetMainImg(int index, vnSprite* img);


	vnSprite* GetBackGroundImg(int index);
	vnSprite* GetFreamImg(int index);
	vnSprite* GetMainImg(int index);

	vnSprite* GetDisplayBackGroundImg(int index);
	vnSprite* GetDisplayFrameImg(int index);
	vnSprite* GetDisplayMainImg(int index);

	//レベルアップ時に出すUIをセッティング
	// (「強化項目（速度など）」,%アップ,10.0などの数値,強化する値の配列番号,アイコン番号,スキルのレベル,最大レベル)
	void SettingUI(
		const ExperienceManager::UpgradeUIData& data,
		int uiIndex,
		int valueIndex);

	//レベルアップ終了時UIを隠す
	void HideUI();

	//レベルアップ時に上にUIをスーって出す
	void UpdateUI(IDWriteTextFormat* pFormat);

	void ResetPhase();

	bool GetIsFinishAnim()const { return m_isAnimLvFinish; }


	bool GetIsClosingUI()const { return m_isClosingUI; }
	void SetIsClosingUI(bool isClosing) { m_isClosingUI = isClosing; }

	void SetUiPhase(UIPhase uiPhase) { m_uiPhase = uiPhase; }

private:
	static constexpr int MAX_SKILL_NUM = 5;
	std::array<SlotUI,3> m_slots;
	std::array<SlotUI, 3> m_displaySlots;	//画面に表示する用

	UIPhase m_uiPhase = UIPhase::LogoFadeIn;
	float   m_logoTimer = 0.0f;          // ロゴの表示時間を計るタイマー
	float   m_logoScale = 0.0f;          // ロゴの拡大率（演出用）

	// 「強化する項目を〜」のテキスト用座標
	float   m_headerY = -100.0f;
	float   m_headerTargetY = 150.0f; // 画面上部の定位置

	bool m_isAnimation    = false;				//アニメーション中かどうか
	bool m_isAnimUp	      = false;				//上に上昇中か
	bool m_isAnimLvFinish = false;				//レベルアップ終了時に上に上がるようにする
	float m_animTimer	  = 0;					//上昇する時間
	bool m_isClosingUI = false;



	// --- 役割分け関数 ---
	void UpdateLogo(IDWriteTextFormat* pFormat);
	void UpdateSlots(IDWriteTextFormat* pFormat);
	void FinishClosing();
	void UpdateHeader(IDWriteTextFormat* pFormat);
	bool IsClosingFinished();
};