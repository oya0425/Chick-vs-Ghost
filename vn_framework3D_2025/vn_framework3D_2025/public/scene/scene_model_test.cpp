#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool SceneModelTest::initialize()
{
	//コンストラクタ（フォルダ名、ファイル名）
	pCube = new vnModel(L"data/model/Orca/", L"Orca.vnm");
	//new vnModel(L"data/model/", L"fence.vnm");
	pTheta = new cCube();
	pPhi = new cCube();
	pTheta->setScaleX(0.5);
	pTheta->setScaleY(0.5);
	pTheta->setScaleZ(0.5);

	pPhi->setScaleX(0.5f);
	pPhi->setScaleY(0.5f);
	pPhi->setScaleZ(0.5f);




	resetPos = XMVectorSet(pCube->getPositionX(), pCube->getPositionY(), pCube->getPositionZ(), 0.0f);
	resetRot = XMVectorSet(pCube->getRotationX(), pCube->getRotationY(), pCube->getRotationZ(), 0.0f);
	resetScl = XMVectorSet(pCube->getScaleX(), pCube->getScaleY(), pCube->getScaleZ(), 0.0f);
	resetCam = XMVectorSet(vnCamera::getPositionX(), vnCamera::getPositionY(), vnCamera::getPositionZ(), 0.0f);
	resetTarget = XMVectorSet(vnCamera::getTargetX(), vnCamera::getTargetY(), vnCamera::getTargetZ(), 0.0f);
	radiusRe = 15.0f;
	radius = 15.0f;
	theta = -0.75;
	phi = 0.5f;


	Cursor = 0;

	return true;
}

