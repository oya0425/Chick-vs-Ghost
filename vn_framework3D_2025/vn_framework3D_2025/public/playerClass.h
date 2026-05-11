//--------------------------------------------------------------//
//	"playerClass.h"												//
//		プレイヤークラス										//
//													2025/02/01	//
//														Oya  	//
//--------------------------------------------------------------//
#pragma once
#include"vn_model.h"
#include"../public/RigidbodyComponent.h"

//--------------------------------------
// プレイヤーのクラス
//--------------------------------------
constexpr float GROUND_OFFSET = 0.1f;
class PlayerClass
{
public:
	PlayerClass();
	~PlayerClass();

	void Update(float deltaTime);

	void SetModel(vnCharacter* model);
	vnCharacter* GetModel() const;	//プレイヤーのモデルを返す

	void SetMeteorModel(vnModel* model); //上の殻のモデルをセット
	vnModel* GetMeteorModel()const;	//上の殻のモデルを返す

	void SetUpKaraModel(vnModel* model); //上の殻のモデルをセット
	vnModel* GetUpKaraModel()const;	//上の殻のモデルを返す


	void SetMoveDirection(const XMVECTOR& dir);

	bool requestDestroy = false;

	void SetPlayerMove(bool canMove);
	bool GetPlayerMove() { return isMove; }

	RigidbodyComponent& GetRigidbody() { return rigidbody; }

	float GetBaseMoveSpeed() { return baseMoveSpeed; }

	void SetIsJump(bool canJump) { isJump = canJump; }
	bool GetIsJump() { return isJump; }

	void SetIsGround(bool onGround) { isGroundHit = onGround; }
	bool GetIsGround() { return isGroundHit; }


	// --- 殻を初期化 --- 
	void ResetMeteorModel(); //落ちてくる殻のリセット
	void ResetUpKara();		 //上の殻をリセット

	// --- 必殺 ---
	//隕石必殺
	void SpecialMeteor();
	void StartSpecialMeteor();	//隕石必殺開始

	//移動必殺
	void ChangeSpeed(float speed);
	void StartSpecialBoost();	//移動必殺開始


	// --- HP関連の関数 ---
	float getCurrentHp() const { return currentHP; }
	float getMaxHp() const { return maxHP; }
	void  addHP(float addhp);


	// ダメージ処理（死んだら true を返すようにすると便利）
	bool applyDamage(float damage) {
		currentHP -= damage;
		if (currentHP <= 0) {
			currentHP = 0;
			isDead = true;
			return true;
		}
		return false;
	}

	// 回復処理
	void recoverHp(float amount) {
		currentHP += amount;
		if (currentHP > maxHP) currentHP = maxHP;
	}


private:
	// --- モデル ---
	vnCharacter* pModel = nullptr;
	vnModel* pUpKara;
	// --- 移動 ---
	float baseMoveSpeed;
	float boostSpeedMultiplier;

	bool  isMove;//動けるかどうか（操作可能か？）
	bool  isJump;//ジャンプできるかどうか

	bool isMoving;	//移動中かどうか？

	// --- 床についてるか ---
	bool isGroundHit;

	// --- 必殺 ---
	float specialGauge;
	float specialGaugeMax;
	float specialGaugeMin;
	float needGaugeBoost;
	float needGaugeMeteor;

	// --- 状態 ---
	bool isCanBoost;	//移動必殺が打てるかどうか？
	float boostTime;
	float boostTimeMax;

	vnModel* pMeteorModel = nullptr;
	bool isCanMeteor;	//隕石必殺が打てるかどうか？
	//隕石必殺演出変数
	bool isUpKara = false; // 殻が真上に上がる
	bool isForwardBig = false; // 正面上空へ＋巨大化
	bool isDownKara = false; // 落下

	// --- HP ---
	float currentHP;
	float maxHP;
	float hpDecreasePerTime;
	bool isDead;

	// --- 向き ---
	XMVECTOR moveDir = XMVectorZero();

	// --- 物理 ---
	RigidbodyComponent rigidbody;

};
