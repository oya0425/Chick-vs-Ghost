#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool ScenePickTest::initialize()
{
	Point = new XMVECTOR[5]; // 5個分確保

	Point[0] = XMVectorSet(-2.0f, 4.0f, -2.0f, 0.0f);
	Point[1] = XMVectorSet(2.0f, -4.0f, 3.0f, 0.0f);
	Point[2] = XMVectorSet(-1.0f, 0.0f, 1.0f, 0.0f);
	Point[3] = XMVectorSet(-1.0f, 0.0f, -1.0f, 0.0f);
	Point[4] = XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);

	PointS = new XMVECTOR[4]; 
	PointS[0] = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
	PointS[1] = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
	PointS[2] = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
	PointS[3] = XMVectorSet(0.0f,0.0f,0.0f,0.0f);


	for (int i = 0; i < 2; i++) {
		pHit[i] = new vnModel(L"data/model/primitive/", L"Sphere.vnm");
		registerObject(pHit[i]);
		pHit[i]->setScale(0.1f, 0.1f, 0.1f);

	}

	pSphere = new vnModel(L"data/model/primitive/", L"Sphere.vnm");
	registerObject(pSphere);

	pPoint = new vnModel(L"data/model/primitive/", L"Sphere.vnm");
	registerObject(pPoint);
	pPoint->setScale(0.1f, 0.1f, 0.1f);

	int index = 0;
	float spacing = 2.0f;  // 各モデルの間隔
	float offset = (5 - 1) / 2.0f;  // 中心補正（= 2.0）

	for (int z = 0; z < 5; z++) {
		for (int x = 0; x < 5; x++) {
			// モデル生成
			pModel[index] = new vnModel(L"data/model/primitive/", L"Sphere.vnm");

			// 配置位置を中心合わせで計算
			float posX = (x - offset) * spacing;  // 左から右
			float posZ = (z - offset) * spacing;  // 奥から手前

			pModel[index]->setPosition(posX, 0.0f, posZ);

			// 登録
			registerObject(pModel[index]);

			index++;
		}
	}


	radiusRe = 15.0f;
	radius = 15.0f;
	theta = -0.75;
	phi = 0.5f;

	Cursor = 0;

	return true;
}

//終了関数
void ScenePickTest::terminate()
{
	//オブジェクトの削除
	delete[] Point;
	Point = nullptr;
	deleteObject(pSphere);
	for (int i = 0; i < 2; i++) {
		deleteObject(pHit[i]);
	}
	for (int i = 0; i < 25; i++) {
		deleteObject(pModel[i]);  // 中身を先に解放

	}

}

