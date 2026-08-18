#include"../framework.h"
#include"../framework/vn_environment.h"

vnMotionData* motion_idle_bird;

namespace 
{
	// --- UpDate ---
	//--モーション
	constexpr float normalSpeed_motion = 1.0f;
	constexpr float downKaraNormalRotSpeed_motion = 0.1f;
	constexpr float downKaraBoostRotSpeed_motion = 0.5f;

	constexpr float boxColSize = 2.4f;

	constexpr float underRespawnPos = -10.0f;

	bool KeySpace = vnKeyboard::trg(DIK_SPACE);
	bool MouseL = vnMouse::trgL();
}
NewPlayerClass::NewPlayerClass()
	:CharacterBase(),
	m_isMove(true),
	m_isJump(false),
	m_isMoving(false),
	m_boostSpeedMultiplier(1.0f)
{

	SetMaxHp(100);
	//SetCurrentHP(50);

	GetCollision().SetSize(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)*boxColSize);
	GetCollision().SetCenter(XMVectorSet(0, boxColSize / 2, 0, 0));
	motion_idle_bird = loadMotionFile(L"data/model/Brid/brid_animation_new/motion/Brid_Idle1.mot");

}
//======================================================================
// --- ステータス初期化 ---
//======================================================================
void NewPlayerClass::Init()
{
	//スキル関係
	//範囲攻撃
	m_areaAtkCoolTimer = 0.0f;
	m_areaUseCount = 0;
	m_areaAttackState = eSkillState::READY;
	m_isHaveAreaAtkSkill = false;
	m_isExpanding = false;
	m_currentRadius = 1.0f;
	m_expandTimer = 0.0f;			// 拡大用タイマー
	m_maxAttackRadius = 20.0f;		

	// --- 引き寄せ攻撃 ---
	m_pullState = eSkillState::READY;	//現在の状態
	m_isHavePullSkill = false;				//スキル獲得済みか？
	m_pullRadius = 20.0f;			    //引き寄せ範囲
	m_pullTimer = 0.0f;				    //吸引時間の計測用
	m_pullCooldownTimer = 0.0f;				    //クールタイムの計測用
	
	m_isLevelUp = false;	//レベルアップ中はスキルの時間を止める
	GetModel()->setMotion(NULL);

	GetModel()->setPosition(&respawnPos);
	GetModel()->setRotationY(0);

	SetCurrentHP(100);
}

//======================================================================
// --- スキルのクールタイムのリセット ---
//======================================================================
void NewPlayerClass::ResetSkillCoolTime()
{
	m_areaAtkCoolTimer = 0.0f;
	m_pullCooldownTimer = 0.0f;
}

//上の殻のモデルセット
void NewPlayerClass::SetUpKaraModel(vnModel* model)
{
	m_pUpKara = model;
	ResetUpKara();
}


vnModel* NewPlayerClass::GetUpKaraModel()const
{
	return m_pUpKara;
}


void NewPlayerClass::SetPlayerMove(bool canMove)
{
	m_isMove = canMove;
}



//======================================================================
// --- 更新 ---
//======================================================================
void NewPlayerClass::Update(float deltaTime)
{
	vnCharacter* pModel = GetModel();
	if (!pModel)return;

	//入力と移動ベクトル計算（カメラ方向を変換含む）
	XMVECTOR vInput = CalculateInputVector();

	//物理・移動処理（ジャンプ、Rigidbody更新、座標反映）
	HandlePhysicsAndMovement(vInput, deltaTime);


	//スキル・攻撃処理
	UpdateSkills(deltaTime);



	//キャラクターの向きとアニメーション
	UpdateVisuals(vInput, deltaTime);

	//デバッグ
	DrawDebugInfo();

}


