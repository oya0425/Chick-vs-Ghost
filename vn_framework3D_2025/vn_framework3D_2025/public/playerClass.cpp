#include"../framework.h"
#include"../framework/vn_environment.h"

extern vnMotionData* motion_idle_bird;


PlayerClass::PlayerClass()
{
	// --- ƒ‚ƒfƒ‹ ---
	pModel = nullptr;
	pUpKara = nullptr;

	pMeteorModel = nullptr;


	// --- ˆÚ“® ---
	baseMoveSpeed = 7.0f;
	boostSpeedMultiplier = 5.0f;

	isMove = false;
	isJump = false;
	isGroundHit = false;
	isMoving = false;


	// --- •KŽEƒQ[ƒW ---
	specialGaugeMax = 1.0f;
	specialGaugeMin = 0.0f;
	specialGauge = 0.0f;

	needGaugeBoost = 0.5f;
	needGaugeMeteor = 0.7f;

	// --- •KŽEó‘Ô ---
	isCanBoost = true;
	isCanMeteor = true;

	// ˆÚ“®•KŽE
	boostTimeMax = 10.0f;
	boostTime = 0.0f;

	// è¦Î•KŽE
	isUpKara = false;
	isForwardBig = false;
	isDownKara = false; 

	// --- HP ---
	maxHP = 100.0f;
	currentHP = maxHP;
	hpDecreasePerTime = 1.0f;
	isDead = false;


	
	// PlayerClass ‘¤
	if (isDead)
	{
		requestDestroy = true;
	}
	
	motion_idle_bird = loadMotionFile(L"data/model/Brid/brid_animation_new/motion/Brid_Idle1.mot");



}

void PlayerClass::SetModel(vnCharacter* model)
{
	pModel = model;
	// --- “–‚½‚è”»’è ---
	float sizeSetNum = 1.0f;

	pModel->setPositionY(0.0f);
	pModel->center = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	pModel->size = XMVectorSet(2.0f, 2.0f, 2.0f, 0.0f)*sizeSetNum;
	pModel->setScale(1 * sizeSetNum, 1 * sizeSetNum, 1 * sizeSetNum);

}

//•KŽE‚Ì—Ž‚¿‚Ä‚­‚éŠk‚Ìƒ‚ƒfƒ‹ƒZƒbƒg
void PlayerClass::SetMeteorModel(vnModel* model) 
{
	pMeteorModel = model;
	ResetMeteorModel();
}

//ã‚ÌŠk‚Ìƒ‚ƒfƒ‹ƒZƒbƒg
void PlayerClass::SetUpKaraModel(vnModel* model)
{
	pUpKara = model;
	ResetUpKara();
}

vnCharacter* PlayerClass::GetModel() const
{
	return pModel;
}

vnModel* PlayerClass::GetMeteorModel()const
{
	return pMeteorModel;
}
vnModel* PlayerClass::GetUpKaraModel()const
{
	return pUpKara;
}


void PlayerClass::SetPlayerMove(bool canMove) 
{
	isMove = canMove;
}




