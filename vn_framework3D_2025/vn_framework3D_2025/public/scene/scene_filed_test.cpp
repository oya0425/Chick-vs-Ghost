#include "../../framework.h"
#include "../../framework/vn_environment.h"
#include<algorithm>
#include <DirectXMath.h>
#include <cmath>

//extern stMotion motion_idle;
//extern stMotion motion_walk;

extern vnMotionData* motion_idle;
extern vnMotionData* motion_walk;
extern vnMotionData* motion_idle_bird;
extern vnMotionData* motion_BoxUnity_RunF;

vnMotionData* motion_idle_enemy;

//初期化関数
bool SceneFieldTest::initialize()
{

	//pPlayer = new cObj(L"data/model/bear/", L"bear.vnm");
	//pPlayer->size = XMVectorSet(1.5f, 3.0f, 1.5f, 0.0f);
	//pPlayer->center = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
	//pPlayer = new cBear();
	//pPlayer = new vnCharacter(L"data/model/BoxUnityChan/", L"BoxUnityChan.bone");
	//pPlayer = new vnCharacter(L"data/model/bear_part/", L"bear_part.bone");
	//pPlayer = new vnCharacter(L"data/model/Brid/brid_new/", L"brid.bone");
	pPlayer = new vnCharacter(L"data/model/Brid/brid_animation_new/", L"brid.bone");

	//pPlayer->size = XMVectorSet(1.5f, 3.0f, 1.5f, 0.0f);
	//pPlayer->center = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);

	pPlayerTest = new PlayerClass();
	pPlayerTest->SetModel(new vnCharacter(L"data/model/Brid/brid_animation_new/", L"brid.bone"));
	pPlayerTest->SetMeteorModel(new vnModel(L"data/model/Brid/brid_animation_new/", L"KaraDown.vnm"));
	pPlayerTest->SetUpKaraModel(new vnModel(L"data/model/Brid/", L"KaraUp.vnm"));

	pBoxUnity = new vnCharacter(L"data/model/BoxUnityChan/", L"BoxUnityChan.bone");
	pBoxUnity->setScale(3, 3, 3);


	pEnemyTest = new EnemyClass();
	//pEnemyTest->SetModel(new vnCharacter(L"data/model/BoxUnityChan/", L"BoxUnityChan.bone"));
	pEnemyTest->SetModel(new vnCharacter(L"data/model/Ghost/", L"Ghost.bone"));
	//pEnemyTest->SetModel(new vnCharacter(L"data/model/Ghost_animation/", L"Ghost_animation.bone"));


	//pNPC=new vnModel(L"data/model/bear/", L"bear.vnm");
	pNPC = new vnCharacter(L"data/model/bear_part/", L"bear_part.bone");

	pDebugFan=new vnModel(L"data/model/", L"debug_fan.vnm");

	//地面設定
	//pGround = new vnModel(L"data/model/overpass/", L"overpass.vnm");
	pGround = new vnModel(L"data/model/", L"ground.vnm");
	pGround->setScale(10.0f, 1, 10);

	//pGround = createObj(0.0f, 0.125f, 0.0f, 120.0f, 0.25f, 120.0f);
	pGround->setAmbient(0.2f, 0.2f, 0.2f, 0);
	pGround->setDiffuse(0.4f, 0.4f, 0.4f, 0);


	//適当な位置に初期配置
	pNPC->setPosition(5.0f, 0.0f, 5.0f);

	//敵の視覚範囲の設定　位置　色
	//pDebugFan->setPosition(pNPC->getPosition());
	pDebugFan->setParent(pNPC);
	pDebugFan->setAmbient(0, 10, 30, 0);
	pDebugFan->setDiffuse(0, 1, 1, 0);
	pDebugFan->getSpecular(1);
	
	pNPC->getPosition();
	//フェンスの配置
	for (int i = 0; i < FENCE_NUM; i++) {
		pFence[i] = new vnModel(L"data/model/", L"fence.vnm");

		//角度
		float degree = 360.0f/FENCE_NUM * (float)i;
		//Degree->Radian
		float radian = degree * 3.141592f / 180.0f;

		//極座標->直交座標
		float x = sin(radian) * FENCE_RADIUS;
		float z = cos(radian) * FENCE_RADIUS;
		float angle = (2.0f * 3.14159f / FENCE_NUM) * i;

		float degrees = -angle +3.14159f;
		pFence[i]->setPosition(x, 0.0, z);
		//pFence[i]->setRotationY(degrees + 3.141592 / 2);


		pFence[i]->setRotationY(radian);

		registerObject(pFence[i]);
	}
	

	//2D表示
	///ビルボード（ビックリマーク）
	pBillboard = new vnBillboard(1.0f, 1.0f, L"data/image/icon_exc.png");
	//pBillboard->setPosition(pNPC->getPosition());
	pBillboard->setParent(pNPC);
	pBillboard->setPositionY(3.5f);

	///アイコン用
	pIcon = new vnBillboard(1.0f, 1.0f, L"data/image/icon_exc.png");

	pSky=new vnModel(L"data/model/", L"skydome.vnm");

	pShadow = new vnModel(L"data/model/", L"shadow.vnm");
	pShadow->setTransparent(true);	//半透明

	pSky->setLighting(false);
	pSky->setScale(5.0f, 5.0f, 5.0f);



	// --- 後期追加 ---
	pCrown = new vnModel(L"data/model/", L"crown.vnm");
	//pCrown->setParent(pPlayer->getParts(3));
	pCrown->setParent(pPlayer->getParts("head"));
	//pCrown->setRenderEnable(false);

	//pBrid = new vnModel(L"data/model/Brid/", L"brid.vnm");
	////pBrid->setParent(pPlayer->getParts("head"));
	//pBrid->setPositionY(0.0f);


	//位置大きさ調整
	pCrown->setPositionY(1.3f);
	pCrown->setScale(0.4f, 0.4f, 0.4f);

	pKaraUp = new vnModel(L"data/model/Brid/", L"KaraUp.vnm");
	pKaraUp->setParent(pPlayer->getParts("Body"));
	pKaraUp->setPositionY(0.1f);
	//pKaraUp->setPositionZ(-1.0f);
	pKaraUp->setPositionZ(-0.5f);

	//エミッターの作成
	vnEmitter::stEmitterDesc desc;
	//desc.LifeMin = 30.0f;
	//desc.LifeMax = 60.0f;
	//desc.ColorMin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//desc.ColorMax = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	desc.SizeMin = 0.0f;
	desc.SizeMax = 5.0f;
	//desc.SpeedMin = 0.1f;
	//desc.SpeedMax = 0.2f;
	swprintf_s(desc.Texture, L"data/image/cfxr spikes impact.png");
	desc.ColorMax = V_GAME_COLOR_BLUE;
	pEmitter = new vnEmitter(&desc);

	swprintf_s(desc.Texture, L"data/image/enn.png");
	desc.ColorMax = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	pEmitterNPC = new vnEmitter(&desc);

	swprintf_s(desc.Texture, L"data/image/enn.png");
	desc.ColorMax = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	pEmitterBrid = new vnEmitter(&desc);
	//pEmitterBrid->setZWrite(false);

	registerObject(pEmitter);
	registerObject(pEmitterNPC);
	registerObject(pEmitterBrid);
	pEmitter->setParent(pPlayer);
	pEmitterNPC->setParent(pNPC);
	//pEmitterBrid->setParent(pBrid);
	pEmitterBrid->setZWrite(true);

	// ----------------
	pEmitter->setRenderEnable(false);
	pEmitterBrid->setRenderEnable(false);

	//vnObject::setRenderFlag()


	motion_idle = loadMotionFile(L"data/model/bear_part/motion/motion_idle.mot");
	motion_walk = loadMotionFile(L"data/model/bear_part/motion/motion_walk.mot");
	motion_idle_bird = loadMotionFile(L"data/model/Brid/brid_animation_new/motion/Brid_Idle1.mot");

	motion_idle_enemy = loadMotionFile(L"data/model/Ghost/motion/ghost_idle.mot");


	//motion_BoxUnity_RunF = loadMotionFile(L"data/model/BoxUnityChan/motion/RUN00_F.mot");
	//motion_BoxUnity_RunF = loadMotionFile(L"data/model/BoxUnityChan/motion/DAMAGED01.mot");

	//オブジェクトの登録（シーン基底クラス）
	registerObject(pPlayer);
	for (int i = 0; i < pPlayer->getPartsNum(); i++) {
		registerObject(pPlayer->getParts(i));
	}

	registerObject(pPlayerTest->GetModel());
	for (int i = 0; i < pPlayerTest->GetModel()->getPartsNum(); i++) {
		registerObject(pPlayerTest->GetModel()->getParts(i));
	}

	registerObject(pPlayerTest->GetMeteorModel());
	registerObject(pPlayerTest->GetUpKaraModel());

	//registerObject(pBoxUnity);
	//for (int i = 0; i < pBoxUnity->getPartsNum(); i++) {
	//	registerObject(pBoxUnity->getParts(i));
	//}

	registerObject(pEnemyTest->GetModel());
	for (int i = 0; i < pEnemyTest->GetModel()->getPartsNum(); i++) {
		registerObject(pEnemyTest->GetModel()->getParts(i));
	}


	//registerObject(pGround);
	registerObject(pNPC);
	for (int i = 0; i < pNPC->getPartsNum(); i++) {
		registerObject(pNPC->getParts(i));
	}

	registerObject(pCrown);
	registerObject(pKaraUp);

	//registerObject(pBrid);

	//半透明のオブジェクトは不透明オブジェクトの後に描画
	registerObject(pSky);
	registerObject(pGround);
	registerObject(pShadow);
	registerObject(pDebugFan);
	registerObject(pBillboard);
	registerObject(pIcon);


	//プレイヤー関連初期設定
	speed = 0.5f;

	air = true;
	velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	gravity = XMVectorSet(0.0f, -0.015f, 0.0f, 0.0f);
	jumpForce = XMVectorSet(0.0f, 0.25f, 0.0f, 0.0f);

	startPos = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);
	pPlayer->setPosition(&startPos);


	radius = 10.0f;
	theta = 0.0f;
	phi = 0.0f;

	radiusCam = 20.0f;
	thetaCam = 0.0f;
	phiCam = 0.0f;


	input = false;
	reverse = true;
	onePos = false;
	//isJump = false;

	pPlayer->setMotion(motion_idle);

	//emitters.push_back(pEmitter);
	//emitters.push_back(pEmitterNPC);
	//emitters.push_back(pEmitterBrid);
	pEnemyTest->Spawn(XMVectorSet(10, 2, 10, 0));


	pPlayerTest->SetPlayerMove(true);

	isDead = false;

	return true;
}