//======================================================================
// --- 移動入力 ---
//======================================================================
XMVECTOR NewPlayerClass::CalculateInputVector()
{
	if (!m_isMove)return XMVectorZero();
	XMVECTOR vInput = XMVectorZero();
	if (vnKeyboard::on(DIK_W)) vInput = XMVectorSetZ(vInput, -1.0f);
	if (vnKeyboard::on(DIK_S)) vInput = XMVectorSetZ(vInput, 1.0f);
	if (vnKeyboard::on(DIK_A)) vInput = XMVectorSetX(vInput, 1.0f);
	if (vnKeyboard::on(DIK_D)) vInput = XMVectorSetX(vInput, -1.0f);

#pragma region カクカク動く用
	// --- カクカク動く感じ（斜め移動ができない）--------------------------------
//if(vnKeyboard::on(DIK_W)||
//   vnKeyboard::on(DIK_S)||
//   vnKeyboard::on(DIK_A)||
//   vnKeyboard::on(DIK_D))
//{
//	if (vnKeyboard::trg(DIK_W)) m_lastInput = FORWARD;
//	if (vnKeyboard::trg(DIK_S)) m_lastInput = BACK;
//	if (vnKeyboard::trg(DIK_A)) m_lastInput = LEFT;
//	if (vnKeyboard::trg(DIK_D)) m_lastInput = RIGHT;

//	// 押されてない方向なら再判定
//	if (m_lastInput == FORWARD && !vnKeyboard::on(DIK_W))m_lastInput = NONE;
//	if (m_lastInput == BACK && !vnKeyboard::on(DIK_S))	 m_lastInput = NONE;
//	if (m_lastInput == LEFT && !vnKeyboard::on(DIK_A))   m_lastInput = NONE;
//	if (m_lastInput == RIGHT && !vnKeyboard::on(DIK_D))  m_lastInput = NONE;
//	// NONEなら他の押されてるキーにフォールバック
//	if (m_lastInput == NONE)
//	{
//		if (vnKeyboard::on(DIK_W))		m_lastInput = FORWARD;
//		else if (vnKeyboard::on(DIK_S)) m_lastInput = BACK;
//		else if (vnKeyboard::on(DIK_A)) m_lastInput = LEFT;
//		else if (vnKeyboard::on(DIK_D)) m_lastInput = RIGHT;
//	}
//}
//else
//{
//	m_lastInput = NONE;
//}

//switch (m_lastInput)
//{
//case NewPlayerClass::NONE:
//	break;
//case NewPlayerClass::FORWARD:
//	if (vnKeyboard::on(DIK_W))
//		vInput = XMVectorSetZ(vInput, -1.0f);
//	break;
//case NewPlayerClass::BACK:
//	if (vnKeyboard::on(DIK_S))
//		vInput = XMVectorSetZ(vInput, 1.0f);

//	break;
//case NewPlayerClass::LEFT:
//	if (vnKeyboard::on(DIK_A))
//		vInput = XMVectorSetX(vInput, 1.0f);

//	break;
//case NewPlayerClass::RIGHT:
//	if (vnKeyboard::on(DIK_D))
//		vInput = XMVectorSetX(vInput, -1.0f);

//	break;
//default:
//	break;
//}

// --------------------------------------------

#pragma endregion

	if (!XMVector3Equal(vInput, XMVectorZero()))
	{

		vInput = XMVector3Normalize(vInput);
		m_isMoving = true;

		// --- カメラ基準回転 ---
		XMVECTOR cam_pos = *vnCamera::getPosition();
		XMVECTOR cam_trg = *vnCamera::getTarget();
		XMVECTOR dist = XMVector3Normalize(cam_pos - cam_trg);
		float camRotY = atan2f(XMVectorGetX(dist), XMVectorGetZ(dist));
		XMMATRIX rotMtx = XMMatrixRotationY(camRotY);

		vInput = XMVector3TransformNormal(vInput, rotMtx);

	}
	else {
		m_isMoving = false;
	}
	return vInput;

}




