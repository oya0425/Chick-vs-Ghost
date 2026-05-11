//--------------------------------------------------------------//
//	"EnemyClass.h"												//
//		敵クラス												//
//													2025/02/09	//
//														Oya  	//
//--------------------------------------------------------------//
#pragma once
#include"../public/RigidbodyComponent.h"

class EnemyClass
{
public:
	EnemyClass();
	~EnemyClass();

	enum eState
	{
		Idel,		//待機状態
		Run,		//走る
		KnockBack,	//吹っ飛び中
		Dead,		//吹っ飛び終わり
	};


	void Spawn(const XMVECTOR& pos);//生成
	void DeSpawn();		//無効化（プールに戻す）
	void Update(float deltaTime);

	// --- モデルのセットと獲得 ---
	void SetModel(vnCharacter* model);
	vnCharacter* GetModel()const { return pModel; }

	RigidbodyComponent& GetRigidbody() { return rigidbody; }


	// 
	void SetPlayerPos(XMVECTOR* pos) { playerPos = pos; }	//プレイヤーの位置をセットする

	void SetWaveNum(int nowWave) { currentWave = nowWave; }	//現在のWAVEをセット



	// --- 向き ---
	void SetMoveDirection(const XMVECTOR& dir);


	// --- 移動 ---

	void StartKnockback(//吹っ飛びに関する事柄の初期化
		const XMVECTOR& dir,
		float dist,
		float time,
		float height);

	// --- 当たり判定用 ---
	void SetIsHitPlayer(bool hit) { isHit = hit; }	//プレイヤーに当たったか？(true当たった)
	bool GetIsHitPlayer() { return isHit; }

	// --- 状態渡し ---
	eState GetState() { return state; }

	void ChangeSpeed(float speed);


	// --- アクティブ状態か ---
	bool GetActive() { return isActive; }
	void SetActive(bool active) { isActive = active; }


private:
	vnCharacter* pModel = nullptr;	//モデル（今回BoxUnity）
	bool isActive;	//これが有効かどうか（true有効）


	// --- 位置 ---
	const XMVECTOR* playerPos;	//プレイヤーの位置

	int currentWave;		//現在のWAVE

	// --- 移動 ---
	float baseMoveSpeed;		//元の速度
	float boostSpeedMultiplier;	//加速値
	float maxMoveSpeed;	//最大速度（wave数×boostSpeedMultiplier）で行こうと思う

	bool isEscaping;	//逃走状態に入れるか？

	// --- 向き ---
	XMVECTOR moveDir = XMVectorZero();


	// --- 攻撃を受ける ---
	// 吹っ飛び挙動を管理する構造体
	struct Knockback
	{
		bool active = false;          // 吹っ飛び中か
		float timer = 0.0f;           // 経過時間
		float duration = 0.4f;        // 基本の吹っ飛び時間
		//XMVECTOR* startPos = nullptr;
		XMVECTOR startPos = XMVectorZero();
		XMVECTOR direction = XMVectorZero();
		float distance = 0.0f;        // まだ飛ばない
		float height = 0.0f;
	};
	Knockback knockback;

	eState state;	//現在状態


	// --- 物理 ---
	RigidbodyComponent rigidbody;

	bool isHit;	//playerに当たった

	// --- 当たった時の演出 --- 
	bool isRising;      // 上昇中か
	float targetHeight; // 到達したいY
	float  knockSpeed;	// 飛ぶ基準の値（５）
	float  randomKnockNum;	//ランダムに変化する値（高さをランダムにする）

};