//終了関数
void SceneFieldTest::terminate()
{
	for (int i = 0; i < pPlayer->getPartsNum(); i++) {
		deleteObject(pPlayer->getParts(i));
	}
	for (int i = 0; i < pNPC->getPartsNum(); i++) {
		deleteObject(pNPC->getParts(i));
	}

	for (int i = 0; i < pPlayerTest->GetModel()->getPartsNum(); i++) {
		deleteObject(pPlayerTest->GetModel()->getParts(i));
	}
	deleteObject(pPlayerTest->GetModel());

	for (int i = 0; i < pBoxUnity->getPartsNum(); i++) {
		deleteObject(pBoxUnity->getParts(i));
	}

	for (int i = 0; i < pEnemyTest->GetModel()->getPartsNum(); i++) {
		deleteObject(pEnemyTest->GetModel()->getParts(i));
	}
	deleteObject(pEnemyTest->GetModel());


	//シーン登録からの削除
	deleteObject(pPlayer);
	deleteObject(pShadow);
	deleteObject(pBoxUnity);
	deleteObject(pNPC);
	deleteObject(pDebugFan);
	deleteObject(pFence[FENCE_NUM-1]);
	deleteObject(pGround);
	deleteObject(pSky);
	deleteObject(pBillboard);
	deleteObject(pIcon);
	deleteObject(pCrown);
	deleteObject(pEmitter);
	deleteObject(pEmitterNPC);
	deleteObject(pEmitterBrid);
	deleteObject(pKaraUp);

	deleteObject(pPlayerTest->GetMeteorModel());
	deleteObject(pPlayerTest->GetUpKaraModel());
	delete pPlayerTest;
	delete pEnemyTest;
}