//======================================================================
// --- ジャンプ入力や物理 ---
//======================================================================
void NewPlayerClass::HandlePhysicsAndMovement(XMVECTOR vInput, float deltaTime)
{
	bool currentJumpInput = vnKeyboard::trg(DIK_SPACE) || vnMouse::trgL();
	//ジャンプ入力
	if (currentJumpInput)
	{
		Jump(false);
	}

	//速度設定
	if (m_isMoving || GetRigidbody().GetIsGround())
	{
		GetRigidbody().SetBaseVelocity(vInput * m_baseMoveSpeed * m_boostSpeedMultiplier);
	}
	GetRigidbody().Update(deltaTime);

	XMVECTOR move = GetRigidbody().getMoveDelta();
	GetModel()->addPosition(&move);

	//着地判定
	if (GetRigidbody().GetIsGround())
	{
		if (m_isJump)
		{
			m_isJump = false;
			GetRigidbody().SetVerticalVelocity(0.0f);
			GetRigidbody().SetIsUseGravity(false);
		}
	}
	else
	{
		GetRigidbody().SetIsUseGravity(true);
	}

	//落下リスポーン
	if (GetModel()->getPositionY() < underRespawnPos)
	{
		GetModel()->setPosition(&respawnPos);
	}

}




//======================================================================
// --- スキル ---
//======================================================================
void NewPlayerClass::UpdateSkills(float deltaTime)
{
	if (m_isLevelUp)return;
	//範囲攻撃
	UpdateAreaAttackSkill(deltaTime);

	//弾攻撃
	//UpdateBulletAttack(deltaTime);
	//m_bullet->Update(deltaTime);

	//引き寄せ攻撃
	UpdatePullSkill(deltaTime);

}



//======================================================================
// --- モーション ---
//======================================================================
void NewPlayerClass::UpdateVisuals(XMVECTOR vInput, float deltaTime)
{
	vnCharacter* pModel = GetModel();

	//向き
	if (!XMVector3Equal(vInput, XMVectorZero()))
	{
		SetMoveDirection(vInput);
		float rotY = atan2f(XMVectorGetX(vInput), XMVectorGetZ(vInput));
		pModel->setRotationY(rotY);
	}
	//アニメーション
	if (!m_isMoving && m_isMove)
	{
		pModel->setMotion(motion_idle_bird);
		pModel->execute(normalSpeed_motion, false, false);

	}
	else
	{
		pModel->setMotion(NULL);

		auto* pDownKara = pModel->getParts("KaraDown");
		if (pDownKara)
		{
			float rotSpeed = downKaraNormalRotSpeed_motion;
			pDownKara->addRotationY(rotSpeed);
		}

	}
}




//======================================================================
// --- ジャンプ ---
//======================================================================
void NewPlayerClass::Jump(bool isLevelUping)
{
	if (!m_isJump)
	{
		//レベルアップ画面では音は鳴らさない
		if (!isLevelUping)
		{
			//ジャンプしたときになる音
			m_sound->PlaySE(SE_JUMP);
		}
		m_isJump = true;
		GetRigidbody().SetIsGround(false);
		GetRigidbody().AddVerticalVelocity(18.0f);
	}
	//着地判定
	if (GetRigidbody().GetIsGround())
	{
		if (m_isJump)
		{
			m_isJump = false;
			GetRigidbody().SetVerticalVelocity(0.0f);
			GetRigidbody().SetIsUseGravity(false);
		}
	}
	else
	{
		GetRigidbody().SetIsUseGravity(true);
	}
	GetRigidbody().Update(vnScene::getDeltaTime());

}


