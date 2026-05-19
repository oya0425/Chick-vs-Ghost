#pragma once
class EnemyAIDebug 
{
public:
	EnemyAIDebug();
	~EnemyAIDebug() = default;

	struct DebugTextState {
		float timer = 0.0f;
		bool  isDone = false;

		// 状態が変わった時にリセットするための関数
		void Reset(float duration = 1.0f) {
			timer = duration;
			isDone = false;
		}
	};

	//パニック状態の表示
	static void ShowPanic(const XMVECTOR& worldPos,float timer);

	//特攻状態
	static void ShowCharge(const XMVECTOR& worldPos);
	
	//リーダー探し状態の表示
	static void ShowSearching(const XMVECTOR& worldPos);


	// 学習・強化の表示（一定時間だけ呼ぶ想定）
	static void ShowUpgrade(const XMVECTOR& worldPos, const WCHAR* text);

	//時間制限付きで表示する
	static void ShowStateOnce(const XMVECTOR& worldPos, DebugTextState& state, float deltaTime, const WCHAR* text, DWORD color);

	//固定で表示する
	static void ShowStateArrow(const XMVECTOR& worldPos, const WCHAR* text, DWORD color);

private:


};