//処理関数
void SceneFieldTest::execute()
{

	if (vnKeyboard::trg(DIK_RETURN))
	{
		pPlayerTest->StartSpecialMeteor();
	}
	if (vnKeyboard::trg(DIK_R)) {
		pPlayerTest->StartSpecialBoost();
	}


	if (vnKeyboard::trg(DIK_K))
	{
		isDead = !isDead;
	}

	if (isDead) {
		pBoxUnity->setMotion(motion_BoxUnity_RunF);
		pBoxUnity->execute(1,isDead,true);
		//pNPC->execute(1);
	}
	else {

	}
	


	float dt = vnScene::getDeltaTime();
	{
		pPlayerTest->Update(dt * 5);

		XMVECTOR move = pPlayerTest->GetRigidbody().getMoveDelta();

		pPlayerTest->GetModel()->addPosition(&move);

		OnCollider(pPlayerTest->GetModel(), pGround, 1.0f, pPlayerTest->GetRigidbody());

		//LinePos = *pPlayerTest->GetModel()->getPosition();
		////LineDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

		//// --- モデルデータから内部情報を取得 ---
		//int vnum = pGround->getVertexNum();	//頂点数を獲得
		//int inum = pGround->getIndexNum();	//インデックス数

		////メッシュ単位で走査するため、メッシュデータを取得
		//int meshNum = pGround->getMeshNum();
		//vnModel_MeshData* pMesh = pGround->getMesh();

		//vnVertex3D* vtx = pGround->getVertex();	//頂点配列
		//unsigned short* idx = pGround->getIndex();	//インデックス配列
		////ワールドマトリクス
		//XMMATRIX world = *pGround->getWorld();

		//float highestY = -10000.0f; // 初期値は極端に低く
		//float horizontalX = -10000.0f;
		//int hitMeshID = -1;
		////地面
		//vnCollide::stSegment seg;
		//float footOffset = 1.0f;   // 足の高さ
		//float safetyMargin = 0.2f;

		//seg.Pos = *pPlayerTest->GetModel()->getPosition() + XMVectorSet(0, footOffset, 0, 0);
		//seg.Dir = XMVectorSet(0, -1, 0, 0);
		//seg.Length = footOffset + safetyMargin;
		////seg.Pos = *pPlayerTest->GetModel()->getPosition() + XMVectorSet(0.0f, 20.5f, 0.0f, 0.0f);
		////seg.Dir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
		////seg.Length = 0.6f;
		//vnCollide::stSegment segCeil;
		//float headOffset = 1.8f; // 頭の高さ

		//segCeil.Pos = *pPlayerTest->GetModel()->getPosition()
		//	+ XMVectorSet(0, headOffset, 0, 0);
		//segCeil.Dir = XMVectorSet(0, 1, 0, 0);
		//segCeil.Length = safetyMargin;


		//vnCollide::stSegment segWall;
		//float bodyHeight = 2.0f;   // 体の中心
		//float wallDist = 1.1f;   // 半径イメージ

		//XMVECTOR moveDir = pPlayerTest->GetRigidbody().getMoveDelta();
		//moveDir = XMVectorSetY(moveDir, 0.0f);


		//if (!XMVector3Equal(moveDir, XMVectorZero()))
		//{
		//	moveDir = XMVector3Normalize(moveDir);
		//}

		//segWall.Pos = *pPlayerTest->GetModel()->getPosition()
		//	+ XMVectorSet(0, bodyHeight, 0, 0);
		//segWall.Dir = moveDir;
		//segWall.Length = wallDist;
		//pPlayerTest->SetIsWall(false);



		//for (int m = 0; m < meshNum; m++)
		//{
		//	int m_inum = pMesh[m].IndexNum;
		//	int m_sidx = pMesh[m].StartIndex;

		//	//for(int i=sidx;i<sidx+inum;i+=3)
		//	for (int i = 0; i < m_inum; i += 3)
		//	{
		//		XMVECTOR v1 = XMVector3TransformCoord(
		//			XMVectorSet(vtx[idx[m_sidx + i + 0]].x,
		//				vtx[idx[m_sidx + i + 0]].y,
		//				vtx[idx[m_sidx + i + 0]].z, 0.0f),
		//			world);

		//		XMVECTOR v2 = XMVector3TransformCoord(
		//			XMVectorSet(vtx[idx[m_sidx + i + 1]].x,
		//				vtx[idx[m_sidx + i + 1]].y,
		//				vtx[idx[m_sidx + i + 1]].z, 0.0f),
		//			world);

		//		XMVECTOR v3 = XMVector3TransformCoord(
		//			XMVectorSet(vtx[idx[m_sidx + i + 2]].x,
		//				vtx[idx[m_sidx + i + 2]].y,
		//				vtx[idx[m_sidx + i + 2]].z, 0.0f),
		//			world);

		//		// ★ ここで vnCollide 用の三角形を作る
		//		vnCollide::stTriangle tri;
		//		tri.fromPoints(&v1, &v2, &v3);

		//		// ★ ここで Segment と当てる
		//		XMVECTOR hit;
		//		if (vnCollide::isCollide(&hit, &seg, &tri))
		//		{
		//			float y = XMVectorGetY(hit);
		//			if (y > highestY)
		//			{
		//				highestY = y;
		//			}
		//		}
		//		if (vnCollide::isCollide(&hit, &segCeil, &tri))
		//		{
		//			float hitY = XMVectorGetY(hit);
		//			// 上昇中なら止める
		//			if (pPlayerTest->GetRigidbody().GetVerticalVelocity() > 0.0f)
		//			{
		//				pPlayerTest->GetRigidbody().setVerticalVelocity(0.0f);
		//			}
		//		}
		//		if (vnCollide::isCollide(&hit, &segWall, &tri))
		//		{
		//			float x = XMVectorGetX(hit);
		//			if (x > horizontalX)
		//			{
		//				horizontalX = x;
		//			}
		//			float hitDist =
		//				XMVectorGetX(
		//					XMVector3Length(hit - segWall.Pos)
		//				);
		//			float pushBack = wallDist - hitDist;
		//			if (pushBack > 0.0f)
		//			{
		//				XMVECTOR back = -moveDir * pushBack;
		//				pPlayerTest->GetModel()->addPosition(&back);
		//			}
		//		}

		//		vnDebugDraw::Line(&v1, &v2, 0xffff0000);
		//		vnDebugDraw::Line(&v2, &v3, 0xffff0000);
		//		vnDebugDraw::Line(&v3, &v1, 0xffff0000);

		//	}
		//}

		//float playerY = pPlayerTest->GetModel()->getPositionY();

		//if (highestY > -10000.0f)
		//{
		//	pPlayerTest->SetIsGround(true);
		//	pPlayerTest->GetModel()->setPositionY(highestY + GROUND_OFFSET);
		//	pPlayerTest->GetRigidbody().setVerticalVelocity(0.0f);
		//}
		//else {
		//	pPlayerTest->SetIsGround(false);
		//}


		////pPlayerTest->GetModel()->addPosition(&move);


		//XMVECTOR LineEnd = segWall.Pos + segWall.Dir * 10.0f; // ← まずは変数に代入
		//vnDebugDraw::Line(&LinePos, &LineEnd, GAME_COLOR_PURPLE); // ← これでOK！


		if (pPlayerTest->GetModel()->getPositionY() < -30.0f) {
			//プレイヤーが一定以上落下したら
			pPlayerTest->GetModel()->setPosition(0.0f, 1.0f, 0.0f);
		}


		//XMVECTOR LineEnd = LinePos + LineDir * 10.0f; // ← まずは変数に代入
		//vnDebugDraw::Line(&LinePos, &LineEnd, GAME_COLOR_PURPLE); // ← これでOK！

	}


	pEnemyTest->SetPlayerPos(pPlayerTest->GetModel()->getPosition());
	//pEnemyTest->Spawn(XMVectorSet(10, 2, 10, 0));

	pEnemyTest->Update(dt*3);

	XMVECTOR move = pEnemyTest->GetRigidbody().getMoveDelta();

	pEnemyTest->GetModel()->addPosition(&move);


	OnCollider(pEnemyTest->GetModel(), pGround, 1.0f, pEnemyTest->GetRigidbody());

	vnFont::print(10.0f, 440.0f, L"dt : %3f", dt);
	vnFont::print(10.0f, 460.0f, L"重力：%3f", pPlayerTest->GetRigidbody().GetGracity());
	vnFont::print(10.0f, 480.0f, L"GetVerticalVelocity : % 3f", pPlayerTest->GetRigidbody().GetVerticalVelocity());
	vnFont::print(10.0f, 500.0f, pPlayerTest->GetIsJump() ? L"isJump : ture" : L"isJump:false");
	vnFont::print(10.0f, 540.0f, pPlayerTest->GetIsGround() ? L"IsGround : ture" : L"IsGround:false");

	vnFont::print(10.0f, 580.0f, L"殻Y : % 3f", pPlayerTest->GetMeteorModel()->getPositionY());
	vnFont::print(10.0f, 10.0f, L"pPlayerTest->GetMeteorModel : % 3f,% 3f,% 3f",
	pPlayerTest->GetMeteorModel()->getPositionX(),
	pPlayerTest->GetMeteorModel()->getPositionY(),
	pPlayerTest->GetMeteorModel()->getPositionZ());
	vnFont::print(10.0f, 50.0f, isDead ? L"isDead : ture" : L"isDead:false");

	vnFont::print(10.0f, 30.0f, L"pPlayerTest->getScaleX : % 3f,% 3f,% 3f",
		pPlayerTest->GetMeteorModel()->getScaleX(),
		pPlayerTest->GetMeteorModel()->getScaleY(),
		pPlayerTest->GetMeteorModel()->getScaleZ());

	vnFont::print(10.0f, 70.0f, L"pEnemyTest->GetModel : % 3f,% 3f,% 3f",
		pEnemyTest->GetModel()->getPositionX(),
		pEnemyTest->GetModel()->getPositionY(),
		pEnemyTest->GetModel()->getPositionZ());
	vnFont::print(10.0f, 120, L"getPartsNum box : % 3d", pEnemyTest->GetModel()->getPartsNum());

	if (pEnemyTest->GetState() != EnemyClass::eState::KnockBack)
	{
		if (colliderCtoC(pEnemyTest->GetModel(), pPlayerTest->GetModel()) != None)
		{
			pEnemyTest->SetIsHitPlayer(true);
		}
		else {
			pEnemyTest->SetIsHitPlayer(false);
		}


	}

	//InFence(pEnemyTest->GetModel());
	//InFence(pPlayerTest->GetModel());

	//XMVECTOR ropecenter = XMVectorAdd(*pPlayerTest->GetModel()->getPosition(), pPlayerTest->GetModel()->center);
	//vnDebugDraw::Box(ropecenter, pPlayerTest->GetModel()->size, GAME_COLOR_LIME);
	//XMVECTOR ropecenter = XMVectorAdd(*pGround->getPosition(), pGround->center);
	//vnDebugDraw::Box(ropecenter, pGround->size, GAME_COLOR_LIME);

	//デバッグ表示
	//vnFont::print(10.0f, 520.0f, L"PlayerTestPos : % 3f,% 3f,% 3f",
	//	pPlayerTest->GetModel()->getPositionX(),
	//	pPlayerTest->GetModel()->getPositionY(),
	//	pPlayerTest->GetModel()->getPositionZ());

	pCrown->setRenderEnable(false);
	//pPlayerTest->GetModel()->setRenderEnable(false);
	for (int i = 0; i < pPlayerTest->GetModel()->getPartsNum(); i++) {
		//pPlayerTest->GetModel()->getParts(i)->setRenderEnable(false);
	}

	for (int i = 0; i < pPlayer->getPartsNum(); i++) {
		pPlayer->getParts(i)->setRenderEnable(false);
	}

	input = false;

	XMVECTOR vMove = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


	//pEmitterBrid->setEmit(true);
	pEmitter->getZWrite();

	static float f = 0.0f;
	if (vnKeyboard::trg(DIK_F)) {
		f += 0.01f;
	}
	static bool KaraOn = false;

	if (vnKeyboard::on(DIK_O)) {
		//if (pKaraUp->getPositionZ() > 0) {
		//}
		//else if (pKaraUp->getPositionZ() < -0.1) {
			//pKaraUp->addPositionZ(0.01f);
		//}
		//pKaraUp->addScale(0.02f, 0.02f, 0.02f);
		pKaraUp->addPositionZ(1.1f);
	}
	//Common::UpdateCameraByMouse(theta, phi, radius, pPlayer->getPosition());
	if (vnKeyboard::trg(DIK_Q)) {
		bool flag = pEmitter->getRenderFlag(vnObject::eRenderFlag::Addition);
		flag = !flag;
		pEmitter->setRenderFlag(vnObject::eRenderFlag::Addition, flag);

	}

	if (input) {
		float moveX = XMVectorGetX(vMove);
		float moveZ = XMVectorGetZ(vMove);
		float rotY = atan2f(moveX,moveZ);

		//現在の角度（source）
		//目的の角度（destination）
		XMMATRIX mSrc, mDst, mAns;
		XMVECTOR qSrc, qDst, qAns;

		//オイラー角をマトリクスに変換
		mSrc = XMMatrixRotationY(pPlayer->getRotationY());
		mDst = XMMatrixRotationY(rotY);



		//マトリクスをクオータニオンに変換
		qSrc = XMQuaternionRotationMatrix(mSrc);
		qDst = XMQuaternionRotationMatrix(mDst);

		//mSrc = XMMatrixRotationRollPitchYaw(0.0f, pPlayer->getRotationY(), 0.0f);
		//mDst = XMMatrixRotationRollPitchYaw(0.0f, rotY, 0.0f);


		//球面線形補間で中間の姿勢を計算
		qAns = XMQuaternionSlerp(qSrc, qDst, 0.1f);

		//クオータニオンからマトリクスに変換
		mAns = XMMatrixRotationQuaternion(qAns);

		//マトリクスからY軸回転値を計算
		float ry = atan2f(-XMVectorGetZ(mAns.r[0]), XMVectorGetZ(mAns.r[2]));

		pPlayer->setRotationY(ry);
		



		//pEmitter->setEmit(true);
		//pPlayer->setMotion(motion_walk);
		pPlayer->setMotion(NULL);

		pPlayer->getParts("KaraDown")->addRotationY(0.1f);
	}
	else {
		//pEmitter->setEmit(false);
		pPlayer->setMotion(motion_idle_bird);


	}
	if (vnKeyboard::trg(DIK_R)/*&&vnKeyboard::trg(DIK_T)*/) {
		pPlayer->addPosition(5.0f, 3.0f, 0.0f);
	}

	if (air) {
		float speed = 0.2f; // 横の移動量
		if (vnKeyboard::on(DIK_R)) { // 右
			pPlayer->addPosition(speed, 0.0f, 0.0f);
		}
		if (vnKeyboard::on(DIK_L)) { // 左
			pPlayer->addPosition(-speed, 0.0f, 0.0f);
		}
	}
	InFence(pPlayer);
	InFence(pNPC);


	////ステージオブジェクトとの衝突
	//for (int i = 0; i < AABB_MAX; i++)
	//{
	//	if (pGround == NULL)continue;

	//	eDirection dir = colliderAtoB_new(pPlayer, pGround);

	//	if (dir == eDirection::Y_Pos)	//上に乗った
	//	{
	//		velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//		air = false;
	//	}
	//	else if (dir == eDirection::Y_Neg)	//下からぶつかった
	//	{
	//		velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//	}
	//}


	////ステージオブジェクトとの衝突
	//for (int i = 0; i < AABB_MAX; i++)
	//{
	//	if (pGround == NULL)continue;

	//	eDirection dir = colliderAtoB_new(pPlayerTest->GetModel(), pGround);

	//	if (dir == eDirection::Y_Pos)	//上に乗った
	//	{
	//		velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//		air = false;
	//	}
	//	else if (dir == eDirection::Y_Neg)	//下からぶつかった
	//	{
	//		velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//	}
	//}
	//pPlayerTest->GetModel()->addPosition(&velocity);


	//ジャンプの発動
	if (air == false && vnKeyboard::trg(DIK_SPACE)|| air == false && vnMouse::trg())
	{
		air = true;
		velocity = jumpForce;
	}

	//物理挙動
	velocity += gravity;
	pPlayer->addPosition(&velocity);


	//NPCからプレイヤーへ向かうベクトルを作る
	XMVECTOR vNpcToPlayer = *pPlayer->getPosition() - *pNPC->getPosition();
	//NPCからプレイヤーへ向かうベクトルの長さを求める
	XMVECTOR vDist = XMVector3Length(vNpcToPlayer);	//NPCとプレイヤーの距離
	float dist = XMVectorGetX(vDist);

	XMMATRIX mNpc = *pNPC->getWorld();	//ワールドマトリクスを取得
	XMVECTOR vFront = mNpc.r[2];		//前方向を表すベクトル

	vNpcToPlayer = XMVector3Normalize(vNpcToPlayer);
	vFront = XMVector3Normalize(vFront);
	//内積値を求める
	XMVECTOR vDot = XMVector3Dot(vNpcToPlayer, vFront);
	float dot = XMVectorGetX(vDot);

	//float型変数のノイズ除去
	if (dot > 1.0f) {
		dot = 1.0f;
	}
	else if(dot<-1.0f) {
		dot = -1.0f;
	}
	
	//θを求める
	float angle = acosf(dot);

	//NPCの認識範囲にプレイヤーが入っているか調べる
	
	//①NPCとプレイヤーの距離が10m以内
	//②NPCの前方向からプレイヤーの角度が+-45°
	if (dist < 10.0f && angle < 45.0f * 3.141592f / 180.0f) {

		vnFont::print(250.0f, 190.0f, L"認識に入っている");
		pBillboard->setRenderEnable(true);
		//pEmitterNPC->setEmit(true);
		pNPC->setMotion(motion_walk);
			//pBillboard->setPosition(pNPC->getPositionX(), pNPC->getPositionY() + 3.5f, pNPC->getPositionZ());

		//プレイヤーを追いかける
		vNpcToPlayer = XMVector3Normalize(vNpcToPlayer);
		vNpcToPlayer = XMVectorScale(vNpcToPlayer, 0.05f);
		pNPC->addPosition(&vNpcToPlayer);
		//プレイヤーの方を向く

		float x = XMVectorGetX(vNpcToPlayer);
		float z = XMVectorGetZ(vNpcToPlayer);
		float npcY = atan2f(x, z);
		pNPC->setRotationY(npcY);
		//pDebugFan->setPosition(pNPC->getPosition());
		//pDebugFan->setRotationY(npcY);
	}
	else {
		//pEmitterNPC->setEmit(false);
		pBillboard->setRenderEnable(false);
		pNPC->setMotion(motion_idle);
	}


	// --- NPCとの当たり判定 ---
	XMVECTOR vNpcPlayerAtari = *pPlayer->getPosition() - *pNPC->getPosition();
	XMVECTOR avDist = XMVector3Length(vNpcPlayerAtari);	//NPCとプレイヤーの距離
	float adist = XMVectorGetX(avDist);
	XMVECTOR pushDir = XMVector3Normalize(vNpcPlayerAtari); // 単位ベクトル
	XMVECTOR pushAmount = pushDir * (1.8f - adist); // 足りない距離分押し戻す
	

	if (adist < 1.8f) {
		isatari = true;
		XMVECTOR NnewPos = *pNPC->getPosition() - pushAmount;
		pNPC->setPosition(&NnewPos);
		//pDebugFan->setPosition(&NnewPos);
	}	







	//影の座標にプレイヤーの座標を代入
	pShadow->setPosition(pPlayer->getPosition());

	//Z-Fightingを回避するため、影のポリゴンを少し浮かす
	pShadow->setPositionY(0.5f);


	const float delta = 3.141592f / 180.0f;
	const float maxPhi = 3.141592f / 2.0f;
	const float minPhi = -3.141592f / 2.0f;      // -90度	
	//固定カメラ
	{
		//{
		//	// プレイヤーの位置を取得
		//	XMVECTOR playerPos = *pPlayer->getPosition();

		//	//カメラの位置をプレイヤーの後ろ + 高さ
		//	XMVECTOR camPos = XMVectorSet(
		//		XMVectorGetX(playerPos) - 10.0f,  // X方向に少し引く
		//		XMVectorGetY(playerPos) + 15.0f,  // 高さを持たせる
		//		XMVectorGetZ(playerPos) - 10.0f,  // Z方向にも引く
		//		0.0f
		//	);

		//	// 注視点はプレイヤー
		//	//XMVECTOR camTrg = playerPos;

		//	// カメラの位置を固定（X,Zにオフセットして高さを持たせる）
		//	XMVECTOR camPos = XMVectorSet(10.0f, 20.0f, -20.0f, 0.0f);

		//	// 注視点を原点に固定（またはプレイヤーの初期位置）
		//	XMVECTOR camTrg = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//	// カメラに適用
		//	vnCamera::setPosition(&camPos);
		//	vnCamera::setTarget(&camTrg);

		//}
	}

	//プレイヤーの目線に合わせたカメラ
	{
		//カメラ移動
		{
			if (vnKeyboard::rel(DIK_Q)) {
				reverse = !reverse;
			}

			if (vnKeyboard::on(DIK_UP)) {

				if (!reverse) {
					phi -= delta;
					if (phi < -maxPhi) {
						phi = -maxPhi;
					}
				}
				else {
					phi += delta;
					if (phi > maxPhi) {
						phi = maxPhi;
					}
				}
			}
			if (vnKeyboard::on(DIK_DOWN)) {
				if (!reverse) {
					phi += 1.0f * 3.141592f / 180.0f;

				}
				else {
					phi -= 1.0f * 3.141592f / 180.0f;

				}
			}

			if (vnKeyboard::on(DIK_LEFT)) {
				if (reverse) {

					theta -= 1.0f * 3.141592f / 180.0f;

				}
				else {

					theta += 1.0f * 3.141592f / 180.0f;
				}
			}

			if (vnKeyboard::on(DIK_RIGHT)) {
				if (reverse) {

					theta += 1.0f * 3.141592f / 180.0f;

				}
				else {

					theta -= 1.0f * 3.141592f / 180.0f;
				}

			}
		}

		XMVECTOR camPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR camTrg = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR camBias = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);


		//カメラの座標の計算
		//回転していない基準となるベクトル
		XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);

		//回転マトリクスを作成
		XMMATRIX camRotate = XMMatrixRotationRollPitchYaw(phi, theta, 0.0f);

		//基準となるベクトルを回転させる
		eye = XMVector3TransformNormal(eye, camRotate);

		//回転の中心をプレイヤーのする
		//XMVECTOR v = XMVectorSet(px, py, pz, 0.0f);
		//camPos = XMVectorAdd(eye, *pPlayer->getPosition());
		camPos = XMVectorAdd(eye, *pPlayerTest->GetModel()->getPosition());
		camPos = XMVectorAdd(camPos, camBias);

		//カメラの位置を極座標(ｒ、θ、φ)で表した座標にする
		vnCamera::setPosition(&camPos);

		//カメラの注視点をプレイヤーに合わせる
		//camTrg = XMVectorAdd(*pPlayer->getPosition(), camBias);
		camTrg = XMVectorAdd(*pPlayerTest->GetModel()->getPosition(), camBias);
		if (!onePos) {
			vnCamera::setTarget(&camTrg);
		}
		else {
			vnCamera::setPosition(&camTrg);
			vnCamera::setTarget(&camPos);

		}

	}
	pIcon->setPosition(vnCamera::getPositionX()+2.5, vnCamera::getPositionY(), vnCamera::getPositionZ()+3.5f);

	Common::UpdateCameraByMouse(theta, phi, radius, pPlayerTest->GetModel()->getPosition());

	//デバッグ表示
	//vnFont::print(10.0f, 10.0f,L"PlayerPos : % 3f,% 3f,% 3f",
	//	pPlayer->getPositionX(),
	//	pPlayer->getPositionY(),
	//	pPlayer->getPositionZ());
	//vnFont::print(10.0f, 30.0f, L"radius : % 3f", radius);
	//vnFont::print(10.0f, 50.0f, L"theta : % 3f", theta*180.0f/3.141592f);
	//vnFont::print(10.0f, 70.0f, L"phi : % 3f", phi * 180.0f / 3.141592f);
	//vnFont::print(10.0f, 90.0f, reverse ? L"Qキー:カメラ反転 : OFF" : L"Qキー:カメラ反転 : ON");
	//vnFont::print(10.0f, 110.0f, onePos ? L"1キー:反転 : ON" : L"1キー:反転 : OFF");
	//vnFont::print(10.0f, 130.0f, L"Player RotY : %.3f", pPlayer->getRotationY() * 180.0f / 3.141592f);
	//vnFont::print(10.0f, 150.0f, L"NPCとプレイヤーの距離 : %.3f", dist);
	//vnFont::print(10.0f, 170.0f, L"NPCとプレイヤーの角度 : %.3f", angle * 180.0f / 3.141592f);

	//vnFont::print(10.0f, 200.0f, L"CrownPos : % 3f,% 3f,% 3f",
	//	pCrown->getPositionX(),
	//	pCrown->getPositionY(),
	//	pCrown->getPositionZ());
	//XMVECTOR wld_crown;
	//pCrown->getWorldPosition(&wld_crown);
	//vnFont::print(10.0f, 230.0f, L"CrownWorldPos : % 3f,% 3f,% 3f",
	//	XMVectorGetX(wld_crown),
	//	XMVectorGetY(wld_crown),
	//	XMVectorGetZ(wld_crown));
	//pEmitter->setZWrite(false);

	//vnFont::print(10.0f, 310.0f, pEmitter->getZWrite() ? L"true" : L"false");
	//if (pEmitter->getZWrite()) {
	//	vnFont::print(10.0f, 320.0f, L"true");
	//}
	//else {
	//	vnFont::print(10.0f, 320.0f, L"false");
	//}
	//if (!pEmitter)
	//	vnFont::print(10, 350, L"NULL");

	//XMVECTOR md = pPlayerTest->GetRigidbody().getMoveDelta();

	//vnFont::print(
	//	10.0f, 400.0f,
	//	L"playerMoveDelta : %.3f, %.3f, %.3f",
	//	XMVectorGetX(md),
	//	XMVectorGetY(md),
	//	XMVectorGetZ(md)
	//);

	//XMVECTOR wld_crown;
	//pCrown->getWorldPosition(&wld_crown);
	//vnFont::print(10.0f, 230.0f, L"CrownWorldPos : % 3f,% 3f,% 3f",
	//	XMVectorGetX(wld_crown),
	//	XMVectorGetY(wld_crown),
	//	XMVectorGetZ(wld_crown));



	// --- ライト設定 ---

	// 環境光：全体の底上げ（入れすぎない）
	vnLight::setAmbient(0.35f, 0.35f, 0.35f);

	// キャラ個別の環境光：ほぼ補助レベル
	//pBrid->setAmbient(0.1f, 0.1f, 0.1f, 1.0f);

	// 平行光源：少し斜め前上から当てる
	vnLight::setLightDir(0.3f, -1.0f, -0.4f);

	// 光の色：完全な白より少しだけ抑える
	vnLight::setLightCol(0.95f, 0.95f, 0.95f);

	vnDebugDraw::Grid();
	vnDebugDraw::Axis();
	vnScene::execute();
}