//======================================================================
// --- 範囲攻撃スキル ---
//======================================================================
void NewPlayerClass::UpdateAreaAttackSkill(float deltaTime)
{

	// スキルを持っていないなら、これ以降の処理を一切やらない
	if (!m_isHaveAreaAtkSkill) return;
	if (m_pullState == eSkillState::ACTIVE)return;
	switch (m_areaAttackState)
	{
	case eSkillState::READY:
	{
		bool keyE = vnKeyboard::trg(DIK_E);

		// --- 入力検知 ---
		if ((keyE && !m_isExpanding) /*|| (vnMouse::trgR() && !m_isExpanding)*/)
		{
			//範囲攻撃時に音を鳴らす
			m_sound->PlaySE(SE_AREA_ATTACK);
			m_areaUseCount++;

			m_isExpanding = true;
			m_expandTimer = 0.0f;
			m_currentRadius = m_defaultRadius;
			m_areaAttackState = eSkillState::ACTIVE;

		}

	}

	break;
	case eSkillState::ACTIVE:
		// --- 半径拡大ロジック ---
		if (m_isExpanding)
		{
			Common::StartCameraShake(3.5f, 3.5f, 0.5f);

			m_expandTimer += deltaTime;

			// 1秒かけて m_defaultRadius から m_maxAttackRadius まで線形補間(Lerp)
			// t = 0.0 ～ 1.0 になるように計算
			float t = m_expandTimer / m_attackTime;
			if (t > m_attackTime) t = m_attackTime;

			// 半径を更新
			m_currentRadius = m_defaultRadius + (m_maxAttackRadius - m_defaultRadius) * t;
			XMVECTOR ropecenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
			DWORD debugColor = m_isExpanding ? GAME_COLOR_SILVER : GAME_COLOR_LIME; // 攻撃中は色を変化する
			vnDebugDraw::Sphere(ropecenter, m_currentRadius, debugColor);

			// 1秒経過（最大サイズに到達）したらリセット
			if (m_expandTimer >= m_attackTime)
			{
				m_isExpanding = false;
				m_currentRadius = m_defaultRadius; // 元に戻す
				m_expandTimer = 0.0f;
				m_areaAtkCoolTimer = m_areaAtkCoolTimeMax;
				m_areaAttackState = eSkillState::COOLDOWN;
			}
		}

		break;
	case eSkillState::COOLDOWN:
		m_areaAtkCoolTimer -= deltaTime;
		if (m_areaAtkCoolTimer <= 0)
		{
			//スキル回復時に音を出す
			m_sound->PlaySE(SE_SKILL_HEAL);
			m_areaAttackState = eSkillState::READY;

		}

		break;
	}
}


//======================================================================
// --- 引き寄せスキル ---
//======================================================================
void NewPlayerClass::UpdatePullSkill(float deltaTime)
{
	// スキルを持っていないなら、これ以降の処理を一切やらない
	if (!m_isHavePullSkill) return;
	if (m_areaAttackState == eSkillState::ACTIVE)return;

	switch (m_pullState)
	{
	case eSkillState::READY:
	{
		//引き寄せの表示（線を円に描く）
		XMVECTOR ropecenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
		vnDebugDraw::DrawSuctionEffect(ropecenter, m_pullRadius, m_pullDuration, GAME_COLOR_LIGHT_BLUE,false,0);
		vnDebugDraw::DrawSuctionLines(ropecenter, m_pullRadius, m_pullDuration, GAME_COLOR_LIGHT_BLUE, GAME_COLOR_WHITE,false);

		//入力判定
		if (vnKeyboard::trg(DIK_Q))
		{
			//引き寄せ攻撃時に音を鳴らす
			m_sound->PlaySE(SE_PULL_ATTACK);
			m_pullUseCount++;
			m_pullState = eSkillState::ACTIVE;
			m_pullTimer = m_pullDuration;

		}
		break;

	}

	case eSkillState::ACTIVE:
		m_pullTimer -= deltaTime;

		if (m_pullTimer <= 0)
		{

			m_pullState = eSkillState::COOLDOWN;
			m_pullCooldownTimer = m_pullCooldownMax;
			//m_pullCooldownTimer = 0;
			Common::StartCameraShake(3.5f, 3.5f, 1.0f);

		}
		else
		{
			XMVECTOR ropecenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
			vnDebugDraw::DrawSuctionEffect(ropecenter, m_pullRadius, m_pullTimer, GAME_COLOR_BLUE,true,0);
			vnDebugDraw::DrawSuctionEffect(ropecenter, m_pullRadius, m_pullTimer, GAME_COLOR_CYAN,true,3);
		}

		break;
	case eSkillState::COOLDOWN:
		m_pullCooldownTimer -= deltaTime;
		if (m_pullCooldownTimer <= 0)
		{
			//スキル回復時に音を出す
			m_sound->PlaySE(SE_SKILL_HEAL);

			m_pullState = eSkillState::READY;
		}

		break;
	}
}


