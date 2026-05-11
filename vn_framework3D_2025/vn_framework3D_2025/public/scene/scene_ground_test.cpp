#include "../../framework.h"
#include "../../framework/vn_environment.h"
#include<algorithm>
#include <DirectXMath.h>
#include <cmath>


//初期化関数
bool SceneGroundTest::initialize()
{
	pPlayer = new vnModel(L"data/model/bear/", L"bear.vnm");
	//pPlayer->size = XMVectorSet(1.5f, 3.0f, 1.5f, 0.0f);
	//pPlayer->center = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);


	//pNPC=new vnModel(L"data/model/bear/", L"bear.vnm");
	//pDebugFan=new vnModel(L"data/model/", L"debug_fan.vnm");

	//地面設定
	//pGround = new vnModel(L"data/model/", L"simple_bg_wall.vnm");
	//pGround = new vnModel(L"data/model/overpass/", L"overpass.vnm");
	pGround = new vnModel(L"data/model/", L"colored_ground.vnm");
	
	registerObject(pGround);
	//pGround = createObj(0.0f, 0.125f, 0.0f, 120.0f, 0.25f, 120.0f);
	//pGround->setAmbient(0.2f, 0.2f, 0.2f, 0);
	//pGround->setDiffuse(0.4f, 0.4f, 0.4f, 0);

	pGround->setScale(2.0f, 1.0f, 2.0f);
	//pGround->setRotationX(45.0f * XM_PI/180.0f);

	

	pSky=new vnModel(L"data/model/", L"skydome.vnm");

	pShadow = new vnModel(L"data/model/", L"shadow.vnm");
	pShadow->setTransparent(true);	//半透明

	pSky->setLighting(false);




	// --- 後期追加 ---
	//pCrown = new vnModel(L"data/model/", L"crown.vnm");
	//pCrown->setParent(pPlayer);
	////位置大きさ調整
	//pCrown->setPositionY(2.7f);
	//pCrown->setScale(0.4f, 0.4f, 0.4f);

	// ----------------







	//オブジェクトの登録（シーン基底クラス）
	registerObject(pPlayer);
	//registerObject(pGround);
	//registerObject(pCrown);


	//半透明のオブジェクトは不透明オブジェクトの後に描画
	registerObject(pSky);
	registerObject(pShadow);


	//プレイヤー関連初期設定
	speed = 0.2f;

	air = true;
	velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	gravity = XMVectorSet(0.0f, -0.015f, 0.0f, 0.0f);

	jumpForce = XMVectorSet(0.0f, 0.25f, 0.0f, 0.0f);


	startPos = XMVectorSet(0.0f, 0.2f, 0.0f, 0.0f);
	pPlayer->setPosition(&startPos);


	radius = 10.0f;
	theta = 0.0f;
	phi = 0.0f;


	reverse = true;
	onePos = false;
	//isJump = false;
	return true;
}

//終了関数
void SceneGroundTest::terminate()
{
	//シーン登録からの削除
	deleteObject(pPlayer);
	deleteObject(pShadow);
	deleteObject(pGround);
	deleteObject(pSky);
	deleteObject(pCrown);
}