//描画関数
void SceneFieldTest::render()
{

	pEmitterBrid->setLighting(true);
	vnScene::render();
}

//フェンスの外に出ないようにする関数
void SceneFieldTest::InFence(vnCharacter* pObject)
{
	//if (pObject == nullptr) return;

	XMVECTOR vPos = *pObject->getPosition();  // 現在位置
	float length = XMVectorGetX(XMVector3Length(vPos));  // 原点からの距離

	if (length > FENCE_RADIUS - 1.0f)
	{
		XMVECTOR dir = XMVector3Normalize(vPos);
		XMVECTOR newPos = XMVectorScale(dir, FENCE_RADIUS - 1.0f);
		pObject->setPosition(&newPos);
	}
}


bool SceneFieldTest::movePlayer(vnObject* p,bool canMove)
{
	if (!canMove) return false; // 移動禁止なら即リターン

	float lx = vnJoystick::isValid() ? vnJoystick::leftX() : (vnKeyboard::on(DIK_A) == true ? -1.0f : (vnKeyboard::on(DIK_D) == true ? 1.0f : 0.0f));
	float ly = vnJoystick::isValid() ? vnJoystick::leftY() : (vnKeyboard::on(DIK_W) == true ? -1.0f : (vnKeyboard::on(DIK_S) == true ? 1.0f : 0.0f));
	if (lx == 0.0f && ly == 0.0f)return false;

	XMVECTOR cam_pos = *vnCamera::getPosition();
	XMVECTOR cam_trg = *vnCamera::getTarget();
	XMVECTOR dist = cam_pos - cam_trg;

	dist = XMVector3Normalize(dist);

	float rot = atan2f(XMVectorGetX(dist), XMVectorGetZ(dist));

	XMMATRIX mtx;
	mtx = XMMatrixRotationY(rot);

	XMVECTOR move = XMVectorSet(lx, 0.0f, -ly, 0.0f);
	XMVECTOR move_r;
	move_r = XMVector3TransformNormal(move, mtx);

	rot = atan2f(XMVectorGetX(move_r), XMVectorGetZ(move_r)) + 3.141592f;

#if 1	//クオータニオンの球面線形補間を使い回転を滑らかに行う(使わない場合は関数を呼ばない)
	//rot = slerpRotY(p->getRotationY(), rot, 0.1f);
#endif

	p->setRotationY(rot);

	move_r = XMVectorMultiply(move_r, XMVectorSet(-speed, -speed, -speed, 0.0f));
	p->addPosition(&move_r);

	return true;
}

