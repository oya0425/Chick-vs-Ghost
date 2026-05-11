#pragma once
#include <map>
#include <array>
class UpgradeSelectionUI
{
public:
	struct SlotUI
	{
		vnSprite* backGroundImg = nullptr;
		vnSprite* freamImg = nullptr;
		vnSprite* mainImg = nullptr;

		float currentY;
		float targetY;

		const wchar_t* label;
		const wchar_t* suffix;

		float value;
		int currentLv;
		int maxLv;
	};

	UpgradeSelectionUI();
	~UpgradeSelectionUI();
	void SetBackGroundImg(int index, vnSprite* img);
	void SetFreamImg(int index, vnSprite* img);
	void SetMainImg(int index, vnSprite* img);


	//void SetBackGroundImg(vnSprite* img) { m_backGroundImg = img; }
	//void SetFreamImg(vnSprite* img) { m_freamImg = img; }
	//void SetMainImg(vnSprite* img) { m_mainImg= img; }

	vnSprite* GetBackGroundImg(int index);
	vnSprite* GetFreamImg(int index);
	vnSprite* GetMainImg(int index);

	//レベルアップ時に出すUIをセッティング
	// (「強化項目（速度など）」,%アップ,10.0などの数値,強化する値の配列番号,アイコン番号,スキルのレベル,最大レベル)
	void SettingUI(
		const ExperienceManager::UpgradeUIData& data,
		int uiIndex,
		int valueIndex);

	//レベルアップ終了時UIを隠す
	void HideUI();

	//レベルアップ時に上にUIをスーって出す
	void UpdateUI();

	bool GetIsFinishAnim()const { return m_isAnimLvFinish; }


	bool GetIsClosingUI()const { return m_isClosingUI; }
	void SetIsClosingUI(bool isClosing) { m_isClosingUI = isClosing; }

private:
	//vnSprite* m_backGroundImg;
	//vnSprite* m_freamImg;
	//vnSprite* m_mainImg;
	static constexpr int MAX_SKILL_NUM = 5;
	std::array<SlotUI,5> m_slots;
	std::array<SlotUI, 3> m_displaySlots;	//画面に表示する用

	bool m_isAnimation    = false;				//アニメーション中かどうか
	bool m_isAnimUp	      = false;				//上に上昇中か
	bool m_isAnimLvFinish = false;				//レベルアップ終了時に上に上がるようにする
	float m_animTimer	  = 0;					//上昇する時間
	bool m_isClosingUI = false;
};