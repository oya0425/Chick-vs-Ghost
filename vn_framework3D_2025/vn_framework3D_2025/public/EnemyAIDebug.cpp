#include "../framework.h"
#include "../framework/vn_environment.h"

EnemyAIDebug::EnemyAIDebug()
{

}
//======================================================================
// --- パニック状態の表示 ---
//======================================================================
void EnemyAIDebug::ShowPanic(const XMVECTOR& worldPos,float timer)
{
	if (timer <= 0.0f) return;
	float floatUp = (1.0f - timer) * 2.0f;
	float sx = 0, sy = 0;
	//座標を入れる
	XMVECTOR pos = XMVectorSetY(worldPos, XMVectorGetY(worldPos) +floatUp);
	pos = XMVectorSetX(pos, XMVectorGetX(pos) - 2.0f);
	if (vnFont::CalculateScreenPosition(pos, &sx, &sy))
	{
		vnFont::print(sx, sy, GAME_COLOR_BLUE, L"リーダー！");
	}

}
//======================================================================
// --- 特攻状態 --- 
//======================================================================
void EnemyAIDebug::ShowCharge(const XMVECTOR& worldPos)
{
	float sx = 0, sy = 0;
	//座標を入れる
	XMVECTOR pos = XMVectorSetY(worldPos, XMVectorGetY(worldPos) + 4.0f);
	pos = XMVectorSetX(pos, XMVectorGetX(pos) - 2.0f);

	if (vnFont::CalculateScreenPosition(pos, &sx, &sy))
	{
		vnFont::print(sx, sy, GAME_COLOR_BLUE, L"特攻！");
	}

}
//======================================================================
// --- リーダー探し状態の表示 ---
//======================================================================
void EnemyAIDebug::ShowSearching(const XMVECTOR& worldPos)
{
	float sx = 0, sy = 0;
	//座標を入れる
	XMVECTOR pos = XMVectorSetY(worldPos, XMVectorGetY(worldPos) + 4.0f);
	pos = XMVectorSetX(pos, XMVectorGetX(pos) - 2.0f);

	if (vnFont::CalculateScreenPosition(pos, &sx, &sy))
	{
		vnFont::print(sx, sy, GAME_COLOR_BLUE, L"リーダーどこやねん？");
	}

}

//======================================================================
// --- 学習・強化の表示 ---
//======================================================================
void EnemyAIDebug::ShowUpgrade(const XMVECTOR& worldPos, const WCHAR* text,DWORD color) {
	float sx, sy;
	XMVECTOR pos = XMVectorSetY(worldPos, XMVectorGetY(worldPos)); // 状態より少し上
	pos = XMVectorSetX(pos, XMVectorGetX(pos) - 3.0f);

	if (vnFont::CalculateScreenPosition(pos, &sx, &sy)) {
		vnFont::print(sx, sy, color, text);
	}
}

void EnemyAIDebug::ShowStateOnce(const XMVECTOR& worldPos, DebugTextState& state, float timer, const WCHAR* text, DWORD color)
{
	// すでに表示が終わっているなら何もしない
	//if (state.isDone) return;

	// タイマーを更新
	state.timer = timer;

	state.timer -= vnScene::getDeltaTime();

	// 文字が上に流れていく感じ
	float floatUp = (1.0f - state.timer) * 2.0f;
	XMVECTOR displayPos = XMVectorSetY(worldPos, XMVectorGetY(worldPos) + floatUp + 3.0f);
	displayPos = XMVectorSetX(displayPos, XMVectorGetX(worldPos) - 3.0f);

	float sx, sy;
	if (vnFont::CalculateScreenPosition(displayPos, &sx, &sy)) {
		vnFont::print(sx, sy, color, text);
	}

	// 終わったらフラグを立てる
	if (state.timer <= 0.0f) {
		state.isDone = true;
	}

}
//======================================================================
// --- ３D空間のものと同じ場所に文字を表示 ---
//======================================================================
void EnemyAIDebug::ShowStateArrow(const XMVECTOR& worldPos, const WCHAR* text, DWORD color)
{
	XMVECTOR displayPos = XMVectorSetY(worldPos, XMVectorGetY(worldPos)+2.0f);

	float sx, sy;
	if (vnFont::CalculateScreenPosition(displayPos, &sx, &sy)) {
		vnFont::print(sx, sy, color, text);
	}
}

//======================================================================
// --- ボスの位置を知らせる矢印の設定 ---
//======================================================================
void EnemyAIDebug::ShowBossDirectionArrow(
    const XMVECTOR& bossWorldPos,
	IDWriteTextFormat* pFormat)
{
    float bossX;
    float bossY;

    // ボスをスクリーン座標へ変換
	// ボスが画面外の時にのみ表示する
    if (!vnFont::CalculateScreenPosition(
        bossWorldPos, &bossX, &bossY))
    {
        return;
    }

    float screenW = (float)vnMainFrame::screenWidth;
    float screenH = (float)vnMainFrame::screenHeight;

    // ボスが画面内なら矢印を表示しない
    if (bossX >= 0.0f && bossX <= screenW &&
        bossY >= 0.0f && bossY <= screenH)
    {
        return;
    }

    // 画面中央からボス方向
    float centerX = screenW * 0.5f;
    float centerY = screenH * 0.5f;

    float dx = bossX - centerX;
    float dy = bossY - centerY;

    // 矢印を表示する位置
    float arrowX;
    float arrowY;

    const float margin = 100.0f;

    // X方向とY方向、どちらが画面端に先に到達するか
    float absX = fabsf(dx);
    float absY = fabsf(dy);

    if (absX > absY)
    {
        // 左右の端
        arrowX = (dx > 0.0f)
            ? screenW - margin
            : margin;

        arrowY =
            centerY + dy * (arrowX - centerX) / dx;
    }
    else
    {
        // 上下の端
        arrowY = (dy > 0.0f)
            ? screenH - margin
            : margin;

        arrowX =
            centerX + dx * (arrowY - centerY) / dy;
    }

    // 念のため画面内に収める
    arrowX = Common::MyClamp(
        arrowX,
        margin,
        screenW - margin);

    arrowY = Common::MyClamp(
        arrowY,
        margin,
        screenH - margin);

	// 方向によって矢印を変える
	const WCHAR* arrow = L"→";

	float angle = atan2f(dy, dx);

	if (angle >= -0.7854f && angle < 0.7854f)
	{
		// 右
		arrow = L"→";
	}
	else if (angle >= 0.7854f && angle < 2.3562f)
	{
		// 下
		arrow = L"↓";
	}
	else if (angle >= 2.3562f || angle < -2.3562f)
	{
		// 左
		arrow = L"←";
	}
	else
	{
		// 上
		arrow = L"↑";
	}

	vnFont::setFontSize(pFormat, 100);
	vnFont::print(
        arrowX,
        arrowY,
        GAME_COLOR_YELLOW,
        arrow);
}