//オブジェクトを回転させる(オブジェ、半径、Yの中心、Xの中心、回転する速さ(0.02が普通))
void SceneFieldTest::MoveCircle(cObj* obj, float radius, float centerY, float centerX, float speed) {
	static float angle = 0.0f;   // 角度（ラジアン）

	float x = centerX + radius * cos(angle);
	float y = centerY + radius * sin(angle);

	obj->setPosition(0, y, x);

	angle += speed;

	//1πで一周
	if (angle >= 2 * 3.141592f) {
		angle -= 2 * 3.141592f; // 1周したらリセット
	}


}

//コンストラクタ
cObj::cObj(const WCHAR* folder, const WCHAR* file) : vnModel(folder, file)
{
	size = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	center = XMVectorZero();
}

cObj* SceneFieldTest::createObj(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ)
								
{
	cObj* ret = new cObj(L"data/model/primitive/", L"cube.vnm");
	//ret->setScale(sizeX * 0.5f, sizeY * 0.5f, sizeZ * 0.5f);
	ret->setScale(sizeX , sizeY , sizeZ );

	ret->setPosition(posX, posY, posZ);
	ret->size = XMVectorSet(sizeX, sizeY, sizeZ, 0.0f);
	ret->center = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	registerObject(ret);
	return ret;
}

