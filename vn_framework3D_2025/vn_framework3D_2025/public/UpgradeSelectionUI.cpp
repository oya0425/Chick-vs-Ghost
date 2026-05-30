#include"../framework.h"
#include"../framework/vn_environment.h"

namespace
{
	constexpr float textOffset = 10;
}

UpgradeSelectionUI::UpgradeSelectionUI()
{

}



UpgradeSelectionUI::~UpgradeSelectionUI()
{

}
void UpgradeSelectionUI::SettingUI(
	const ExperienceManager::UpgradeUIData& data,
	int uiIndex,
	int valueIndex)
{
	//if (uiIndex < 0 || uiIndex >= 3) return;
	//if (data.spriteID < 0 || data.spriteID >= 5) return; // arrayのサイズ5に対応
	//// 配列の安全チェックは残す
	if (uiIndex < 0 || uiIndex >= 3) return;

	// （これで3つ目の表示が復活するかテスト）
	int safeSpriteID = data.spriteID;
	if (safeSpriteID < 0 || safeSpriteID >= 5)
	{
		// 止まったとき「data.spriteID」がいくつになっているか確認してください。
		safeSpriteID = 0; // 仮に 0 (移動速度のアイコン) にして強制進める
	}
	m_isAnimLvFinish = false;
	m_isClosingUI = false;
	float posY = 200.0f + (uiIndex * 170.0f);

	m_displaySlots.at(uiIndex) = m_slots[data.spriteID];


	auto& slot = m_displaySlots[uiIndex];

	
	// 表示して、表示位置へ移動
	slot.freamImg->setRenderEnable(true);
	slot.backGroundImg->setRenderEnable(true);
	slot.mainImg->setRenderEnable(true);

	slot.freamImg->setAlpha(0.5f);
	slot.backGroundImg->setAlpha(0.5f);
	slot.mainImg->setAlpha(1.0f);

	slot.targetY = posY;
	slot.currentY = slot.targetY + 700.0f; // 下から出す

	slot.freamImg->setPos(640, slot.currentY);

	slot.backGroundImg->setPos(640, slot.currentY);

	slot.mainImg->setPos(220, slot.currentY);

	//文字で出すやつを保存しておく
	slot.label = data.label;
	slot.suffix = data.suffix;
	slot.value = data.value[valueIndex];
	slot.currentLv = data.currentLv;
	slot.maxLv = data.maxLv;

	vnFont::print(
		400,
		slot.currentY-textOffset,
		GAME_COLOR_YELLOW,
		L"[%d] %ls + %d %ls (Lv.%d / %d)",
		uiIndex + 1,
		data.label,
		(int)data.value[valueIndex],
		data.suffix,
		data.currentLv,
		data.maxLv
	);
	m_isAnimation = true;
}

void UpgradeSelectionUI::UpdateUI()
{
	if (!m_isAnimation) return;

	bool allOutOfScreen = true; // 全ての要素が画面外に行ったかチェック用
	vnFont::setFontSize(38, 30);
	for (int i = 0; i < m_displaySlots.size(); i++)
	{
		auto& slot = m_displaySlots[i];

		// 移動処理
		slot.currentY += (slot.targetY - slot.currentY) * 0.1f;

		// 座標更新
		slot.freamImg->setPos(640, slot.currentY);
		slot.backGroundImg->setPos(640, slot.currentY);
		slot.mainImg->setPos(220, slot.currentY);

		// 描画（画面内にいる時だけ描画すると効率的）
		if (slot.currentY > -100) {
			vnFont::print(400, slot.currentY - textOffset, L"[%d] %ls + %d %ls (Lv.%d / %d)",
				i + 1, slot.label, (int)slot.value, slot.suffix, slot.currentLv, slot.maxLv);
		}

		// まだ画面内に残っているものがあれば false
		if (slot.currentY > -500) {
			allOutOfScreen = false;
		}
	}

	// 全て画面外へ移動し終えたら、終了フラグを立てて描画を止める
	if (allOutOfScreen && m_displaySlots.size() > 0)
	{
		for (auto& slot : m_displaySlots) {
			slot.freamImg->setRenderEnable(false);
			slot.backGroundImg->setRenderEnable(false);
			slot.mainImg->setRenderEnable(false);
		}
		m_isAnimLvFinish = true;
		m_isClosingUI = true;
		// m_isAnimation = false; // 必要に応じてアニメーション自体の更新も止める
	}



}

void UpgradeSelectionUI::HideUI()
{

	// 目的地を画面外に設定するだけ（これ以降 UpdateUI で移動が始まる）
	for (int i = 0; i < m_displaySlots.size(); i++)
	{
		auto& slot = m_displaySlots[i];
		slot.targetY = -700; // 画面上に消去
	}
	m_isAnimLvFinish = false; // 念のためリセット

}


void UpgradeSelectionUI::SetBackGroundImg(int index, vnSprite* img)
{
	m_slots[index].backGroundImg = img;
}

void UpgradeSelectionUI::SetFreamImg(int index, vnSprite* img)
{
	m_slots[index].freamImg = img;
}

void UpgradeSelectionUI::SetMainImg(int index, vnSprite* img)
{
	m_slots[index].mainImg = img;
}

vnSprite* UpgradeSelectionUI::GetBackGroundImg(int index)
{
	return m_slots[index].backGroundImg;
}

vnSprite* UpgradeSelectionUI::GetFreamImg(int index)
{
	return m_slots[index].freamImg;
}

vnSprite* UpgradeSelectionUI::GetMainImg(int index)
{
	return m_slots[index].mainImg;
}