#include"../framework.h"
#include"../framework/vn_environment.h"

extern vnMotionData* motion_idle_bird;

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
	m_isJump(true),
	m_isMoving(true),
	m_boostSpeedMultiplier(1.0f)
{
	m_boostTime = 0.0f;
	m_specialGauge = 0.0f;
	m_isCanBoost = false;
	m_isCanMeteor = false;

	SetMaxHp(100);
	//SetCurrentHP(50);

	GetCollision().SetSize(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)*boxColSize);
	GetCollision().SetCenter(XMVectorSet(0, boxColSize / 2, 0, 0));
	motion_idle_bird = loadMotionFile(L"data/model/Brid/brid_animation_new/motion/Brid_Idle1.mot");

}


//必殺の落ちてくる殻のモデルセット
void NewPlayerClass::SetMeteorModel(vnModel* model)
{
	m_pMeteorModel = model;
	ResetMeteorModel();
}

//上の殻のモデルセット
void NewPlayerClass::SetUpKaraModel(vnModel* model)
{
	m_pUpKara = model;
	ResetUpKara();
}


vnModel* NewPlayerClass::GetMeteorModel()const
{
	return m_pMeteorModel;
}
vnModel* NewPlayerClass::GetUpKaraModel()const
{
	return m_pUpKara;
}


void NewPlayerClass::SetPlayerMove(bool canMove)
{
	m_isMove = canMove;
}




void NewPlayerClass::Update(float deltaTime)
{
	vnCharacter* pModel = GetModel();
	if (!pModel)return;

	//1.入力と移動ベクトル計算（カメラ方向を変換含む）
	XMVECTOR vInput = CalculateInputVector();

	//2.物理・移動処理（ジャンプ、Rigidbody更新、座標反映）
	HandlePhysicsAndMovement(vInput, deltaTime);


	//3.スキル・攻撃処理
	UpdateSkills(deltaTime);

	//4.キャラクターの向きとアニメーション
	UpdateVisuals(vInput, deltaTime);

	//5.デバッグ
	DrawDebugInfo();

}



// --- 移動入力 ---
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

// --- ジャンプ入力や物理 ---
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

// --- スキル ---
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

// --- モーション ---
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
			float rotSpeed =
				m_isCanBoost ? downKaraBoostRotSpeed_motion : downKaraNormalRotSpeed_motion;
			pDownKara->addRotationY(rotSpeed);
		}

	}
}

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


void NewPlayerClass::DrawDebugInfo()
{

	//XMVECTOR ropecenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
	//DWORD debugColor = m_isExpanding ? GAME_COLOR_RED : GAME_COLOR_LIME; // 攻撃中は赤くする
	//vnDebugDraw::Sphere(ropecenter, m_currentRadius, debugColor);

	//const wchar_t* stateStr = L"UNKNOWN";
	//switch (m_ShootState)
	//{
	//case eSkillState::READY:    stateStr = L"READY";    break;
	//case eSkillState::ACTIVE:   stateStr = L"ACTIVE";   break;
	//case eSkillState::COOLDOWN: stateStr = L"COOLDOWN"; break;
	//}

	//// 2. 指定のフォーマットで表示
	//// 座標(400, posY)などは適宜調整してください
	//vnFont::print(20, 400, L"[Skill] ShootState : %s (CT: %.1f / %.1f)",
	//	stateStr,                     // ステート名
	//	m_shootCooldownTimer,         // 現在の残り時間
	//	m_shootCooldownMax            // 最大クールタイム
	//);

	//vnFont::print(10, 650, L"最大反射回数　%d", m_bullet->GetMaxBounce());

	//vnFont::print(10, 700, L"反射可能回数　%d", m_bullet->GetCurrentBounce());

	//vnFont::print(10, 650, L"MoveDir %.f, %.f , %.f",
	//	XMVectorGetX(GetMoveDir()),
	//	XMVectorGetY(GetMoveDir()),
	//	XMVectorGetZ(GetMoveDir())
	//	);
	//vnFont::print(10, 700, L"RotY %.f", GetModel()->getRotationY());
}