SceneFieldTest::eDirection SceneFieldTest::colliderAtoB(cObj* p1, cObj* p2)
{
	eDirection ret = eDirection::None;

	if (p1 == NULL || p2 == NULL)return ret;

	XMVECTOR range = XMVectorAdd(p1->size * 0.5f, p2->size * 0.5f);
	float rx = XMVectorGetX(range);
	float ry = XMVectorGetY(range);
	float rz = XMVectorGetZ(range);

	//AABB中心のワールド座標
	XMVECTOR center1 = XMVectorAdd(*p1->getPosition(), p1->center);
	XMVECTOR center2 = XMVectorAdd(*p2->getPosition(), p2->center);
	//中心座標の差分(絶対値)
	XMVECTOR dif = XMVectorSubtract(center1, center2);
	dif = XMVectorAbs(dif);

	float dx = XMVectorGetX(dif);
	float dy = XMVectorGetY(dif);
	float dz = XMVectorGetZ(dif);

	if (dx < rx && dy < ry && dz < rz)	//衝突判定
	{
		//重なっている部分の長さ(最少の要素の方向に押し戻す)
		float sx = rx - dx;
		float sy = ry - dy;
		float sz = rz - dz;
		if (sx < sy)
		{
			if (sx < sz)
			{	//xが最少
				if (XMVectorGetX(center1) < XMVectorGetX(center2))
				{
					p1->addPositionX(-sx);

					ret = eDirection::X_Neg;
				}
				else
				{
					p1->addPositionX(sx);

					ret = eDirection::X_Pos;
				}
			}
			else
			{	//zが最少
				if (XMVectorGetZ(center1) < XMVectorGetZ(center2))
				{
					p1->addPositionZ(-sz);

					ret = eDirection::Z_Neg;
				}
				else
				{
					p1->addPositionZ(sz);

					ret = eDirection::Z_Pos;
				}
			}
		}
		else
		{
			if (sy < sz)
			{	//yが最少
				if (XMVectorGetY(center1) < XMVectorGetY(center2))
				{
					p1->addPositionY(-sy);

					ret = eDirection::Y_Neg;
				}
				else
				{
					p1->addPositionY(sy);

					ret = eDirection::Y_Pos;
				}
			}
			else
			{	//zが最少
				if (XMVectorGetZ(center1) < XMVectorGetZ(center2))
				{
					p1->addPositionZ(-sz);

					ret = eDirection::Z_Neg;
				}
				else
				{
					p1->addPositionZ(sz);

					ret = eDirection::Z_Pos;
				}
			}
		}
	}
	return ret;
}


