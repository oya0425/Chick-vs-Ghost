#pragma once
#include <string>
class NewEnemyClass :public CharacterBase
{
public:
	NewEnemyClass();
	~NewEnemyClass();

	// ----------------------------
	// 敵の種類・状態
	// ----------------------------

	// --- 敵の種類 ---
	enum class EnemyType {
		NONE,
		GHOST,
		MUSHROOM,
	};

	// --- 状態 ---
	enum eState
	{
		Idel,		//待機状態
		Run,		//走る
		KnockBack,	//吹っ飛び中
		Dead,		//吹っ飛び終わり
		Attracted,	//引き寄せられる
		Follow,		//リーダーを探す見つけたら追いかける（リーダーについていく）
		Panic,
		/*Panic(錯乱状態）
			→まずリーダーが倒される（登録されてるリーダーを外す）
			→近くのリーダーを範囲で探す（そこそこの範囲で（大体は見つかるはず））
			→それでも見つからなかったら錯乱状態に入る
			→錯乱状態で四方八方に移動する（この時にもリーダーを探す（狭い範囲で）見つかればそのリーダーにつく）
		*/

		Charge,
		/*
		Charge(特攻)
		→まず登録されてるリーダーが倒される（登録されてるリーダーを外す）
		→同じリーダーを失った群れ内全員がプレイヤーに向かってくる
		*/

		Patrol,	//リーダーの自然状態
	};

	//群れとしての統一するための状態
	enum class eGroupMode
	{
		Normal,	//通常（リーダーについていく）
		Panic,	//群れ全体がパニック
		Charge,	//群れ全体が特攻
	};

	//死因
	enum class DamageSource
	{
		NONE,
		Melee,		//近接攻撃
		AreaAttack,	//範囲攻撃
		PullAttack,	//引き寄せ攻撃
	};

	enum class eShowUISelect
	{
		Text1,	//例えば（範囲攻撃範囲に入った）
		Text2,	//（範囲攻撃外に出た）とか
		Text3,
		None,	//この状態の時にタイマーを戻す
	};

	// ----------------------------
	// データ構造体
	// ----------------------------

	// --- 敵のデータ ---
	struct EnemyData {
		EnemyType type;
		const WCHAR* folder;
		const WCHAR* file;
		int maxCount;		//生成する最大数
		float scale;		//モデルのサイズ
		XMFLOAT3 colSize;	//当たり判定の大きさ
	};

	//群れを色で識別
	struct GroupColorData
	{
		int id;				//群れとしての番号
		XMVECTOR color;		//群れの色

		wchar_t colorName[32]; // ここに追加（例: L"真紅", L"コバルトブルー"）
	};

	//群の学習データ
	struct GroupData
	{
		int id;	//群の番号
		XMVECTOR color;	//群の色

		const wchar_t* colorName;

		//学習
		float meleeFear = 0.0f;		//近接警戒(特攻確率をあげる)
		float rangeFear = 0.0f;		//基礎速度アップ
		//範囲攻撃警戒(逃げる速度を上げる)プレイヤーの範囲攻撃にこれを掛けて範囲攻撃に当たらないように
		float pullResistance = 0.0f;//引き寄せ耐性

		//一個前の記録上書きしていく比較用
		float oldMeleeFear = 0.0f;
		float oldRangeFear = 0.0f;
		float oldPullResistance = 0.0f;


		//学習値の最大
		float maxMeleeFear = 1.0f;
		float maxRangeFear = 10.0f;
		float maxPullResistance = 1.0f;

		float maxBossMeleeFear = 50.0f;
		float maxBossRangeFear = 20.0f;

		//状態
		int memberCount = 0;		//群の数
		bool isLeaderAlive = true;	//リーダーが生きているか

		bool isLeaderEscaping = false;	//プレイヤーから全力で逃げる（号令を出す）
	
		eGroupMode oldMode;
		eGroupMode mode;

	};


	struct MessageBanner
	{
		float remainingTime = 0.0f;               // 残り表示時間
		const float defaultDisplayDuration = 2.0f; // 基本の表示時間（定数化）
		eShowUISelect uiState = eShowUISelect::None;

