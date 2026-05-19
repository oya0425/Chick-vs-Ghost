//--------------------------------------------------------------//
//	"playerClass.h"												//
//		プレイヤークラス										//
//													2025/02/01	//
//														Oya  	//
//--------------------------------------------------------------//
#pragma once
#include"vn_model.h"
#include "CharacterBase.h"
#include"../public/Bullet.h"


//--------------------------------------
// プレイヤーのクラス
//--------------------------------------
class NewPlayerClass:public CharacterBase
{
public:
	NewPlayerClass();
	~NewPlayerClass();

	void Update(float deltaTime)override;

	// --- モデルセット ---
	void SetMeteorModel(vnModel* model); //上の殻のモデルをセット
	vnModel* GetMeteorModel()const;	//上の殻のモデルを返す

	void SetUpKaraModel(vnModel* model); //上の殻のモデルをセット
	vnModel* GetUpKaraModel()const;	//上の殻のモデルを返す


	// --- フラグ ---
	bool requestDestroy = false;

	void SetPlayerMove(bool canMove);
	bool GetPlayerMove() { return m_isMove; }

	float GetBaseMoveSpeed() { return m_baseMoveSpeed; }

	//void SetIsGround(bool onGround) { isGroundHit = onGround; }
	//bool GetIsGround() { return isGroundHit; }


	// --- 殻を初期化 --- 
	void ResetMeteorModel(); //落ちてくる殻のリセット
	void ResetUpKara();		 //上の殻をリセット

	// --- 必殺 ---
	//隕石必殺
	void SpecialMeteor();
	void StartSpecialMeteor();	//隕石必殺開始

	//移動必殺
	void StartSpecialBoost();	//移動必殺開始


	// --- ステータス強化 ---
	void SetSpeedMultiplier(float multiplier);
	void SetRangeMultiplier(float multiplier);
	void SetPullMultiplier(float multiplier);
	void SetBounceMultiplier(int multiplier);
	void SetBulletSpeedMultiplier(float multiplier);


	float GetEffectiveRadius() override {
		return m_currentRadius; // Updateで計算している変動する半径を返す
	}



	// --- 弾撃ち ---
	void SetBulletClass(Bullet* bullet) { m_bullet = bullet; }


	// --- スキルの解放 ---
	void UnlockAreaAttackSkill(bool unlock) { m_isHaveAreaAtkSkill = unlock; }
	void UnlockPullAttackSkill(bool unlock) { m_isHavePullSkill = unlock; }
	
	// --- スキル持ってるかのGet ---
	bool GetIsHaveAreaAtkSkill()const { return m_isHaveAreaAtkSkill; }
	bool GetIsHavePullSkill()const { return m_isHavePullSkill; }

	//---スキルのクールタイムのGet
	float GetAreaAttackCoolTime()const { return m_areaAtkCoolTimer; }
	float GetAreaAttackMaxCoolTime()const { return m_areaAtkCoolTimeMax; }

	float GetPullAttackCoolTime()const { return m_pullCooldownTimer; }
	float GetPullAttackMaxCoolTime()const { return m_pullCooldownMax; }

	//レベルアップ時にジャンプ
	bool GetIsJump()const { return m_isJump; }
	void Jump();

	//レベルアップ中かどうか（スキルの時間の経過を止めるため）
	void SetIsLevelUp(bool isLevelUp) { isLevelUp = m_isLevelUp; }

	// --- レベルアップ演出用 ---
	void UpdateLevelUp();
	void FinishLevelUp();	//レベルアップが終わった
	void ResetSkillCounts()//WAVE終了時にカウントをリセットする
	{
		m_pullUseCount = 0;
		m_areaUseCount = 0;
	}

	// --- 敵の学習用 ---
	//スキルの発動回数
	int GetPullUseCount()const { return m_pullUseCount; }
	int GetAreaAttackCount()const { return m_areaUseCount; }


	//--スキルの発動中か
	// --- 範囲攻撃 ---
	bool CanAreaAttack()const override{return (m_isHaveAreaAtkSkill && (m_areaAttackState == eSkillState::READY));}
	bool IsAreaAttack()const override { return m_areaAttackState == eSkillState::ACTIVE; }//範囲攻撃を撃てるか(スキルを持っているか)
	float GetAreaAttackRadius()const override { return m_maxAttackRadius; }				//範囲攻撃の大きさ

	// --- 引き寄せ攻撃 ---
	bool IsPulling()const { return m_pullState == eSkillState::ACTIVE; }	//引き寄せ攻撃中か
	float GetPullRadius()const { return m_pullRadius; }//敵が範囲に入っているか判定するため


	// --- クールタイムのゲッター ---
	//float Getm_areaAtkCoolTimer

	// ---　入力関係(移動) ---
	enum eInputDir
	{
		NONE,
		FORWARD,
		BACK,
		LEFT,
		RIGHT,
	};