//処理関数
void SceneGroundTest::execute()
{
	static float f = 0.0f;
	if (vnKeyboard::trg(DIK_F)) {
		f += 0.01f;
	}


	//プレイヤーの移動
	bool update = movePlayer(pPlayer, true);

	//if (vnKeyboard::trg(DIK_R)/*&&vnKeyboard::trg(DIK_T)*/) {
	//	pPlayer->addPosition(5.0f, 3.0f, 0.0f);
	//}

	if (air) {
		float speed = 0.2f; // 横の移動量
		if (vnKeyboard::on(DIK_R)) { // 右
			pPlayer->addPosition(speed, 0.0f, 0.0f);
		}
		if (vnKeyboard::on(DIK_L)) { // 左
			pPlayer->addPosition(-speed, 0.0f, 0.0f);
		}
	}

	//ステージオブジェクトとの衝突



	//ジャンプの発動
	if (air == false && vnKeyboard::trg(DIK_SPACE) || air == false && vnMouse::trg())
	{
		air = true;
		velocity = jumpForce;
	}

	//物理挙動
	velocity += gravity;
	pPlayer->addPosition(&velocity);

	if (pPlayer->getPositionY() < -30.0f) {
		//プレイヤーが一定以上落下したら
		pPlayer->setPosition(0.0f, 0.0f, 0.0f);
		velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
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
		camPos = XMVectorAdd(eye, *pPlayer->getPosition());
		camPos = XMVectorAdd(camPos, camBias);

		//カメラの位置を極座標(ｒ、θ、φ)で表した座標にする
		vnCamera::setPosition(&camPos);

		//カメラの注視点をプレイヤーに合わせる
		camTrg = XMVectorAdd(*pPlayer->getPosition(), camBias);
		if (!onePos) {
			vnCamera::setTarget(&camTrg);
		}
		else {
			vnCamera::setPosition(&camTrg);
			vnCamera::setTarget(&camPos);

		}

	}









	Common::UpdateCameraByMouse(theta, phi, radius, pPlayer->getPosition());
















	//直線の情報
//#if 0
	LinePos = *pPlayer->getPosition();
	LineDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//
	//#else
	//	vnCollide::stLine line;
	//	line.Pos = *pPlayer->getPosition();
	//	line.Dir = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	//
	//
	//#endif
	//	LinePos = *pPlayer->getPosition();
	//	LineDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//
	//
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

	//	
	//	for (int i = 0; i < inum; i += 3)		//三角形の数だけループ
	//	{
	//		////頂点番号
	//		//idx[0]	(5)
	//		//idx[1]	(10)
	//		//idx[2]	(20)
	//		////頂点の座標
	//		//vtx[5].xyz
	//		//vtx[10].xyz
	//		//vtx[20].xyz
	//
	//		////最終的にこうなる
	//		//vtx[idx[0]].xyz
	//		//vtx[idx[1]].xyz
	//		//vtx[idx[2]].xyz
	//
	//
	//		XMVECTOR v1 = XMVectorSet(
	//			vtx[idx[i+0]].x, vtx[idx[i+0]].y, vtx[idx[i+0]].z, 0.0f);
	//		XMVECTOR v2 = XMVectorSet( 					  
	//			vtx[idx[i+1]].x, vtx[idx[i+1]].y, vtx[idx[i+1]].z, 0.0f);
	//		XMVECTOR v3 = XMVectorSet( 					  
	//			vtx[idx[i+2]].x, vtx[idx[i+2]].y, vtx[idx[i+2]].z, 0.0f);
	//		
	//		//ワールド座標に変換
	//		v1 = XMVector3TransformCoord(v1, world);
	//		v2 = XMVector3TransformCoord(v2, world);
	//		v3 = XMVector3TransformCoord(v3, world);
	////#if 1
	//		//vnCollide::stTriangle tri;
	//		//tri.fromPoints(&v1, &v2, &v3);
	//		//XMVECTOR hit;
	//
	//		//if (vnCollide::isCollide(&hit, &line, &tri)) {
	//		//	pPlayer->setPositionY(XMVectorGetY(hit));
	//		//	velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//
	//		//	vnDebugDraw::Line(&v1, &v2, 0xffff00ff);
	//		//	vnDebugDraw::Line(&v2, &v3, 0xffff00ff);
	//		//	vnDebugDraw::Line(&v3, &v1, 0xffff00ff);
	//		//}
	//
	////#else
	//
	//		// --- 平面の情報を作る ---
	//		XMVECTOR v, w;
	//		v = v2 - v1;
	//		w = v3 - v1;
	//		PlaneNormal = XMVector3Cross(v, w);
	//		PlaneNormal = XMVector3Normalize(PlaneNormal);
	//
	//		//平面と原点の距離（法線と平面上の１点の内積値を計算）
	//		XMVECTOR vDot = XMVector3Dot(PlaneNormal, v1);
	//		PlaneDistance = -XMVectorGetX(vDot);
	//
	//		//直線と平面の衝突座標を求める
	//		float d, s;
	//		XMVECTOR vS = XMVector3Dot(PlaneNormal, LinePos);
	//		s = XMVectorGetX(vS);
	//		s += PlaneDistance;
	//		XMVECTOR vD = XMVector3Dot(PlaneNormal, LinePos + LineDir);
	//		d = XMVectorGetX(vD);
	//		d += PlaneDistance;
	//
	//		//直線と平面が平行の場合、これ以上計算しない
	//		if (s == d)continue;
	//
	//		//衝突座標を表す媒介変数
	//		float t = s / (s - d);
	//
	//		if (t > 0)continue;	//下にのみRayをだす
	//
	//
	//		//衝突座標
	//		XMVECTOR hit = LinePos + LineDir * t;
	//		//vnFont::print(20, 300, L"t = %f", t);
	//
	//
	//		//衝突座標が三角形の内側にあるか調べる
	//		XMVECTOR e0, e1, e2;	//三角形のエッジ（方向ベクトル）
	//		XMVECTOR h0, h1, h2;	//各頂点から衝突点へのベクトル（方向ベクトル）
	//		XMVECTOR c0, c1, c2;	//外積ベクトル
	//		float d0 = 0, d1 = 0, d2 = 0;		//内積値
	//		bool including = true;	//内包判定
	//
	//		e0 = v2 - v1;
	//		h0 = hit - v2;
	//		c0 = XMVector3Cross(e0, h0);
	//		d0 = XMVectorGetX(XMVector3Dot(c0, PlaneNormal));
	//		if (d0 < 0.0f) including = false;
	//
	//		e1 = v3 - v2;
	//		h1 = hit - v3; // ← ここもv1ではなくv3です
	//		c1 = XMVector3Cross(e1, h1);
	//		d1 = XMVectorGetX(XMVector3Dot(c1, PlaneNormal));
	//		if (d1 < 0.0f) including = false;
	//
	//		e2 = v1 - v3;
	//		h2 = hit - v3;
	//		c2 = XMVector3Cross(e2, h2);
	//		d2 = XMVectorGetX(XMVector3Dot(c2, PlaneNormal));
	//		if (d2 < 0.0f) including = false;
	//
	//		if (including) {
	//			//プレイヤーの高さ(Y座標)を衝突点に合わせる
	//			pPlayer->setPositionY(XMVectorGetY(hit));
	//			velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//
	//			vnDebugDraw::Line(&v1, &v2, 0xffff00ff);
	//			vnDebugDraw::Line(&v2, &v3, 0xffff00ff);
	//			vnDebugDraw::Line(&v3, &v1, 0xffff00ff);
	//
	//
	//		}
	//		else {
	//			vnDebugDraw::Line(&v1, &v2, 0xffff0000);
	//			vnDebugDraw::Line(&v2, &v3, 0xffff0000);
	//			vnDebugDraw::Line(&v3, &v1, 0xffff0000);
	//
	//
	//		}
	//
	//
	//
	////#endif 1
	//
	//
	//	}
	//

	float highestY = -10000.0f; // 初期値は極端に低く

	int hitMeshID = -1;

	for (int m = 0; m < meshNum; m++)
	{
		int m_inum = pMesh[m].IndexNum;
		int m_sidx = pMesh[m].StartIndex;

		//for(int i=sidx;i<sidx+inum;i+=3)
		for (int i = 0; i < m_inum; i += 3)
		{
			// 三角形頂点を取得
			XMVECTOR v1 = XMVector3TransformCoord(XMVectorSet(vtx[idx[m_sidx + i + 0]].x, vtx[idx[m_sidx + i + 0]].y, vtx[idx[m_sidx + i + 0]].z, 0.0f), world);
			XMVECTOR v2 = XMVector3TransformCoord(XMVectorSet(vtx[idx[m_sidx + i + 1]].x, vtx[idx[m_sidx + i + 1]].y, vtx[idx[m_sidx + i + 1]].z, 0.0f), world);
			XMVECTOR v3 = XMVector3TransformCoord(XMVectorSet(vtx[idx[m_sidx + i + 2]].x, vtx[idx[m_sidx + i + 2]].y, vtx[idx[m_sidx + i + 2]].z, 0.0f), world);

			// 平面法線と距離
			XMVECTOR PlaneNormal = XMVector3Normalize(XMVector3Cross(v2 - v1, v3 - v1));
			float PlaneDistance = -XMVectorGetX(XMVector3Dot(PlaneNormal, v1));

			// Ray-平面交点計算
			float s = XMVectorGetX(XMVector3Dot(PlaneNormal, LinePos)) + PlaneDistance;
			float d = XMVectorGetX(XMVector3Dot(PlaneNormal, LinePos + LineDir)) + PlaneDistance;

			if (s == d) continue; // 平行
			float t = s / (s - d);
			if (t > 0) continue; // 下方向のみ

			XMVECTOR hit = LinePos + LineDir * t;

			// 三角形内包判定
			bool including = true;
			XMVECTOR e0 = v2 - v1, h0 = hit - v2, c0 = XMVector3Cross(e0, h0);
			if (XMVectorGetX(XMVector3Dot(c0, PlaneNormal)) < 0.0f) including = false;

			XMVECTOR e1 = v3 - v2, h1 = hit - v3, c1 = XMVector3Cross(e1, h1);
			if (XMVectorGetX(XMVector3Dot(c1, PlaneNormal)) < 0.0f) including = false;

			XMVECTOR e2 = v1 - v3, h2 = hit - v3, c2 = XMVector3Cross(e2, h2);
			if (XMVectorGetX(XMVector3Dot(c2, PlaneNormal)) < 0.0f) including = false;

			if (including)
			{
				float y = XMVectorGetY(hit);
				if (y > highestY) highestY = y;
				hitMeshID = m;
				// デバッグ用ライン
				vnDebugDraw::Line(&v1, &v2, 0xffff00ff);
				vnDebugDraw::Line(&v2, &v3, 0xffff00ff);
				vnDebugDraw::Line(&v3, &v1, 0xffff00ff);
			}
			else
			{
				vnDebugDraw::Line(&v1, &v2, 0xffff0000);
				vnDebugDraw::Line(&v2, &v3, 0xffff0000);
				vnDebugDraw::Line(&v3, &v1, 0xffff0000);
			}
		}
	}


	// 最高の衝突点が見つかればプレイヤーに反映
	if (highestY > -10000.0f)
	{
		pPlayer->setPositionY(highestY);
		velocity = XMVectorZero();
	}

	XMVECTOR LineEnd = LinePos + LineDir * 10.0f; // ← まずは変数に代入
	vnDebugDraw::Line(&LinePos, &LineEnd, GAME_COLOR_PURPLE); // ← これでOK！










	//デバッグ表示
	vnFont::print(10.0f, 10.0f, L"PlayerPos : % 3f,% 3f,% 3f",
		pPlayer->getPositionX(),
		pPlayer->getPositionY(),
		pPlayer->getPositionZ());
	vnFont::print(10.0f, 100.0f, L"hitMeshID : %d", hitMeshID);


	//int materialID = pMesh[hitMeshID].MaterialID;
	//vnFont::print(10.0f, 120.0f, L"MaterialColor : (%.3f, %.3f, %.3f)", 
	//	pGround->getDiffuseR(materialID),
	//	pGround->getDiffuseG(materialID),
	//	pGround->getDiffuseB(materialID));
	//pPlayer->setDiffuse(pGround->getDiffuse(materialID));
	

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


		



	vnDebugDraw::Grid();
	vnDebugDraw::Axis();
	vnScene::execute();
}

//描画関数
void SceneGroundTest::render()
{
	vnScene::render();
}



bool SceneGroundTest::movePlayer(vnObject* p,bool canMove)
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



////コンストラクタ
////cObj::cObj(const WCHAR* folder, const WCHAR* file) : vnModel(folder, file)
////{
////	size = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
////	center = XMVectorZero();
////}
////vnModel* SceneGroundTest::createObj(float posX, float posY, float posZ, float sizeX, float sizeY, float sizeZ)
////								
////{
////	cObj* ret = new cObj(L"data/model/primitive/", L"cube.vnm");
////	//ret->setScale(sizeX * 0.5f, sizeY * 0.5f, sizeZ * 0.5f);
////	ret->setScale(sizeX , sizeY , sizeZ );
////
////	ret->setPosition(posX, posY, posZ);
////	ret->size = XMVectorSet(sizeX, sizeY, sizeZ, 0.0f);
////	ret->center = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
////	registerObject(ret);
////	return ret;
////}
//
//
//