		// 状態を変更するための関数
		void SetState(eShowUISelect newState)
		{
			// 同じ状態がセットされた場合は何もしない（タイマーをリセットしたくない場合）
			if (uiState == newState) return;

			uiState = newState;

			// 状態が変わった瞬間にタイマーを初期化
			// Noneに切り替わった時も、次に備えてリセットしておく
			remainingTime = defaultDisplayDuration;
		}

		// 更新処理
		void Update(float deltaTime)
		{
			if (uiState == eShowUISelect::None) return;

			remainingTime -= deltaTime;
			if (remainingTime <= 0.0f)
			{
				SetState(eShowUISelect::None);
			}
		}
	};

	// ----------------------------
	// staticデータ
	// ----------------------------

	// --- セットする色（リーダー）---
	static std::vector<NewEnemyClass::GroupColorData> g_LeaderColorPalette;

	// staticをつけることで「クラス共通のデータ」にする
	static const EnemyData MasterTable[];
	static const int MasterTableCount;

	static NewEnemyClass* CreateEnemyByType(EnemyType type);

	// ----------------------------
	// 生成・更新
	// ----------------------------

	// --- 出現・削除 ---
	void Spawn(const XMVECTOR& pos);//生成
	void DeSpawn();					//無効化（プールに戻す）

	// --- 更新 ---
	void Update(float deltaTime) override;

	// ----------------------------
	// ノックバック・引き寄せ
	// ----------------------------

	// --- ノックバック ---
	void StartKnockback(
		const XMVECTOR& dir,
		float dist,
		float time,
		float height);
	void UpdateKnockback(float deltaTime);

	// --- 引き寄せられ状態 ---
	void UpdateAttracted(float deltaTime, XMVECTOR toPlayer, float distance);
	bool IsAttracted() const { return m_state == eState::Attracted; }

	// プレイヤーのスキル情報を外から受け取って、状態を切り替える
	void CheckPullTrigger(bool isPlayerPulling, float pullRadius, float distance);

	// ----------------------------
	// プレイヤー関連
	// ----------------------------

	// --- プレイヤーの位置を獲得 ---
	void SetPlayerPos(CharacterBase* player) { m_pPlayer = player; }
	CharacterBase* GetPlayer() const { return m_pPlayer; }

	// --- 現在のWAVEを獲得 ---
	void SetWaveNum(int nowWave) { m_currentWave = nowWave; }

	// --- プレイヤーと当たったか ---
	void SetIsHitPlayer(bool hit) { m_isHitPlayer = hit; }
	bool GetIsHitPlayer() const { return m_isHitPlayer; }

	// ----------------------------
	// 状態管理
	// ----------------------------

	// --- 状態渡し ---
	void SetState(eState currentState)
	{
		m_state = currentState;
		m_aiDebugText.Reset(m_defaultTextTime);
	}

	eState GetState() const { return m_state; }

	// --- 特攻状態かどうか（引き寄せ攻撃用）---
	bool GetIsCharge() const { return m_isCharge; }

	// --- アクティブ状態か ---
	bool GetActive() { return m_isActive; }
	void SetActive(bool active) { m_isActive = active; }

	// ----------------------------
	// 敵情報
	// ----------------------------

	void ChangeSpeed(float speed);

	// --- 種類の獲得 ---
	EnemyType GetType() const { return m_type; }

	void SetUnlock(bool unlock) { m_isUnlocked = unlock; }
	bool IsUnlocked() const { return m_isUnlocked; }

	// ----------------------------
	// リーダー・群れ
	// ----------------------------

	// --- リーダー関連 ---
	bool GetIsLeader() const { return m_isLeader; }
	void SetIsLeader(bool isLeader) { m_isLeader = isLeader; }

	void SettingLeader(GroupData* groupData);
	void SettingOther();

	XMVECTOR GetColor() const { return GetModel()->GetAllPartsDiffuse(); }
	bool IsLeaderPanic() const { return m_leaderInTrouble; }