//======================================================================
// --- 弾撃つ攻撃 ---
//======================================================================
//======================================================================
void NewPlayerClass::UpdateBulletAttack(float deltaTime)
{
	switch (m_ShootState)
	{
	case eSkillState::READY:
		if (vnKeyboard::trg(DIK_R))
		{
			if (m_bullet)
			{
				m_bullet->SetIsActive(true);
				m_bullet->Shoot(*GetModel()->getPosition(),GetMoveDir(),GetModel()->getRotationY());
			}
			m_ShootState = eSkillState::COOLDOWN;
			m_shootCooldownTimer = m_shootCooldownMax;
		}
		break;

	case eSkillState::COOLDOWN:
		m_shootCooldownTimer -= deltaTime;
		if (m_shootCooldownTimer <= 0)
		{
			m_ShootState = eSkillState::READY;
		}
		break;
	}
}



//======================================================================
// --- 上の殻をプレイヤーの頭の上にそろえる ---
//======================================================================
void NewPlayerClass::ResetUpKara()
{
	if (!m_pUpKara || !GetModel())return;
	m_pUpKara->setRenderEnable(true);
	m_pUpKara->setParent(GetModel()->getParts("Body"));
	m_pUpKara->setPositionY(0.1f);
	m_pUpKara->setPositionZ(-0.5f);
	m_pUpKara->setScale(1, 1, 1);
	m_pUpKara->setRotationY(0);
}



//======================================================================
// --- 経験値で変化する用のやつ ---
//======================================================================

//移動速度
void NewPlayerClass::SetSpeedMultiplier(float multiplier)
{
	// multiplier が 1.1f なら 10% アップの状態
	m_boostSpeedMultiplier = multiplier;
}
//範囲攻撃の範囲
void NewPlayerClass::SetRangeMultiplier(float multiplier)
{
	m_maxAttackRadius = m_defaultAttackRadius * multiplier;
}
//引き寄せる範囲
void NewPlayerClass::SetPullMultiplier(float multiplier)
{
	m_pullRadius = m_defaultPullRadius * multiplier;
}
//弾の反射する回数
void NewPlayerClass::SetBounceMultiplier(int multiplier)
{
	m_bullet->SetMaxBounce(multiplier);
}
//弾の速度
void NewPlayerClass::SetBulletSpeedMultiplier(float multiplier)
{
	m_bullet->SetSpeed(multiplier);
}

//======================================================================
// --- レベルアップ画面での行動 ---
//======================================================================
void NewPlayerClass::UpdateLevelUp()
{
	m_isLevelUp = true;
	//ジャンプする
		//プレイヤーがジャンプするようにする
	if (!GetIsJump()) {
		SetPlayerMove(false); // これでキー入力による移動ベクトルが 0 になる
		Jump(true);
	}
	Update(vnScene::getDeltaTime());


}
void NewPlayerClass::FinishLevelUp()
{
	m_isLevelUp = false;
	SetPlayerMove(true);
}


NewPlayerClass::~NewPlayerClass()
{
}


void NewPlayerClass::DrawDebugInfo()
{
}