void SceneFieldTest::OnCollider(vnCharacter* pCharacter,vnModel *pGround, float footOffset,RigidbodyComponent &rigidBody)
{
	XMVECTOR LinePos = *pCharacter->getPosition();
	//LineDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

	// --- モデルデータから内部情報を取得 ---
	int vnum = pGround->getVertexNum();	//頂点数を獲得
	int inum = pGround->getIndexNum();	//インデックス数

	//メッシュ単位で走査するため、メッシュデータを取得
	int meshNum = pGround->getMeshNum();
	vnModel_MeshData* pMesh = pGround->getMesh();

	vnVertex3D* vtx = pGround->getVertex();	//頂点配列
	unsigned short* idx = pGround->getIndex();	//インデックス配列
	//ワールドマトリクス
	XMMATRIX world = *pGround->getWorld();

	float highestY = -10000.0f; // 初期値は極端に低く
	int hitMeshID = -1;

	//地面
	vnCollide::stSegment seg;
	float safetyMargin = 0.2f;

	seg.Pos = *pCharacter->getPosition() + XMVectorSet(0, footOffset, 0, 0);
	seg.Dir = XMVectorSet(0, -1, 0, 0);
	seg.Length = footOffset + safetyMargin;
	//seg.Pos = *pPlayerTest->GetModel()->getPosition() + XMVectorSet(0.0f, 20.5f, 0.0f, 0.0f);
	//seg.Dir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//seg.Length = 0.6f;

	for (int m = 0; m < meshNum; m++)
	{
		int m_inum = pMesh[m].IndexNum;
		int m_sidx = pMesh[m].StartIndex;

		//for(int i=sidx;i<sidx+inum;i+=3)
		for (int i = 0; i < m_inum; i += 3)
		{
			XMVECTOR v1 = XMVector3TransformCoord(
				XMVectorSet(vtx[idx[m_sidx + i + 0]].x,
					vtx[idx[m_sidx + i + 0]].y,
					vtx[idx[m_sidx + i + 0]].z, 0.0f),
				world);

			XMVECTOR v2 = XMVector3TransformCoord(
				XMVectorSet(vtx[idx[m_sidx + i + 1]].x,
					vtx[idx[m_sidx + i + 1]].y,
					vtx[idx[m_sidx + i + 1]].z, 0.0f),
				world);

			XMVECTOR v3 = XMVector3TransformCoord(
				XMVectorSet(vtx[idx[m_sidx + i + 2]].x,
					vtx[idx[m_sidx + i + 2]].y,
					vtx[idx[m_sidx + i + 2]].z, 0.0f),
				world);

			// ★ ここで vnCollide 用の三角形を作る
			vnCollide::stTriangle tri;
			tri.fromPoints(&v1, &v2, &v3);

			// ★ ここで Segment と当てる
			XMVECTOR hit;
			if (vnCollide::isCollide(&hit, &seg, &tri))
			{
				float y = XMVectorGetY(hit);
				if (y > highestY)
				{
					highestY = y;
				}
			}
		}
	}

	if (highestY > -10000.0f)
	{
		//pEnemy->SetIsGround(true);
		rigidBody.SetVerticalVelocity(0.0f);
		rigidBody.SetIsGround(true);
		rigidBody.SetIsUseGravity(false);

		pCharacter->setPositionY(highestY + GROUND_OFFSET);
	}
	else {
		//pEnemy->SetIsGround(false);
		rigidBody.SetIsUseGravity(true);
		rigidBody.SetIsGround(false);

	}


}