	// --- ボスのセッティング（初期化）（学習データを最初は入れておく、最後まで行ったときにデータを全部足す）---
	void SettingBoss(GroupData* groupData);
	bool GetIsBoss() const { return m_isBoss; }

	// --- 群れ ---
	//eGroupMode GetGroupMode() const { return m_currentGroupMode; }

	void SetGroupID(int id) { m_gruoupID = id; }
	int GetGroupID() const { return m_gruoupID; }

	//学習データのset
	void SetGroupData(GroupData* data)
	{
		m_pGroupData = data;
	}

	//学習データのget
	GroupData* GetGroupData()
	{
		return m_pGroupData;
	}

	// --- 群の色・番号取得 ---
	GroupColorData GetRandomGroupData();

	// ----------------------------
	// ステージ・移動補助
	// ----------------------------

	// --- ステージの半径（逃げるときに壁側を移動し続けないようにするため）
	void SetFenceRadius(float r) { m_myFenceRadius = r; }
	float GetFenceRadius() { return m_myFenceRadius; }

	float GetRandomFloat();
	float GetRandomRange(float min, float max);
	XMVECTOR GetRandomDirection();

	//壁（ブロック）に当たったかどうか
	void SetWallHit(bool hit) { m_isWallHit = hit; }
	bool GetWallHit() const { return m_isWallHit; }

	// ----------------------------
	// 学習・UI
	// ----------------------------

	// --- 敵の状態メッセージの表示 ---
	void UpdateEnemyMessage(float deltaTime);
	void ShowMessage();

	void OnDie(DamageSource source);

	// --- リスタートしたときに所持していた学習データを削除する ---
	void ReStartEnemy();

	// ----------------------------
	// マークUI
	// ----------------------------

	// ゲッターset（ビルボード）
	vnSprite* GetChargeMark() const { return m_pChargeMark; }
	vnSprite* GetPanicMark() const { return m_pPanicMark; }

	void SetPanicMark(vnSprite* sprite) { m_pPanicMark = sprite; }
	void SetChargeMark(vnSprite* sprite) { m_pChargeMark = sprite; }


protected:
	//==================================================
	// 状態ごとの処理（子クラス実装）
	//==================================================
	virtual void OnIdel(float deltaTime, float distance, const XMVECTOR& toPlayer) = 0;
	virtual void OnRun(float deltaTime, float distance, const XMVECTOR& toPlayer) = 0;
	virtual void OnDead() = 0;

	virtual void OnFollow(float deltaTime) = 0;	//リーダー以外の敵がリーダーを追う
	virtual void OnPanic(float deltaTime) = 0;	//パニック状態
	virtual void OnCharge(float deltaTime, const XMVECTOR& toPlayer) = 0;	//群れ全体が攻撃
	virtual void OnPatrol(float deltaTime, float distance) = 0;	//リーダーの徘徊

	//==================================================
	// 移動関連
	//==================================================
	void ApplyMovement(float deltaTime, const XMVECTOR moveDir);

	bool InPlayerArea(float escapeStartDist, float escapeStopDist);

	//==================================================
	// 基本ステータス
	//==================================================
	EnemyType m_type = EnemyType::NONE;	//タグ（敵の種類）
	bool m_isUnlocked;					//出現可能フラグ（false(出現不可)）

	//==================================================
	// 移動速度設定（子の初期化で設定する）
	//==================================================
	const float m_baseMoveSpeed = 1.0f;

	float m_waveBoostSpeedMultiplier = 1.0f;			//全体の敵に適応されるWAVEごとに早くなるやつ
	float m_boostSpeedMultiplier = 1.0f;				//それぞれの個体ではなく種全体の基本速度

	float m_leaderSpeedMultiplier = 1.0f;				//リーダーの追加速度
	float m_defalutLeaderSpeedMultiplier = 1.0f;		//リーダーの追加速度

	float m_otherSpeedMultiplier = 1.0f;				//リーダー以外の敵の速度
	float m_defalutOtherSpeedMultiplier = 1.0f;		//リーダー以外の敵の速度