void ScenePickTest::execute() {






	// --- 直線の情報を作る (Point[0],[1] -> LinePos,LineDir)
	LinePos = PointS[0];
	LineDir = PointS[1] - PointS[0];
	LineDir = XMVector3Normalize(LineDir);

	//球の情報を作る
	SphereCenter = PointS[2];
	SphereRadius = XMVectorGetX(PointS[3]);

	//衝突の有無だけ知りたい
	{
		//球中心からの最近傍距離を示す直線状の一点を求める
		XMVECTOR v = SphereCenter - LinePos;
		float dot = XMVectorGetX(XMVector3Dot(v, LineDir));
		XMVECTOR closestPoint = LinePos + LineDir * dot;

		pPoint->setPosition(&closestPoint);	//デバッグモデル

		//最近傍距離
		XMVECTOR vDist = SphereCenter - closestPoint;
		float dist = XMVectorGetX(XMVector3Length(vDist));

		vnFont::print(10.0f, 300.0f, L"直線と球中心の距離:%f", dist);
		if (dist <= SphereRadius) {
			vnFont::print(10.0f, 320.0f, L"当たっている");
		}
		else {
			vnFont::print(10.0f, 320.0f, L"当たっていない");

		}
	}

	//直線と球の衝突座標を求める
	{
		int n = 0;		//衝突点の数
		XMVECTOR hit[2];//衝突点の座標


		XMVECTOR v = LinePos - SphereCenter;
		XMVECTOR nv = XMVector3Normalize(v);
		XMVECTOR vdt = XMVector3Dot(nv, LineDir);
		float dt = XMVectorGetX(vdt);
		XMVECTOR vdl = XMVector3Length(v);
		float dl = XMVectorGetX(vdl);
		float dr = dl * sqrtf(1.0f - dt * dt);	//球中心と直線の距離
		if (dr <= SphereRadius) {
			//球の中心と直線の距離が球の半径以内なら衝突
			float dq = sqrtf(SphereRadius * SphereRadius - dr * dr);
			float ds = -dl * dt;
			float dp = ds + dq;
			float dm = ds - dq;

			XMVECTOR hit1 = LinePos + LineDir * dm;
			XMVECTOR hit2 = LinePos + LineDir * dp;

			hit[n++] = LinePos + LineDir * dm;

			if (dq >= 0.0f) {
				//諸突点が２個ある
				hit[n++] = LinePos + LineDir * dp;
			}
			else {
				hit[n++] = LinePos + LineDir * dp;
			}

			//デバッグ
			pHit[0]->setPosition(&hit1);
			pHit[1]->setPosition(&hit2);

		}
		else {

		}
		for (int i = 0; i < 2; ++i) {
			if (i < n) {
				//衝突している
				pHit[i]->setPosition(&hit[i]);
				pHit[i]->setDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
				pHit[i]->setRenderEnable(true);
			}
			else {
				//衝突していない
				pHit[i]->setRenderEnable(false);
			}
		}
	}

	//if (including) {
	//	//三角形の内側に入っている
	//	pPoint->setDiffuse(1.0f, 0.0f, 0.0f, 1.0f);		//赤にする

	//}
	//else {
	//	pPoint->setDiffuse(0.0f, 1.0f, 1.0f, 1.0f);		//白に戻す

	//}


	//マウス座標
	float mx = (float)vnMouse::getX();
	float my = (float)vnMouse::getY();
	vnFont::print(10.0f, 10.0f, L"マウス座標：（%.3f, %.3f）", mx, my);

	//マウス座標（スクリーン座標）->ビューポート座標

	//X : 0(左) ~ 1280(右)        -> -1 ~ +1
	//Y : 0 ~ 720				  -> +1 ~ -1

	float viewport_x = mx / (float)vnMainFrame::screenWidth * 2.0f - 1.0f;
	float viewport_y = -my / (float)vnMainFrame::screenHeight * 2.0f + 1.0f;

	vnFont::print(10.0f, 30.0f, L"ビューポート座標 :（%.3f, %.3f）", viewport_x, viewport_y);

	//マウス座標（ビューポート座標）FarPlaneに置く(z=1,w=1)※同次座標系
	
	XMVECTOR v = XMVectorSet(viewport_x, viewport_y, 1.0f, 1.0f);

	//ビューポート座標->ワールド座標
	XMMATRIX VP = *vnCamera::getScreen();	//View * Proj
	//逆行列を計算
	XMMATRIX iVP = XMMatrixInverse(NULL, VP);

	//XMVECTOR *pDeterminant : 行列式(スカラー)
	// 0 : なら逆行列の計算に失敗した
	// 返還前から変換後の体積比率
	// 特にない場合、NULLでよい
	//XMMATRIX  M : 変換対象のマトリクス

	XMVECTOR world = XMVector4Transform(v, iVP);
	//w除算（w成分を1にする　＝　輪あーるど空間のゆがみを無い状態にする）
	XMVECTOR w = XMVectorSplatW(world);	//xyzw -> wwww
	world = XMVectorDivide(world, w);	//x=x/w ,y=y/w ,z=z/w ,w=w/w
	

	vnFont::print(10.0f, 50.0f, L"ワールド座標 : (%.3f,%.3f,%.3f,%.3f)",
		XMVectorGetX(world),
		XMVectorGetY(world),
		XMVectorGetZ(world),
		XMVectorGetW(world)

	);
	//直線の情報
	XMVECTOR LinePos = *vnCamera::getPosition();
	XMVECTOR LineDir = world - LinePos;
	LineDir = XMVector3Normalize(LineDir);

	//
	for (int i = 0; i < 25; i++) {
		XMVECTOR SphereCenter = *pModel[i]->getPosition();
		float SphereRadius = 1.0f;

		//衝突の有無だけ知りたい
		{
			//球中心からの最近傍距離を示す直線状の一点を求める
			XMVECTOR v = SphereCenter - LinePos;
			float dot = XMVectorGetX(XMVector3Dot(v, LineDir));

			XMVECTOR closestPoint = LinePos + LineDir * dot;

			//最近傍距離
			XMVECTOR vDist = SphereCenter - closestPoint;
			float dist = XMVectorGetX(XMVector3Length(vDist));
			vnFont::print(10.0f, 400.0f + i * 20.0f, L"dist[%d] : %.3f", i, dist);

			if (dist <= SphereRadius) {
				pModel[i]->setDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else {
				pModel[i]->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);

			}

		}


	}



	//--------------------------------------
	// マウスによるカメラ操作
	//--------------------------------------
	{
		// 右クリックドラッグで回転
		if (vnMouse::onL()) {
			int dx = vnMouse::getDX();
			int dy = vnMouse::getDY();

			// マウスの移動量に応じて角度を変える
			theta += dx * 0.005f;
			phi -= dy * 0.005f;

			// φが上下に行き過ぎないように制限
			const float limit = XM_PIDIV2 - 0.1f;
			if (phi > limit) phi = limit;
			if (phi < -limit) phi = -limit;
		}

		// ホイールでズーム
		int wheel = vnMouse::getR();
		if (wheel != 0) {
			radius -= wheel * 0.001f;
			if (radius < 1.0f) radius = 1.0f;
			if (radius > 100.0f) radius = 100.0f;
		}

		// 球面座標をデカルト座標に変換してカメラ設定
		float camX = radius * cosf(phi) * cosf(theta);
		float camY = radius * sinf(phi);
		float camZ = radius * cosf(phi) * sinf(theta);

		XMVECTOR camPos = XMVectorSet(camX, camY, camZ, 0.0f);
		XMVECTOR camTarget = XMVectorZero();

		vnCamera::setPosition(&camPos);
		vnCamera::setTarget(&camTarget);
}



	int line = 0;
	vnFont::print(80.0f, (float)(100 + Cursor * 16), L"→");
	// 選択中の頂点やカメラ操作対象を表示
	for (int i = 0; i < 5; i++)
	{
		XMVECTOR p = Point[i];
		vnFont::print(100.0f, 100.0f + (float)(line++ * 16),
			L"Point[%d] X: %.3f Y: %.3f Z: %.3f",
			i, XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p));
	}

	vnFont::print(100.0f, 150.0f + (float)(line++ * 16), L"pPoint: X: %.3f Y: %.3f Z: %.3f", pPoint->getPosition());
	
	//for (int i = 0; i < 2; ++i) {
	//	if () {
	//		pHit[]
	//	}
	//}

	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	vnScene::execute();

}

//描画関数
void ScenePickTest::render()
{

	vnDebugDraw::Line(&Point[0], &Point[1]);	//直線
	vnDebugDraw::Line(&Point[2], &Point[3]);
	vnDebugDraw::Line(&Point[3], &Point[4]);
	vnDebugDraw::Line(&Point[4], &Point[2]);

	vnScene::render();
}
