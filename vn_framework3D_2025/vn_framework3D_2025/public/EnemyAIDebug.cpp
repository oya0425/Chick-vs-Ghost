#include "../framework.h"
#include "../framework/vn_environment.h"
EnemyAIDebug::EnemyAIDebug()
{

}

//パニック状態の表示
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

//特攻状態
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

//リーダー探し状態の表示
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


// 学習・強化の表示（一定時間だけ呼ぶ想定）
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

	// 描画ロジック（以前作った上昇演出）
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

void EnemyAIDebug::ShowStateArrow(const XMVECTOR& worldPos, const WCHAR* text, DWORD color)
{

	// 描画ロジック（以前作った上昇演出）
	XMVECTOR displayPos = XMVectorSetY(worldPos, XMVectorGetY(worldPos)+2.0f);

	float sx, sy;
	if (vnFont::CalculateScreenPosition(displayPos, &sx, &sy)) {
		vnFont::print(sx, sy, color, text);
	}
}