	//==================================================
	// 群れ情報
	//==================================================
	NewEnemyClass* m_pMyLeader = nullptr;	//追いかけるリーダー個体
	bool		   m_isLeader = false;		//リーダーかどうか
	bool		   m_isBoss = false;		//ボスかどうか
	int			   m_gruoupID = 0;			//群れの番号

	//==================================================
	// リーダー追跡
	//==================================================
	float m_baseFollowDist = 1.0f;			//基準停止距離（リーダーとの距離）
	float m_myStopDist = 0.0f;				//個体ごとの停止距離（ばらつき用）
	float m_searchLeaderRadius = 100.0f;	//リーダー探索範囲

	//==================================================
	// パニック状態
	//==================================================
	float m_panicSearchRadius = 10.0f;		//錯乱時のリーダーを探す範囲

	XMVECTOR m_panicDir = XMVectorZero();	//錯乱時の進行方向

	float m_panicDirTimer = 0.0f;			//方向転換タイマー
	float m_panicRecoveryTime = 0.0f;		//パニック時に次にリーダーを探し始めるまでの時間
	float m_panicRecoveryStartTime = 0.5f;	//登場時にパニック状態にならないようにする

	//==================================================
	// 特攻状態
	//==================================================
	float m_chargeSpeedMultiplier = 1.2f;	//特攻時の速度倍率
	bool m_isCharge = false;				//引き寄せ攻撃時に特攻状態が消えるので記録しておく

	//==================================================
	// リーダー行動
	//==================================================
	float m_leaderSeparateRadius = 1.0f;		//リーダー同氏の反発距離
	float m_leaderEscapeRadius = 1.0f;			//プレイヤーから逃げる距離
	float m_leaderRetreatStopRadius = 1.0f;	//プレイヤーから十分に離れて逃げやめる距離

	//==================================================
	// パトロール
	//==================================================
	XMVECTOR m_patrolTargetPos = XMVectorZero();	//目的地

	float m_patrolWaitTimer = 0.0f;				//待ち時間タイマー
	bool m_isReachingTarget = true;				//目的地に到着したか

	float m_patrolSpeedMultiplier = 0.5f;		//徘徊時の速度倍率

	//==================================================
	// 逃走補助
	//==================================================
	float m_escapeAngleOffset = 0.0f;	//逃げる方向のズレ

	//==================================================
	// 色管理
	//==================================================
	XMVECTOR m_defaultLeaderColor = XMVectorZero();	//リーダーのデフォルトの色
	XMVECTOR m_defaultOtherColor = XMVectorZero();	//その他の敵のデフォルトの色

	//==================================================
	// デバッグUI
	//==================================================
	static constexpr float m_defaultTextTime = 2.0f;

	float m_panicTextTimer = m_defaultTextTime;

	EnemyAIDebug::DebugTextState m_aiDebugText;

	//==================================================
	// 調整変数（不具合防止用）
	//==================================================
	bool m_isPullChecked = false;			//引き寄せ攻撃の無効化の抽選用（全員が引き寄せられるのを防ぐ）
	float m_individualPullResist = 0.0f;	//個別に持つランダムな抵抗値、群れの中でもランダムにする

	bool m_wasLeaderEscaping;				//範囲攻撃の逃げるときにタイマーをリセットするため
	bool m_onceStartUI = false;				//登場したときの最初のみ出すUI

	//==================================================
	// メッセージUI
	//==================================================
	MessageBanner m_areaAtkMessage;		//範囲攻撃に関するメッセージ
	MessageBanner m_pullAtkMessage;		//引き寄せに関するメッセージ
	MessageBanner m_panicMessage;		//パニックに関するメッセージ（リーダー探し中も出す）
	MessageBanner m_chargeMessage;		//特攻に関するメッセージ
	MessageBanner m_patrolMessage;		//パトロールに関するメッセージ
	MessageBanner m_followMessage;		//リーダーを追跡に関するメッセージ
	MessageBanner m_runMessage;			//プレイヤーから逃走中メッセージ