void PlayerClass::Update(float deltaTime)
{
	if (!pModel) return;
	

	// --- “ü—ÍŽæ“¾ ---
	XMVECTOR vInput = XMVectorZero();
	if (isMove)
	{
		if (vnKeyboard::on(DIK_W)) vInput = XMVectorSetZ(vInput, -1.0f);
		if (vnKeyboard::on(DIK_S)) vInput = XMVectorSetZ(vInput, 1.0f);
		if (vnKeyboard::on(DIK_A)) vInput = XMVectorSetX(vInput, 1.0f);
		if (vnKeyboard::on(DIK_D)) vInput = XMVectorSetX(vInput, -1.0f);


		if (!XMVector3Equal(vInput, XMVectorZero()))
		{

			vInput = XMVector3Normalize(vInput);
			isMoving = true;

		}
		else {
			isMoving = false;
		}

		// --- ƒJƒƒ‰Šî€‰ñ“] ---
		XMVECTOR cam_pos = *vnCamera::getPosition();
		XMVECTOR cam_trg = *vnCamera::getTarget();
		XMVECTOR dist = XMVector3Normalize(cam_pos - cam_trg);
		float camRotY = atan2f(XMVectorGetX(dist), XMVectorGetZ(dist));
		XMMATRIX rotMtx = XMMatrixRotationY(camRotY);

		vInput = XMVector3TransformNormal(vInput, rotMtx);


	}

	//// --- ƒWƒƒƒ“ƒv”­“® ---
	//if (!isJump && (vnKeyboard::trg(DIK_SPACE) || vnMouse::trg()))
	//{
	//	isJump = true;
	//	isGroundHit = false;
	//	rigidbody.addVerticalVelocity(3.0f); // ƒWƒƒƒ“ƒv—Í
	//}

	// --- Rigidbody XV ---
	rigidbody.SetBaseVelocity(vInput * baseMoveSpeed* boostSpeedMultiplier);
	rigidbody.Update(deltaTime);

	// --- ˆÚ“®”½‰f ---
	//XMVECTOR move = rigidbody.getMoveDelta();


	//pModel->addPosition(&move);

	// --- ’…’n”»’è ---
	// --- ’…’nŽž‚Ìˆ—i’n–Ê”»’è‚ÍŠO‚ÅÏ‚ñ‚Å‚¢‚é‘O’ñj ---
	if (isGroundHit)
	{
		isJump = false;
		rigidbody.SetVerticalVelocity(0.0f);
		rigidbody.SetIsUseGravity(false);
	}
	else {
		rigidbody.SetIsUseGravity(true);
	}

	// --- ‰ñ“]i“ü—Í‚ª‚ ‚é‚Æ‚«‚¾‚¯j ---
	SetMoveDirection(vInput); // moveDir‚É“ü‚ê‚é
	if (!XMVector3Equal(moveDir, XMVectorZero()))
	{
		float rotY = atan2f(
			XMVectorGetX(moveDir),
			XMVectorGetZ(moveDir)
		);
		pModel->setRotationY(rotY);
	}

	// --- •KŽE ---
	SpecialMeteor();
	if (!isCanMeteor)
	{
		isMove = false;

	}
	else {
		isMove = true;
	}


	pMeteorModel->setRenderEnable(true);


	// --- ƒ‚[ƒVƒ‡ƒ“Ý’è ---
	//‘Ò‹@ó‘Ô
	if (!isMoving&&isMove)
	{
		pModel->setMotion(motion_idle_bird);
		pModel->execute(1, false,false);

	}
	//ˆÚ“®’†
	else
	{
		pModel->setMotion(NULL);
		// --- ˆÚ“®’†‰º‚ÌŠk‚ð‰ñ“] --- 
		if (!isCanBoost) {
			pModel->getParts("KaraDown")->addRotationY(0.5f);
		}
		else {
			pModel->getParts("KaraDown")->addRotationY(0.1f);
		}

	}

}

// --- ‰Šú‰»iŠkj---
void PlayerClass::ResetMeteorModel() 
{
	if (pModel == nullptr)return;
	pMeteorModel->setRenderEnable(false);
	pMeteorModel->setPosition(pModel->getPosition());
	
	pMeteorModel->setPositionY(100.0f);
	pMeteorModel->setPositionZ(10.0f);
	pMeteorModel->setScale(1, 1, 1);
	pMeteorModel->setRotationX(4.712734f);
	isCanMeteor = true;
	isUpKara = false;	
	isForwardBig = false;
	isDownKara = false;
}


void PlayerClass::ResetUpKara()
{
	if (pModel == nullptr)return;
	pUpKara->setRenderEnable(true);
	pUpKara->setParent(pModel->getParts("Body"));
	pUpKara->setPositionY(0.1f);
	pUpKara->setPositionZ(-0.5f);
	pUpKara->setScale(1, 1, 1);
	pUpKara->setRotationY(0);
}