void SceneModelTest::execute() {
	{

			float value_t = 0.01f;
			float value_r = 1.0f * 3.141592f / 180.0f;
			float value_s = 0.01f;


			if (vnKeyboard::trg(DIK_UP) && --Cursor < 0)
			{
				Cursor = OperationMax - 1;
			}
			else if (vnKeyboard::trg(DIK_DOWN) && ++Cursor >= OperationMax)
			{
				Cursor = 0;
			}
			else if (vnKeyboard::on(DIK_RIGHT))
			{
				switch (Cursor)
				{
				case PositionX:	pCube->addPositionX(value_t);	break;
				case PositionY:	pCube->addPositionY(value_t);	break;
				case PositionZ:	pCube->addPositionZ(value_t);	break;
				case RotateX:	pCube->addRotationX(value_r);	break;
				case RotateY:	pCube->addRotationY(value_r);	break;
				case RotateZ:	pCube->addRotationZ(value_r);	break;
				case ScaleX:	pCube->addScaleX(value_s);		break;
				case ScaleY:	pCube->addScaleY(value_s);		break;
				case ScaleZ:	pCube->addScaleZ(value_s);		break;
				case CamPositonX: vnCamera::addPositionX(value_t);		break;
				case CamPositonY: vnCamera::addPositionY(value_t);		break;
				case CamPositonZ: vnCamera::addPositionZ(value_t);		break;
				case CamTargetX: vnCamera::addTargetX(value_t);		break;
				case CamTargetY: vnCamera::addTargetY(value_t);		break;
				case CamTargetZ: vnCamera::addTargetZ(value_t);		break;
				case RADIUS:   radius += 0.05f;   break;
				case THERA:	   theta += 0.01f;  break;
				case PHI:    if (phi < 1.2)  phi += 0.05f;	   break;

				}
			}
			else if (vnKeyboard::on(DIK_LEFT))
			{
				switch (Cursor)
				{
				case PositionX:	pCube->addPositionX(-value_t);	break;
				case PositionY:	pCube->addPositionY(-value_t);	break;
				case PositionZ:	pCube->addPositionZ(-value_t);	break;
				case RotateX:	pCube->addRotationX(-value_r);	break;
				case RotateY:	pCube->addRotationY(-value_r);	break;
				case RotateZ:	pCube->addRotationZ(-value_r);	break;
				case ScaleX:	pCube->addScaleX(-value_s);		break;
				case ScaleY:	pCube->addScaleY(-value_s);		break;
				case ScaleZ:	pCube->addScaleZ(-value_s);		break;
				case CamPositonX: vnCamera::addPositionX(-value_s);		break;
				case CamPositonY: vnCamera::addPositionY(-value_s);		break;
				case CamPositonZ: vnCamera::addPositionZ(-value_s);		break;
				case CamTargetX: vnCamera::addTargetX(-value_s);		break;
				case CamTargetY: vnCamera::addTargetY(-value_s);		break;
				case CamTargetZ: vnCamera::addTargetZ(-value_s);		break;
				case RADIUS:   radius -= 0.05f;   break;
				case THERA:	   theta -= 0.01f;  break;
				case PHI:     if (phi > -1.2) phi -= 0.05f;	   break;


				}
			}
			else if (vnKeyboard::on(DIK_SPACE))
			{
				switch (Cursor)
				{
				case PositionX:	pCube->setPositionX(XMVectorGetX(resetPos));	break;
				case PositionY:	pCube->setPositionY(XMVectorGetY(resetPos));	break;
				case PositionZ:	pCube->setPositionZ(XMVectorGetZ(resetPos));	break;
				case RotateX:	pCube->setRotationX(XMVectorGetX(resetRot));	break;
				case RotateY:	pCube->setRotationY(XMVectorGetY(resetRot));	break;
				case RotateZ:	pCube->setRotationZ(XMVectorGetZ(resetRot));	break;
				case ScaleX:	pCube->setScaleX(XMVectorGetX(resetScl));		break;
				case ScaleY:	pCube->setScaleY(XMVectorGetY(resetScl));		break;
				case ScaleZ:	pCube->setScaleZ(XMVectorGetZ(resetScl));		break;
				case CamPositonX: vnCamera::setPositionX(XMVectorGetX(resetCam));		break;
				case CamPositonY: vnCamera::setPositionY(XMVectorGetY(resetCam));		break;
				case CamPositonZ: vnCamera::setPositionZ(XMVectorGetZ(resetCam));		break;
				case CamTargetX: vnCamera::setTargetX(XMVectorGetX(resetTarget));		break;
				case CamTargetY: vnCamera::setTargetY(XMVectorGetY(resetTarget));		break;
				case CamTargetZ: vnCamera::setTargetZ(XMVectorGetZ(resetTarget));		break;
				case RADIUS:   radius = radiusRe;   break;

				}
			}

			/*
			画面に文字を出す
			　vnFont::print(x,y,L"文字列");

			 文字列部分は書式設定可能
			 %d,%f,%s

			 int a = 0;
			 printf("a = %d",a);

			 可変引数(variable argument)引数を増やす

			 */


			 /* マウスによるカメラの操作

			 vnMouse::on()   :ボタンの押下(onL())
			 trg()  :押した瞬間
			 　rel()  :離した瞬間
			  getX() :カーソルのX座標（スクリーン座標）
			  getY() :
			  getDX():移動量（前フレームからの移動量）
			  getDY():移動量（前フレームからの移動量）

			  Unityでのカメラ操作(3Dソフト業界標準)
			  Alt +左ドラッグ:カメラの回転(theta,phi)
			  Alt +右ドラッグ:カメラの遠近(radius)
			  Alt +中ドラッグ:カメラのスライド（ｘ）

			  DIK_LMENU

			  phi90°を超えない-90°下回らない
			  radius 0以下にしない
			  ※

			  radius
			  theta
			  phi

			  */
			  //if (vnMouse::onL()) {
			  //	theta += 5;
			  //}
			  //if (vnMouse::onL()&&(vnMouse::getX()>=1280/2)) {
			  //	theta += 5;
			  //}
			  //if (vnMouse::onL() && vnMouse::getDX() >= 40) {
			  //	theta += 5;
			  //}
			  //視点移動のみ　のこり拡大縮小
			  // 毎フレーム時に
			if (vnMouse::onL()) { // クリックしている間
				theta += vnMouse::getDX() * 0.005;
				phi -= vnMouse::getDY() * 0.005;

				// 角度の制約
				if (phi > 0.9f)  phi = 0.9f;
				if (phi < -0.9f) phi = -0.9f;

				if (radius < 0.0f) radius = 0.0f;
			}




			//極座標→直交座標
			float tx = radius * cosf(theta);
			float tz = radius * sinf(theta);

			pTheta->setPositionX(tx);
			pTheta->setPositionY(tz);

			float px = radius * cosf(theta) * cos(phi);
			float py = radius * sinf(phi);
			float pz = radius * sinf(theta) * cos(phi);

			pPhi->setPositionX(px);
			pPhi->setPositionY(py);
			pPhi->setPositionZ(pz);

			//カメラの位置を極座標(ｒ、θ、φ)で表した座標にする
			vnCamera::setPositionX(px);
			vnCamera::setPositionY(py);
			vnCamera::setPositionZ(pz);


			int line = 0;
			vnFont::print(80.0f, (float)(100 + Cursor * 16), L"→");
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionX : %.3f", pCube->getPositionX());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionY : %.3f", pCube->getPositionY());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"PositionZ : %.3f", pCube->getPositionZ());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationX : %.3f", pCube->getRotationX() / 3.141592f * 180.0f);
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationY : %.3f", pCube->getRotationY() / 3.141592f * 180.0f);
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"RotationZ : %.3f", pCube->getRotationZ() / 3.141592f * 180.0f);
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleX : %.3f", pCube->getScaleX());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleY : %.3f", pCube->getScaleY());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"ScaleZ : %.3f", pCube->getScaleZ());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPositionX: %.3f", vnCamera::getPositionX());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPositionY: %.3f", vnCamera::getPositionY());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamPositionZ: %.3f", vnCamera::getPositionZ());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTargetX: %.3f", vnCamera::getTargetX());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTargetY: %.3f", vnCamera::getTargetY());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"CamTargetZ: %.3f", vnCamera::getTargetZ());
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Radius: %.3f", radius);
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Theta: %.3f", theta);
			vnFont::print(100.0f, 100.0f + (float)(line++ * 16), L"Phi: %.3f", phi);



			vnDebugDraw::Grid();
			vnDebugDraw::Axis();

			vnScene::execute();
		}
	
}

//終了関数
void SceneModelTest::terminate()
{
	//オブジェクトの削除
	delete pCube;
	delete pTheta;
	delete pPhi;
}


//描画関数
void SceneModelTest::render()
{
	//ビューポートの設定(画面中央正方形)
	//D3D12_VIEWPORT	Viewport;
	//Viewport.TopLeftX = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2;
	//Viewport.TopLeftY = 0;
	//Viewport.Width = (float)SCREEN_HEIGHT;
	//Viewport.Height = (float)SCREEN_HEIGHT;
	//Viewport.MinDepth = 0.0f;
	//Viewport.MaxDepth = 1.0f;
	//vnDirect3D::getCommandList()->RSSetViewports(1, &Viewport);

	//キューブの描画
	pCube->render();
	pTheta->render();
	pPhi->render();



	//ビューポートの設定(フルスクリーンに戻す)
	//Viewport.TopLeftX = 0;
	//Viewport.Width = (float)SCREEN_WIDTH;
	//vnDirect3D::getCommandList()->RSSetViewports(1, &Viewport);

	vnScene::render();
}