	//==================================================
	// 頭上マークUI
	//==================================================
	vnSprite* m_pChargeMark;	//特攻状態の怒りマーク
	vnSprite* m_pPanicMark;		//焦りマーク


private:
	//==================================================
	// 基本情報
	//==================================================
	CharacterBase* m_pPlayer = nullptr;	// プレイヤーへのポインタ

	float m_myFenceRadius = 0.0f;		// 自身のフェンス半径
	int   m_currentWave = 0;			// 現在のWAVE

	bool  m_isActive = false;			// アクティブ状態か

	//==================================================
	// 状態管理
	//==================================================
	eState m_state = eState::Idel;
	//eGroupMode m_currentGroupMode = eGroupMode::Normal;

	bool m_isEscaping = false;			// 逃走中か
	bool m_leaderInTrouble = false;		// リーダーが異常（ノックバック・死亡）か

	//==================================================
	// 当たり判定・距離判定
	//==================================================
	float m_currentDistance = 0.0f;		// プレイヤーとの距離

	bool  m_isHitPlayer = false;		// プレイヤーと接触したか
	bool  m_isWallHit = false;			// 壁との衝突フラグ（子クラス参照用）
	bool  m_isJump = false;				// ジャンプ中か

	//==================================================
	// 群れ学習データ
	//==================================================
	GroupData* m_pGroupData = nullptr;

	//==================================================
	// 引き寄せ関連
	//==================================================
	const float m_pullSpeed = 50.0f;	// 引き寄せ速度（定数はm_推奨）

	//==================================================
	// ノックバック
	//==================================================
	struct sKnockbackData
	{
		bool     active = false;
		float    timer = 0.0f;
		float    duration = 0.4f;

		XMVECTOR startPos = XMVectorZero();
		XMVECTOR direction = XMVectorZero();

		float    distance = 0.0f;
		float    height = 0.0f;

	} m_kbData;

	//==================================================
	// ノックバック演出
	//==================================================
	bool  m_isRising = false;					// 上昇中か

	float m_targetHeight = 0.0f;				// 到達目標高さ（Y）

	const float m_baseKnockSpeed = 5.0f;		// 吹き飛び速度基準

	float m_randomKnockNum = 0.0f;				// 高さのランダム補正値
	float m_knockbackRotationPatternRnd = 0.0f;	// 回転軸決定用の乱数

	//==================================================
	// アニメーション
	//==================================================
	float m_livingTime = 0.0f;					// 生存累積時間（sin波計算用）

	XMVECTOR m_defaultScale = XMVectorZero();	// 元の大きさ

	float m_animSpeed = 10.0f;					// 伸び縮みの速さ（sinの中身に掛ける）
	float m_animMagnitude = 0.2f;				// 伸び縮みの強さ（sinの結果に掛ける）

	//==================================================
	// 色管理
	//==================================================
	static std::vector<GroupColorData> m_availableColors; // 使用可能な色のリスト

	//==================================================
	// 強化表示UI
	//==================================================
	bool m_isSpwanStart = false;

	float m_upgradeTimer = 3.0f;

	int m_upgradeStep = 0;		//強化された項目の数、表示するごとに増える

	//std::vector<std::wstring>m_upgradeTexts;	//進化した項目の文字リスト(強化した項目)
	std::vector<std::pair<std::wstring, unsigned int>> m_upgradeTexts;	//進化した項目の文字リスト(強化した項目)

	//==================================================
	// 内部処理
	//==================================================
	void Jump();

	void UpdateSquashAndStretch(float deltaTime);	//伸び縮みするアニメーション

	void CheckEvolutionOnSpawn();	//スポーン時に何が強化されたか出す

	void EscapeAreaAttack();		//範囲攻撃から逃げる関数

	//==================================================
	// 頭上マーク
	//==================================================
	void SetMark(vnSprite* sprite, bool isVisible);

	void UpdateMark();

	//==================================================
	// Update整理用
	//==================================================
	void UpdateState(
		float deltaTime,
		float distance,
		const XMVECTOR& toPlayer);

	void UpdateHitPlayer();

	void UpdatePhysics(float deltaTime);


};