//è¦Î•KŽE”­“®
void PlayerClass::StartSpecialMeteor()
{
	isUpKara = true;
	isForwardBig = false;
	isDownKara = false;
}
//è¦Î•KŽE’†
void PlayerClass::SpecialMeteor()
{
	if (!pMeteorModel) return;
	// ‡@ Šk‚ª^ã‚Éã‚ª‚é
	if (isUpKara)
	{

		isMove = false;
		isCanMeteor = false;
		//pMeteorModel->removeParent();

		pUpKara->addPositionZ(1.0f); // ã

		if (pUpKara->getPositionZ() >= 20.0f)
		{
			pUpKara->setRenderEnable(false);
			pMeteorModel->setPosition(pModel->getPosition());
			//—Ž‚¿‚Ä‚­‚éŠk‚ðplayer‚Ì“®‚«‚É‡‚í‚¹‚Ä‚¨‚¢‚Æ‚­
			pMeteorModel->setPositionY(70.0f);

			isUpKara = false;
			isForwardBig = true;
		}

		return;
	}

	// ‡A ³–Êã‹ó‚ÖˆÚ“®‚µ‚È‚ª‚ç‹‘å‰»
	if (isForwardBig)
	{
		pMeteorModel->setRenderEnable(true);

		//pMeteorModel->addPositionY(-0.3f); // ³–Ê
		//pMeteorModel->setRotationY(XM_PI);

		XMVECTOR currentPos = *pMeteorModel->getPosition();
		//pMeteorModel->setPosition(XMVectorGetX(currentPos), XMVectorGetY(currentPos), XMVectorGetZ(currentPos)+1.0f);
		
		//pMeteorModel->setPosition(pModel->getPositionX(), XMVectorGetY(currentPos),pModel->getPositionZ() +50.0f);
		float rotY = pModel->getRotationY(); // ƒvƒŒƒCƒ„[‚ÌŒü‚«iƒ‰ƒWƒAƒ“j

		// ³–Ê•ûŒüƒxƒNƒgƒ‹
		float dirX = sinf(rotY);
		float dirZ = cosf(rotY);

		// Œ»ÝˆÊ’u
		float baseX = pModel->getPositionX();
		float baseZ = pModel->getPositionZ();

		// ³–Ê50.0fæ‚Éo‚·
		float distance = 70.0f;

		pMeteorModel->setPosition(
			baseX + dirX * distance,
			XMVectorGetY(currentPos),
			baseZ + dirZ * distance
		);
		float tergetScaleNum = 50.0f;
		XMFLOAT3 targetScale = { tergetScaleNum, tergetScaleNum, tergetScaleNum };
		if (pMeteorModel->getScaleX() < targetScale.x)
		{
			pMeteorModel->addScale(1.5f, 1.5f, 1.5f);
		}
		else
		{
			pMeteorModel->setScale(tergetScaleNum, tergetScaleNum, tergetScaleNum);
			isForwardBig = false;
			isDownKara = true;
		}
		return;
	}

	// ‡B —Ž‰º
	if (isDownKara)
	{
		pMeteorModel->addPositionY(-1.0f); // ‰º
		if (pMeteorModel->getPositionY() <= -50.0f) {
			ResetMeteorModel();
			ResetUpKara();
		}
	}
}


//ˆÚ“®•KŽE”­“®(‰Šú‰»)
void  PlayerClass::StartSpecialBoost()
{
	boostTime = boostTimeMax;
	isCanBoost = false;

}
//ˆÚ“®•KŽE’†
void PlayerClass::ChangeSpeed(float spped)
{
	boostSpeedMultiplier = 0.4 * spped;
}


void PlayerClass::addHP(float addhp)
{
	if (currentHP >= maxHP)return;
	// ‚Ü‚¸‚Í’Pƒ‚É‰ÁŽZ‚·‚é
	currentHP += addhp;

	// ‰ÁŽZ‚µ‚½Œ‹‰ÊAÅ‘å‚ð’´‚¦‚Ä‚¢‚½‚çÅ‘å’l‚ÅŽ~‚ß‚é
	if (currentHP > maxHP)
	{
		currentHP = maxHP;
	}
}






void PlayerClass::SetMoveDirection(const XMVECTOR& dir)
{
	moveDir = dir;
}
PlayerClass::~PlayerClass()
{
}