SceneFieldTest::eDirection SceneFieldTest::colliderCtoC(vnCharacter* p1, vnCharacter* p2)
{
	eDirection ret = eDirection::None;

	if (!p1 || !p2) return ret;

	XMVECTOR range = XMVectorAdd(p1->size * 0.5f, p2->size * 0.5f);
	float rx = XMVectorGetX(range);
	float ry = XMVectorGetY(range);
	float rz = XMVectorGetZ(range);

	XMVECTOR center1 = XMVectorAdd(*p1->getPosition(), p1->center);
	XMVECTOR center2 = XMVectorAdd(*p2->getPosition(), p2->center);

	XMVECTOR dif = XMVectorAbs(center1 - center2);

	float dx = XMVectorGetX(dif);
	float dy = XMVectorGetY(dif);
	float dz = XMVectorGetZ(dif);

	if (dx < rx && dy < ry && dz < rz)
	{
		float sx = rx - dx;
		float sy = ry - dy;
		float sz = rz - dz;

		if (sx < sy && sx < sz)
		{
			if (XMVectorGetX(center1) < XMVectorGetX(center2))
			{
				p1->addPositionX(-sx);
				ret = X_Neg;
			}
			else
			{
				p1->addPositionX(sx);
				ret = X_Pos;
			}
		}
		else if (sy < sz)
		{
			if (XMVectorGetY(center1) < XMVectorGetY(center2))
			{
				p1->addPositionY(-sy);
				ret = Y_Neg;
			}
			else
			{
				p1->addPositionY(sy);
				ret = Y_Pos;
			}
		}
		else
		{
			if (XMVectorGetZ(center1) < XMVectorGetZ(center2))
			{
				p1->addPositionZ(-sz);
				ret = Z_Neg;
			}
			else
			{
				p1->addPositionZ(sz);
				ret = Z_Pos;
			}
		}
	}

	return ret;
}