// --- 範囲攻撃スキル ---
void NewPlayerClass::UpdateAreaAttackSkill(float deltaTime)
{

	// スキルを持っていないなら、これ以降の処理（switch文など）を一切やらない
	if (!m_isHaveAreaAtkSkill) return;
	if (m_pullState == eSkillState::ACTIVE)return;
	switch (m_areaAttackState)
	{
	case eSkillState::READY:
	{
		bool keyE = vnKeyboard::trg(DIK_E);

		// --- 入力検知（例えば左クリックや特定のキー） ---
		if ((keyE && !m_isExpanding) /*|| (vnMouse::trgR() && !m_isExpanding)*/)
		{
			//範囲攻撃時に音を鳴らす
			m_sound->PlaySE(SE_AREA_ATTACK);

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
			DWORD debugColor = m_isExpanding ? GAME_COLOR_RED : GAME_COLOR_LIME; // 攻撃中は赤くする
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


// --- 引き寄せスキル ---
void NewPlayerClass::UpdatePullSkill(float deltaTime)
{
	// スキルを持っていないなら、これ以降の処理（switch文など）を一切やらない
	if (!m_isHavePullSkill) return;
	if (m_areaAttackState == eSkillState::ACTIVE)return;

	switch (m_pullState)
	{
	case eSkillState::READY:
		//入力判定
		if (vnKeyboard::trg(DIK_Q))
		{
			//引き寄せ攻撃時に音を鳴らす
			m_sound->PlaySE(SE_PULL_ATTACK);
			
			m_pullState = eSkillState::ACTIVE;
			m_pullTimer = m_pullDuration;

		}
		break;

	case eSkillState::ACTIVE:
		m_pullTimer -= deltaTime;
		if (m_pullTimer <= 0)
		{

			m_pullState = eSkillState::COOLDOWN;
			m_pullCooldownTimer = m_pullCooldownMax;
			//m_pullCooldownTimer = 0;
			Common::StartCameraShake(3.5f, 3.5f, 1.0f);

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
	// --- デバッグ表示 ---
	// スキル発動中(ACTIVE)は青、それ以外(準備完了中など)は薄い色にするなど
	XMVECTOR pullCenter = XMVectorAdd(*GetModel()->getPosition(), GetCollision().GetCenter());
	DWORD debugColor = (m_pullState == eSkillState::ACTIVE) ? GAME_COLOR_MAGENTA : 0x4400FFFF; // 発動中は水色、待機中は半透明

	// 引き寄せ範囲（m_pullRadius）を表示
	vnDebugDraw::Sphere(pullCenter, m_pullRadius, debugColor);
}



// --- 弾撃つ攻撃 ---
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


// --- 初期化（殻）---
void NewPlayerClass::ResetMeteorModel()
{
	if (!m_pMeteorModel||!GetModel())return;
	m_pMeteorModel->setRenderEnable(false);
	m_pMeteorModel->setPosition(GetModel()->getPosition());
	m_pMeteorModel->setPositionY(100.0f);
	m_pMeteorModel->setPositionZ(10.0f);
	m_pMeteorModel->setScale(1, 1, 1);
	m_pMeteorModel->setRotationX(4.712734f);
	m_isCanMeteor = true;
	m_isUpKara = false;	
	m_isForwardBig = false;
	m_isDownKara = false;
}


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

//隕石必殺発動
void NewPlayerClass::StartSpecialMeteor()
{
	m_isUpKara = true;
	m_isForwardBig = false;
	m_isDownKara = false;
}
//隕石必殺中
void NewPlayerClass::SpecialMeteor()
{
	//if (!pMeteorModel) return;
	//// ① 殻が真上に上がる
	//if (isUpKara)
	//{

	//	isMove = false;
	//	isCanMeteor = false;
	//	//pMeteorModel->removeParent();

	//	pUpKara->addPositionZ(1.0f); // 上

	//	if (pUpKara->getPositionZ() >= 20.0f)
	//	{
	//		pUpKara->setRenderEnable(false);
	//		pMeteorModel->setPosition(pModel->getPosition());
	//		//落ちてくる殻をplayerの動きに合わせておいとく
	//		pMeteorModel->setPositionY(70.0f);

	//		isUpKara = false;
	//		isForwardBig = true;
	//	}

	//	return;
	//}

	//// ② 正面上空へ移動しながら巨大化
	//if (isForwardBig)
	//{
	//	pMeteorModel->setRenderEnable(true);

	//	//pMeteorModel->addPositionY(-0.3f); // 正面
	//	//pMeteorModel->setRotationY(XM_PI);

	//	XMVECTOR currentPos = *pMeteorModel->getPosition();
	//	//pMeteorModel->setPosition(XMVectorGetX(currentPos), XMVectorGetY(currentPos), XMVectorGetZ(currentPos)+1.0f);
	//	
	//	//pMeteorModel->setPosition(pModel->getPositionX(), XMVectorGetY(currentPos),pModel->getPositionZ() +50.0f);
	//	float rotY = pModel->getRotationY(); // プレイヤーの向き（ラジアン）

	//	// 正面方向ベクトル
	//	float dirX = sinf(rotY);
	//	float dirZ = cosf(rotY);

	//	// 現在位置
	//	float baseX = pModel->getPositionX();
	//	float baseZ = pModel->getPositionZ();

	//	// 正面50.0f先に出す
	//	float distance = 70.0f;

	//	pMeteorModel->setPosition(
	//		baseX + dirX * distance,
	//		XMVectorGetY(currentPos),
	//		baseZ + dirZ * distance
	//	);
	//	float tergetScaleNum = 50.0f;
	//	XMFLOAT3 targetScale = { tergetScaleNum, tergetScaleNum, tergetScaleNum };
	//	if (pMeteorModel->getScaleX() < targetScale.x)
	//	{
	//		pMeteorModel->addScale(1.5f, 1.5f, 1.5f);
	//	}
	//	else
	//	{
	//		pMeteorModel->setScale(tergetScaleNum, tergetScaleNum, tergetScaleNum);
	//		isForwardBig = false;
	//		m_isDownKara = true;
	//	}
		return;
	//}

	// ③ 落下
	//if (m_isDownKara)
	//{
	//	pMeteorModel->addPositionY(-1.0f); // 下
	//	if (pMeteorModel->getPositionY() <= -50.0f) {
	//		ResetMeteorModel();
	//		ResetUpKara();
	//	}
	//}
}


//移動必殺発動(初期化)
void  NewPlayerClass::StartSpecialBoost()
{
	m_boostTime = m_boostTimeMax;
	m_isCanBoost = false;

}

//--------------------------------------------------------
//
//    経験値で変化する用のやつ
//
//--------------------------------------------------------

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

//---------------------------------------------------------




// --- レベルアップ画面での行動 ---
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