	// --- スキルの構成 ---
	enum class eSkillState
	{
		READY,		//使用可能
		ACTIVE,		//スキル発動中
		COOLDOWN,	//クールダウン
	};

private:
	// --- Update内整理用関数 ---
	//1.入力と移動ベクトル計算（カメラ方向への変換を含む）
	XMVECTOR CalculateInputVector();

	//2.物理・移動処理（ジャンプ、Rigidbody更新、座標反映）
	void HandlePhysicsAndMovement(XMVECTOR Input, float deltaTime);

	//3.スキル・攻撃処理
	void UpdateSkills(float deltaTime);

	//4.キャラクターの向きとアニメーション
	void UpdateVisuals(XMVECTOR Input, float deltaTime);

	//5.デバッグ
	void DrawDebugInfo();




	// --- モデル ---
	vnModel* m_pUpKara = nullptr;
	vnModel* m_pMeteorModel = nullptr;
	
	// --- 移動 ---
	const float m_baseMoveSpeed = 18.0f;
	float m_boostSpeedMultiplier;

	bool  m_isMove;//動けるかどうか（操作可能か？）
	bool  m_isJump;//ジャンプできるかどうか

	bool m_isMoving;	//移動中かどうか？

	XMVECTOR respawnPos = XMVectorSet(0, 5, 0, 0);


	// --- 必殺 ---
	float m_specialGauge;
	const float m_specialGaugeMax = 1.0f;
	const float m_specialGaugeMin = 0;
	const float m_needGaugeBoost = 0.4f;
	const float m_needGaugeMeteor = 0.7f;

	// --- 状態 ---
	bool  m_isCanBoost;	//移動必殺が打てるかどうか？
	float m_boostTime;
	const float m_boostTimeMax = 5.0f;

	bool m_isCanMeteor;	//隕石必殺が打てるかどうか？
	//隕石必殺演出変数
	bool m_isUpKara = false; // 殻が真上に上がる
	bool m_isForwardBig = false; // 正面上空へ＋巨大化
	bool m_isDownKara = false; // 落下

	// --- 範囲攻撃用 ---
	eSkillState m_areaAttackState = eSkillState::READY;
	bool  m_isHaveAreaAtkSkill		  = false;			//スキル獲得済みか？
	bool  m_isExpanding				  = false;			// 半径拡大中か？
	float m_currentRadius			  = 1.0f;			// 現在の半径
	float m_expandTimer				  = 0.0f;			// 拡大用タイマー
	//float m_maxAttackRadius		      = 2.0f;		// 最大半径
	float m_maxAttackRadius		      = 20.0f;		    // 最大半径
	const float m_defaultAttackRadius = 10.0f;			//攻撃範囲のデフォルトのサイズ
	const float m_defaultRadius		  = 1.0f;			// 通常時の半径（size.x/2の値に合わせて調整）普通の当たり判定
	float m_areaAtkCoolTimer		  = 0.0f;			//現在のクールタイム
	const float m_areaAtkCoolTimeMax  = 10.0f;			//最大のクールタイム
	const float m_attackTime		  = 0.5f;			//範囲攻撃の最大まで行く時間

	void UpdateAreaAttackSkill(float deltaTime);

	// --- 引き寄せ攻撃 ---
	eSkillState m_pullState			= eSkillState::READY;	//現在の状態
	bool  m_isHavePullSkill			= false;				//スキル獲得済みか？
	const float m_defaultPullRadius = 15.0f;				//デフォルトの引き寄せ範囲
	float m_pullRadius				= 15.0f;			    //引き寄せ範囲
	float m_pullTimer				= 0.0f;				    //吸引時間の計測用
	float m_pullCooldownTimer		= 0.0f;				    //クールタイムの計測用

	const float m_pullDuration	  = 0.5f;  //吸引し続ける時間
	const float m_pullCooldownMax = 10.0f; //クールタイム

	void UpdatePullSkill(float deltaTime);	//引き寄せスキルの更新ロジック

	// --- 球撃ち攻撃 ---
	eSkillState m_ShootState = eSkillState::READY;
	//弾のデータ配列
	Bullet *m_bullet=nullptr;

	//弾のモデル
	float m_shootCooldownTimer = 0.0f;	//次に打てるまでの残り時間
	const float m_shootCooldownMax = 10.0f;//インターバル

	void UpdateBulletAttack(float deltaTime);


	// --- 移動入力用 ---
	eInputDir m_lastInput = NONE;


	bool m_isLevelUp = false;	//レベルアップ中はスキルの時間を止める

	// --- 敵の学習用 ---
	//--スキルの発動回数
	// 　//移動での攻撃は移動中かを取る・移動中のスキル発動はスキルの発動を優先
	int m_pullUseCount = 0;	//引き寄せ攻撃の発動回数
	int m_areaUseCount = 0;	//範囲攻撃の発動回数


};
