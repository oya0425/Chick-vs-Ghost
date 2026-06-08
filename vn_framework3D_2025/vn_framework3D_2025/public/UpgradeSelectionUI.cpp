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
	//float posY = 200.0f + (uiIndex * 170.0f);
	
	//======================================
	// 選択肢のUIのYの始まり、枠と枠の幅
	//======================================
	float posY = 300.0f + (uiIndex * 150.0f);


	
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
	slot.explanation = data.explanation;
	slot.value = data.value[valueIndex];
	slot.currentLv = data.currentLv;
	slot.maxLv = data.maxLv;

	

	vnFont::print(
		400,
		slot.currentY-textOffset/2,
		GAME_COLOR_YELLOW,
		L"[%d] %ls + %d %ls (Lv.%d / %d)",
		uiIndex + 1,
		data.label,
		(int)data.value[valueIndex],
		data.suffix,
		data.currentLv,
		data.maxLv
	);

	vnFont::print(
		400,
		slot.currentY - textOffset,
		GAME_COLOR_YELLOW,
		L"%ls",
		data.explanation
		);



	m_isAnimation = true;
}

void UpgradeSelectionUI::UpdateUI()
{
	if (!m_isAnimation) return;

	// =================================================================
	// フェーズ1: 「レベルアップ！」ロゴの表示演出
	// =================================================================
	if (m_uiPhase == UIPhase::LogoFadeIn)
	{
		// 1. ロゴのスケールを滑らかに拡大（0.0f から 1.0f へ）
		m_logoScale += (1.0f - m_logoScale) * 0.1f;

		// 2. スケールを元にフォントサイズを計算
		float baseFontSize = 172.0f;
		float currentFontSize = baseFontSize * m_logoScale;

		if (currentFontSize > 1.0f) {
			vnFont::setTextFormat(vnFont::create(vnFont::getFontName(31), (int)currentFontSize));
		}

		// 3. 座標の中心補正計算
		float textWidth = currentFontSize * 2.5f;   // 5文字分に相当する幅を引く
		float textHeight = currentFontSize * 0.5f;  // 高さの半分を引く

		float tx = 640.0f - textWidth;
		float ty = 360.0f - textHeight;

		// 4. 文字の描画（影と本体）
		float off = 3.0f * m_logoScale;
		vnFont::print(tx + off, ty + off, GAME_COLOR_BLACK, L"LEVEL UP !");
		vnFont::print(tx, ty, GAME_COLOR_GOLD, L"LEVEL UP !");

		// 5. 最大サイズに達したらフェーズ移行
		if (m_logoScale >= 0.99f)
		{
			m_logoScale = 1.0f;
			m_uiPhase = UIPhase::SlotsSlideIn;
		}

		return; // ロゴ演出中は下の処理をさせずに抜ける
	}

	// =================================================================
	// フェーズ2 & 3: 「強化項目テキスト」と「3つの選択肢」の処理
	// =================================================================

	// ヘッダーテキストの移動
	m_headerY += (m_headerTargetY - m_headerY) * 0.1f;

	// 画面内にいる時だけ描画
	if (m_headerY > -50.0f)
	{
		vnFont::setFontSize(31, 30);
		vnFont::print(420, (int)m_headerY, GAME_COLOR_WHITE, L"強化する項目を選択してください");
	}

	// 画面外チェック用のフラグ（最初は true にしておき、画面内に何かあれば false にする）
	bool allOutOfScreen = true;

	// 【修正ポイント】終了演出中（Closing）の時だけ、ヘッダーが残っているかをチェックする
	if (m_uiPhase == UIPhase::Closing)
	{
		if (m_headerY > -100.0f) {
			allOutOfScreen = false; // まだヘッダーが画面内（上部）にあるなら終了させない
		}
	}
	else
	{
		// 通常表示中（SlotsSlideIn）は、強制的にフラグを false にして勝手に終了するのを防ぐ
		allOutOfScreen = false;
	}

	// スロットの更新と描画
	vnFont::setFontSize(31, 25);
	for (int i = 0; i < m_displaySlots.size(); i++)
	{
		auto& slot = m_displaySlots[i];

		// 移動・座標更新処理
		slot.currentY += (slot.targetY - slot.currentY) * 0.1f;
		slot.freamImg->setPos(640, slot.currentY);
		slot.backGroundImg->setPos(640, slot.currentY);
		slot.mainImg->setPos(220 * 1.1f, slot.currentY); // 元のコードの倍率を維持

		// 描画（画面内にいる時だけ描画）
		if (slot.currentY > -100 && slot.currentY < 800)
		{
			// 1行目: ステータス上昇情報
			vnFont::print(450, slot.currentY - textOffset * 2, GAME_COLOR_BLACK, L"[%d] %ls + %d %ls (Lv.%d / %d)",
				i + 1, slot.label, (int)slot.value, slot.suffix, slot.currentLv, slot.maxLv);

			// 2行目: スキルの説明文 (explanation)
			vnFont::print(550, slot.currentY + textOffset, GAME_COLOR_RED, L"(%ls)",
				slot.explanation);
		}

		// 終了演出中（Closing）かつ、スロットがまだ画面内に残っているかチェック
		if (slot.currentY > -500) {
			if (m_uiPhase == UIPhase::Closing) {
				allOutOfScreen = false;
			}
		}
	}

	// ヘッダーもスロットも、すべて完全に画面外へ移動し終えたら終了
	if (allOutOfScreen && m_displaySlots.size() > 0 && m_uiPhase == UIPhase::Closing)
	{
		for (auto& slot : m_displaySlots) {
			slot.freamImg->setRenderEnable(false);
			slot.backGroundImg->setRenderEnable(false);
			slot.mainImg->setRenderEnable(false);
		}
		m_isAnimLvFinish = true;
		m_isClosingUI = true;
	}
}

void UpgradeSelectionUI::ResetPhase()
{
	// 1. フェーズを最初のロゴ表示（拡大演出）に戻す
	m_uiPhase = UIPhase::LogoFadeIn;

	// 2. ロゴのスケールを 0 に戻して、次回また中央から広がるようにする
	m_logoScale = 0.0f;

	// 3. もしロゴ表示にタイマーも併用している場合はリセット（使っていなければ不要）
	m_logoTimer = 0.0f;

	// 4. 「強化する項目〜」の見出しテキストの座標も、画面外（上空）の初期位置に戻す
	m_headerY = -100.0f;
	m_headerTargetY = 150.0f;

	// 5. フラグ類のリセット（UIが閉じている状態から、再度アニメーションを動かす状態へ）
	m_isClosingUI = false;
	m_isAnimLvFinish = false;
	m_isAnimation = true; // UIのUpdateを再度動かすために true にする

	// 6. 3つの選択肢（スロット）の描画フラグをONに戻す
	// (前回のClosingフェーズで false にされたものを復活させます)
	for (auto& slot : m_displaySlots)
	{
		if (slot.freamImg)      slot.freamImg->setRenderEnable(true);
		if (slot.backGroundImg) slot.backGroundImg->setRenderEnable(true);
		if (slot.mainImg)       slot.mainImg->setRenderEnable(true);
	}
}
void UpgradeSelectionUI::HideUI()
{
	m_headerTargetY = -300.0f; // スロットより少し手前で消え始めるイメージ

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